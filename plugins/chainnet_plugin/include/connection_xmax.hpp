/**
*  @file
*  @copyright defined in xmax/LICENSE
*/
#pragma once
#include<memory.h>
#include <blockchain_types.hpp>
#include<protocol.hpp>
#include<message_buffer.hpp>


#include <fc/network/ip.hpp>
#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/log/appender.hpp>
#include <fc/container/flat.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/crypto/rand.hpp>
#include <fc/exception/exception.hpp>
#include <fc/time.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/intrusive/set.hpp>
#include <objects/multi_index_includes.hpp>

namespace Xmaxplatform {
	using namespace Chain;
	using namespace boost::multi_index;
	using boost::asio::ip::tcp;
	using boost::asio::ip::address_v4;
	using boost::asio::ip::host_name;
	using boost::intrusive::rbtree;
	using socket_ptr = std::shared_ptr<tcp::socket>;
	class connection_xmax;
	using connection_ptr = std::shared_ptr<connection_xmax>;
	using connection_wptr = std::weak_ptr<connection_xmax>;
	
	struct node_transaction_state {
		xmax_type_transaction_id id;
		time_point_sec  expires;  /// time after which this may be purged.
								  /// Expires increased while the txn is
								  /// "in flight" to anoher peer
		Chain::vector<char>    packed_transaction; /// the received raw bundle
		uint32_t        block_num = 0; /// block transaction was included in
		uint32_t        true_block = 0; /// used to reset block_uum when request is 0
		uint16_t        requests = 0; /// the number of "in flight" requests for this txn
	};
	/**
	* Index by start_block_num
	*/
	struct sync_state {
		sync_state(uint32_t start = 0, uint32_t end = 0, uint32_t last_acted = 0)
			:start_block(start), end_block(end), last(last_acted),
			start_time(time_point::now())//, block_cache()
		{}
		uint32_t     start_block;
		uint32_t     end_block;
		uint32_t     last; ///< last sent or received
		time_point   start_time; ///< time request made or received
	};
	using sync_state_ptr = shared_ptr< sync_state >;
	
	struct transaction_state {
		xmax_type_transaction_id id;
		bool                is_known_by_peer = false; ///< true if we sent or received this trx to this peer or received notice from peer
		bool                is_noticed_to_peer = false; ///< have we sent peer notice we know it (true if we receive from this peer)
		uint32_t            block_num = 0; ///< the block number the transaction was included in
		time_point          requested_time; /// in case we fetch large trx
	};

	typedef multi_index_container<
		transaction_state,
		indexed_by<
		ordered_unique< tag<by_id>, member<transaction_state, xmax_type_transaction_id, &transaction_state::id > >
		>
	> transaction_state_index;

	
	

	struct queued_write {
		std::shared_ptr<Chain::vector<char>> buff;
		std::function<void(boost::system::error_code, std::size_t)> cb;
	};

	struct update_block_num {
		uint32_t new_bnum;
		update_block_num(uint32_t bnum) : new_bnum(bnum) {}
		void operator() (node_transaction_state& nts) {
			if (nts.requests) {
				nts.true_block = new_bnum;
			}
			else {
				nts.block_num = new_bnum;
			}
		}
		void operator() (transaction_state& ts) {
			ts.block_num = new_bnum;
		}
	};

	

	/**
	*
	*/
	struct block_state {
		xmax_type_block_id id;
		bool          is_known;
		bool          is_noticed;
		time_point    requested_time;
	};

	typedef multi_index_container<
		block_state,
		indexed_by<
		ordered_unique< tag<by_id>, member<block_state, xmax_type_block_id, &block_state::id > >
		>
	> block_state_index;

	class chainnet_plugin_impl;
	struct msgHandler : public fc::visitor<void> {
		chainnet_plugin_impl &impl;
		connection_ptr c;
		msgHandler(chainnet_plugin_impl &imp, connection_ptr conn) : impl(imp), c(conn) {}

		template <typename T>
		void operator()(const T &msg) const
		{
			impl.handle_message(c, msg);
		}
	};

	struct connection_status {
		Chain::string            peer;
		bool              connecting = false;
		bool              syncing = false;
		handshake_message last_handshake;
	};


	

	class connection_xmax : public std::enable_shared_from_this<connection_xmax> {
	public:
		explicit connection_xmax(Chain::string endpoint);

		explicit connection_xmax(socket_ptr s);
		~connection_xmax();

		

		void initialize();

		block_state_index       blk_state;
		transaction_state_index trx_state;
		sync_state_ptr          sync_requested;  // this peer is requesting info from us
		socket_ptr              socket;

		message_buffer<1024 * 1024>    pending_message_buffer;
		Chain::vector<char>            blk_buffer;

		
		

		

		



		


		deque<queued_write>     write_queue;

		fc::sha256              node_id;
		handshake_message       last_handshake_recv;
		handshake_message       last_handshake_sent;
		int16_t                 sent_handshake_count;
		bool                    connecting;
		bool                    syncing;
		int                     write_depth;
		Chain::string                  peer_addr;
		unique_ptr<boost::asio::steady_timer> response_expected;
		optional<request_message> pending_fetch;
		leave_reason         no_retry;
		xmax_type_block_id          fork_head;
		uint32_t               fork_head_num;

		connection_status get_status()const {
			connection_status stat;
			stat.peer = peer_addr;
			stat.connecting = connecting;
			stat.syncing = syncing;
			stat.last_handshake = last_handshake_recv;
			return stat;
		}

		/** \name Peer Timestamps
		*  Time message handling
		*  @{
		*/
		// Members set from network data
		tstamp                         org{ 0 };          //!< originate timestamp
		tstamp                         rec{ 0 };          //!< receive timestamp
		tstamp                         dst{ 0 };          //!< destination timestamp
		tstamp                         xmt{ 0 };          //!< transmit timestamp

														  // Computed data
		double                         offset{ 0 };       //!< peer offset

		static const size_t            ts_buffer_size{ 32 };
		char                           ts[ts_buffer_size];          //!< working buffer for making human readable timestamps

		bool connected();
		bool current();
		void reset();
		void close();
		void send_handshake();

		/** \name Peer Timestamps
		*  Time message handling
		*/
		/** @{ */
		/** \brief Convert an std::chrono nanosecond rep to a human readable string
		*/
		char* convert_tstamp(const tstamp& t);
		/**  \brief Populate and queue time_message
		*/
		void send_time();
		/** \brief Populate and queue time_message immediately using incoming time_message
		*/
		void send_time(const time_message& msg);
		/** \brief Read system time and convert to a 64 bit integer.
		*
		* There are only two calls on this routine in the program.  One
		* when a packet arrives from the network and the other when a
		* packet is placed on the send queue.  Calls the kernel time of
		* day routine and converts to a (at least) 64 bit integer.
		*/
		tstamp get_time()
		{
			return std::chrono::system_clock::now().time_since_epoch().count();
		}
		/** @} */

		const Chain::string peer_name();

		void txn_send_pending(const Chain::vector<xmax_type_transaction_id> &ids);
		void txn_send(const Chain::vector<xmax_type_transaction_id> &txn_lis);

		void blk_send_branch();
		void blk_send(const Chain::vector<xmax_type_block_id> &txn_lis);
		void stop_send();

		void msg_enqueue(xmax_type_transaction_id id);
		void msg_enqueue(const net_message &msg, bool trigger_send = true);
		void enqueue(const net_message &msg, bool trigger_send = true);
		void cancel_sync(leave_reason);
		void cancel_fetch();
		void flush_queues();
		bool enqueue_sync_block();

		void cancel_wait();
		void sync_wait();
		void fetch_wait();
		void sync_timeout(boost::system::error_code ec);
		void fetch_timeout(boost::system::error_code ec);

		void queue_write(std::shared_ptr<Chain::vector<char>> buff,
			bool trigger_send,
			std::function<void(boost::system::error_code, std::size_t)> cb);
		void do_queue_write();

		/** \brief Process the next message from the pending message buffer
		*
		* Process the next message from the pending_message_buffer.
		* message_length is the already determined length of the data
		* part of the message and impl in the net plugin implementation
		* that will handle the message.
		* Returns true is successful. Returns false if an error was
		* encountered unpacking or processing the message.
		*/
		bool process_next_message(chainnet_plugin_impl& impl, uint32_t message_length);
		static const fc::string logger_name;
		static fc::logger logger;
	};
	struct handshake_initializer {
		static void setup(handshake_message &hello);
	};

}

FC_REFLECT(Xmaxplatform::connection_status, (peer)(connecting)(syncing)(last_handshake))