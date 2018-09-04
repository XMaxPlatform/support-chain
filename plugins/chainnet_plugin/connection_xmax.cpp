/**
*  @file
*  @copyright defined in xmax/LICENSE
*/

#include <blockchain_exceptions.hpp>
#include <blockchain_plugin.hpp>
//#include <chainnet_plugin.hpp>
#include <connection_xmax.hpp>
#include <application.hpp>
#include <chain_xmax.hpp>

using namespace Baseapp;
namespace Xmaxplatform {

	connection_xmax::connection_xmax(Chain::string endpoint)
		: blk_state(),
		trx_state(),
		sync_requested(),
		socket(std::make_shared<tcp::socket>(std::ref(app().get_io_service()))),
		node_id(),
		last_handshake_recv(),
		last_handshake_sent(),
		sent_handshake_count(0),
		connecting(false),
		syncing(false),
		write_depth(0),
		peer_addr(endpoint),
		response_expected(),
		pending_fetch(),
		no_retry(no_reason),
		fork_head(),
		fork_head_num(0)
	{
		wlog("created connection to ${n}", ("n", endpoint));
		initialize();
	}

	connection_xmax::connection_xmax(socket_ptr s)
		: blk_state(),
		trx_state(),
		sync_requested(),
		socket(s),
		node_id(),
		last_handshake_recv(),
		last_handshake_sent(),
		sent_handshake_count(0),
		connecting(true),
		syncing(false),
		write_depth(0),
		peer_addr(),
		response_expected(),
		pending_fetch(),
		no_retry(no_reason),
		fork_head(),
		fork_head_num(0)
	{
		wlog("accepted network connection");
		initialize();
	}

	connection_xmax::~connection_xmax() {
		if (peer_addr.empty())
			wlog("released connection from client");
		else
			wlog("released connection to server at ${addr}", ("addr", peer_addr));
	}


	void connection_xmax::initialize() {
		auto *rnd = node_id.data();
		rnd[0] = 0;
		response_expected.reset(new boost::asio::steady_timer(app().get_io_service()));
	}

	bool connection_xmax::connected() {
		return (socket->is_open() && !connecting);
	}

	bool connection_xmax::current() {
		return (connected() && !syncing);
	}

	void connection_xmax::reset() {
		sync_requested.reset();
		blk_state.clear();
		trx_state.clear();
	}

	void connection_xmax::flush_queues() {
		if (write_depth > 0) {
			while (write_queue.size() > 1) {
				write_queue.pop_back();
			}
		}
		else {
			write_queue.clear();
		}
	}

	void connection_xmax::close() {
		if (socket) {
			socket->close();
		}
		else {
			wlog("no socket to close!");
		}
		flush_queues();
		connecting = false;
		syncing = false;
		reset();
		sent_handshake_count = 0;
		last_handshake_recv = handshake_message();
		last_handshake_sent = handshake_message();
		cancel_wait();
		pending_message_buffer.reset();
	}


	void connection_xmax::stop_send() {
		syncing = false;
	}

	void connection_xmax::send_handshake() {
		handshake_initializer::setup(last_handshake_sent);
		last_handshake_sent.generation = ++sent_handshake_count;
		fc_dlog(logger, "Sending handshake generation ${g} to ${ep}",
			("g", last_handshake_sent.generation)("ep", peer_addr));
		msg_enqueue(last_handshake_sent);
	}

	void connection_xmax::send_signedblock(const Chain::signed_block &sb)
	{
		fc_dlog(logger, "send signedblock to ${ep}\n", ("ep", peer_addr));
		msg_enqueue(sb);
	}

	void connection_xmax::send_blockconfirm(const Chain::block_confirmation& confirm)
	{
		msg_enqueue(confirm);
	}

	void connection_xmax::send_signedblocklist(const Chain::signed_block_list& blockList)
	{
		fc_dlog(logger, "send signedblocklist to ${ep}\n", ("ep", peer_addr));
		msg_enqueue(blockList);
	}

	char* connection_xmax::convert_tstamp(const tstamp& t)
	{
		const long long NsecPerSec{ 1000000000 };
		time_t seconds = t / NsecPerSec;
		strftime(ts, ts_buffer_size, "%F %T", localtime(&seconds));
		snprintf(ts + 19, ts_buffer_size - 19, ".%lld", t % NsecPerSec);
		return ts;
	}

	void connection_xmax::send_time() {
		time_message xpkt;
		xpkt.org = rec;
		xpkt.rec = dst;
		xpkt.xmt = get_time();
		org = xpkt.xmt;
		msg_enqueue(xpkt);
	}

	void connection_xmax::send_time(const time_message& msg) {
		time_message xpkt;
		xpkt.org = msg.xmt;
		xpkt.rec = msg.dst;
		xpkt.xmt = get_time();
		msg_enqueue(xpkt);
	}

	void connection_xmax::queue_write(std::shared_ptr<Chain::vector<char>> buff,
		bool trigger_send,
		std::function<void(boost::system::error_code, std::size_t)> cb) {
		write_queue.push_back({ buff, cb });
		if (write_queue.size() == 1 && trigger_send)
			do_queue_write();
	}


	void connection_xmax::cancel_sync(leave_reason reason) {
		fc_dlog(logger, "cancel sync reason = ${m}, write queue size ${o} peer ${p}",
			("m", reason_str(reason)) ("o", write_queue.size())("p", peer_name()));
		cancel_wait();
		flush_queues();
		switch (reason) {
		case validation:
		case fatal_other: {
			no_retry = reason;
			msg_enqueue(leave_message(reason));
			break;
		}
		default:
			//TODO
			//enqueue( ( sync_request_message ) {0,0} );
			break;
		}
	}

	void connection_xmax::cancel_fetch() {
		msg_enqueue(request_message());
	}

	bool connection_xmax::enqueue_sync_block() {
		chain_xmax& cc = app().find_plugin<blockchain_plugin>()->getchain();
		if (!sync_requested)
			return false;
		uint32_t num = ++sync_requested->last;
		bool trigger_send = num == sync_requested->start_block;
		if (num == sync_requested->end_block) {
			sync_requested.reset();
		}
		try {
			fc::optional<signed_block> sb = *cc.block_from_num(num);
			if (sb) {
				msg_enqueue(*sb, trigger_send);
				return true;
			}
		}
		catch (...) {
			wlog("write loop exception");
		}
		return false;
	}


	void connection_xmax::cancel_wait() {
		if (response_expected)
			response_expected->cancel();
	}


	void connection_xmax::sync_timeout(boost::system::error_code ec) {
		if (!ec) {
			//TODO
			elog("connection::sync_timeout ${ec} TODO");
		}
		else if (ec == boost::asio::error::operation_aborted) {
		}
		else {
			elog("setting timer for sync request got error ${ec}", ("ec", ec.message()));
		}
	}

	const Chain::string connection_xmax::peer_name() {
		if (!last_handshake_recv.p2p_address.empty()) {
			return last_handshake_recv.p2p_address;
		}
		if (!peer_addr.empty()) {
			return peer_addr;
		}
		return "connecting client";
	}

	void connection_xmax::fetch_timeout(boost::system::error_code ec) {
		if (!ec) {
			if (!(pending_fetch->req_trx.empty() || pending_fetch->req_blocks.empty())) {
				cancel_fetch();
			}
		}
		else if (ec == boost::asio::error::operation_aborted) {
			if (!connected()) {
				fc_dlog(logger, "fetch timeout was cancelled due to dead connection");
			}
		}
		else {
			elog("setting timer for fetch request got error ${ec}", ("ec", ec.message()));
		}
	}

	std::string connection_xmax::get_connecting_endpoint()
	{
		std::string host;
		if (socket != nullptr && socket->is_open())
		{
			std::string ip = socket->remote_endpoint().address().to_string();
			std::string port =  std::to_string(socket->remote_endpoint().port());
			host = ip + ":" + port;
		}

		return host;
	}

	void connection_xmax::send_connection_iplist(const connecting_nodes_message& msg)
	{
		enqueue(msg);
	}
}