/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#include <xmaxtypes.hpp>
#include <chainnet_plugin.hpp>
#include <blockbuilder_plugin.hpp>
#include <message_buffer.hpp>
#include <fc/network/ip.hpp>
#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/log/appender.hpp>
#include <fc/container/flat.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/crypto/rand.hpp>
#include <fc/exception/exception.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/multi_index_container.hpp>

#include<blockchain_exceptions.hpp>
namespace fc {
   extern std::unordered_map<std::string,logger>& get_logger_map();
}

namespace Xmaxplatform {
   static Baseapp::abstract_plugin& _net_plugin = app().register_plugin<chainnet_plugin>();

   using namespace boost::multi_index;

   using Chain::vector;

   using boost::asio::ip::tcp;
   using boost::asio::ip::address_v4;
   using boost::asio::ip::host_name;
   using boost::intrusive::rbtree;

   using fc::time_point;
   using fc::time_point_sec;
   namespace bip = boost::interprocess;

   class connection_xmax;

   using connection_ptr = std::shared_ptr<connection_xmax>;
   using connection_wptr = std::weak_ptr<connection_xmax>;

   using socket_ptr = std::shared_ptr<tcp::socket>;

   using net_message_ptr = std::shared_ptr<net_message>;

   struct update_in_flight {
	   int32_t incr;
	   update_in_flight(int32_t delta) : incr(delta) {}
	   void operator() (node_transaction_state& nts) {
		   int32_t exp = nts.expires.sec_since_epoch();
		   nts.expires = fc::time_point_sec(exp + incr * 60);
		   if (nts.requests == 0) {
			   nts.true_block = nts.block_num;
			   nts.block_num = 0;
		   }
		   nts.requests += incr;
		   if (nts.requests == 0) {
			   nts.block_num = nts.true_block;
		   }
	   }
   } incr_in_flight(1), decr_in_flight(-1);

   struct update_entry {
      const Chain::signed_transaction &txn;
      update_entry(const Chain::signed_transaction &msg) : txn(msg) {}

      void operator() (node_transaction_state& nts) {
         net_message msg(txn);
         uint32_t packsiz = fc::raw::pack_size(msg);
         uint32_t bufsiz = packsiz + sizeof(packsiz);
         nts.packed_transaction.resize(bufsiz);
         fc::datastream<char*> ds( nts.packed_transaction.data(), bufsiz );
         ds.write( reinterpret_cast<char*>(&packsiz), sizeof(packsiz) );
         fc::raw::pack( ds, msg );
      }
   };

   

   struct by_expiry;
   struct by_block_num;

   typedef multi_index_container<
      node_transaction_state,
      indexed_by<
         ordered_unique<
            tag< by_id >,
            member < node_transaction_state,
                     xmax_type_transaction_id,
                     &node_transaction_state::id > >,
         ordered_non_unique<
            tag< by_expiry >,
            member< node_transaction_state,
                    fc::time_point_sec,
                    &node_transaction_state::expires >
            >,

         ordered_non_unique<
            tag<by_block_num>,
            member< node_transaction_state,
                    uint32_t,
                    &node_transaction_state::block_num > >
         >
      >
   node_transaction_index;

   class chainnet_plugin_impl {
   public:
      unique_ptr<tcp::acceptor>        acceptor;
      tcp::endpoint                    listen_endpoint;
	  Chain::string                           p2p_address;
      uint32_t                         max_client_count = 0;
      uint32_t                         num_clients = 0;

	  Chain::vector<Chain::string>                   supplied_peers;
	  Chain::vector<Chain::public_key_type>   allowed_peers; ///< peer keys allowed to connect
      std::map<Chain::public_key_type,
               fc::ecc::private_key>   private_keys; ///< overlapping with builder keys, also authenticating non-producing nodes

      enum possible_connections : char {
         None = 0,
            Builders = 1 << 0,
            Specified = 1 << 1,
            Any = 1 << 2
            };
      possible_connections             allowed_connections{None};

      connection_ptr find_connection(Chain::string host )const;

      std::set< connection_ptr >       connections;
      bool                             done = false;

      unique_ptr<boost::asio::steady_timer> connector_check;
      unique_ptr<boost::asio::steady_timer> transaction_check;
      unique_ptr<boost::asio::steady_timer> keepalive_timer;
      boost::asio::steady_timer::duration   connector_period;
      boost::asio::steady_timer::duration   txn_exp_period;
      boost::asio::steady_timer::duration   resp_expected_period;
      boost::asio::steady_timer::duration   keepalive_interval{std::chrono::seconds{32}};

      const std::chrono::system_clock::duration peer_authentication_interval{std::chrono::seconds{1}}; ///< Peer clock may be no more than 1 second skewed from our clock, including network latency.

      int16_t                       network_version = 0;
      bool                          network_version_match = false;
      chain_id_type                 chain_id;
      fc::sha256                    node_id;

	  Chain::string                        user_agent_name;
      blockchain_plugin*                 chain_plug;
      bool                          send_whole_blocks = false;
      int                           started_sessions = 0;

      node_transaction_index        local_txns;

      shared_ptr<tcp::resolver>     resolver;

      void connect( connection_ptr c );
      void connect( connection_ptr c, tcp::resolver::iterator endpoint_itr );
      void start_session( connection_ptr c );
      void start_listen_loop( );
      void start_read_message( connection_ptr c);

      void   close( connection_ptr c );
      size_t count_open_sockets() const;

      template<typename VerifierFunc>
      void send_all( const net_message &msg, VerifierFunc verify );

      static void transaction_ready( const Chain::signed_transaction& txn);
      void broadcast_block_impl( const signed_block &sb);

      bool is_valid( const handshake_message &msg);

      void handle_message( connection_ptr c, const handshake_message &msg);
      void handle_message( connection_ptr c, const leave_message &msg );
      /** \name Peer Timestamps
       *  Time message handling
       *  @{
       */
      /** \brief Process time_message
       *
       * Calculate offset, delay and dispersion.  Note carefully the
       * implied processing.  The first-order difference is done
       * directly in 64-bit arithmetic, then the result is converted
       * to floating double.  All further processing is in
       * floating-double arithmetic with rounding done by the hardware.
       * This is necessary in order to avoid overflow and preserve precision.
       */
      void handle_message( connection_ptr c, const time_message &msg);
      /** @} */
      void handle_message( connection_ptr c, const notice_message &msg);
      void handle_message( connection_ptr c, const request_message &msg);
      void handle_message( connection_ptr c, const sync_request_message &msg);
      void handle_message( connection_ptr c, const block_summary_message &msg);
      void handle_message( connection_ptr c, const Chain::signed_transaction &msg);
      void handle_message( connection_ptr c, const signed_block &msg);

      void start_conn_timer( );
      void start_txn_timer( );
      void start_monitors( );

      void expire_txns( );
      void connection_monitor( );
      /** \name Peer Timestamps
       *  Time message handling
       *  @{
       */
      /** \brief Peer heartbeat ticker.
       */
      void ticker();
      /** @} */
      /** \brief Determine if a peer is allowed to connect.
       *
       * Checks current connection mode and key authentication.
       *
       * \return False if the peer should not connect, true otherwise.
       */
      bool authenticate_peer(const handshake_message& msg) const;

      /** \brief Returns a signature of the digest using the corresponding private key of the signer.
       *
       * If there are no configured private keys, returns an empty signature.
       */
      fc::ecc::compact_signature sign_compact(const Chain::public_key_type& signer, const fc::sha256& digest) const;

      static const fc::string logger_name;
      static fc::logger logger;
   };

   template<class enum_type, class=typename std::enable_if<std::is_enum<enum_type>::value>::type>
   inline enum_type& operator|=(enum_type& lhs, const enum_type& rhs)
   {
      using T = std::underlying_type_t <enum_type>;
      return lhs = static_cast<enum_type>(static_cast<T>(lhs) | static_cast<T>(rhs));
   }


   static chainnet_plugin_impl *cnet_impl;
   /**
    * default value initializers
    */
   constexpr auto     def_send_buffer_size_mb = 4;
   constexpr auto     def_send_buffer_size = 1024*1024*def_send_buffer_size_mb;
   constexpr auto     def_max_clients = 25; // 0 for unlimited clients
   constexpr auto     def_conn_retry_wait = 30;
   constexpr auto     def_txn_expire_wait = std::chrono::seconds(3);
   constexpr auto     def_resp_expected_wait = std::chrono::seconds(1);
   constexpr auto     def_sync_fetch_span = 100;
   constexpr auto     def_max_just_send = 1500 * 3; // "mtu" * 3
   constexpr auto     def_send_whole_blocks = true;

   constexpr auto     message_header_size = 4;

   const fc::string chainnet_plugin_impl::logger_name("chainnet_plugin_impl");
   fc::logger chainnet_plugin_impl::logger(chainnet_plugin_impl::logger_name);
   const fc::string connection_xmax::logger_name("connection_xmax");
   fc::logger connection_xmax::logger(connection_xmax::logger_name);


   void chainnet_plugin_impl::connect( connection_ptr c ) {
      if( c->no_retry != leave_reason::no_reason) {
         fc_dlog( logger, "Skipping connect due to go_away reason ${r}",("r", reason_str( c->no_retry )));
         return;
      }

      auto colon = c->peer_addr.find(':');

      if (colon == std::string::npos || colon == 0) {
         elog ("Invalid peer address. must be \"host:port\": ${p}", ("p",c->peer_addr));
         return;
      }

      auto host = c->peer_addr.substr( 0, colon );
      auto port = c->peer_addr.substr( colon + 1);
      idump((host)(port));
      tcp::resolver::query query( tcp::v4(), host.c_str(), port.c_str() );
      // Note: need to add support for IPv6 too

      resolver->async_resolve( query,
                               [c, this]( const boost::system::error_code& err,
                                          tcp::resolver::iterator endpoint_itr ){
                                  if( !err ) {
                                     connect( c, endpoint_itr );
                                  } else {
                                     elog( "Unable to resolve ${peer_addr}: ${error}",
                                           (  "peer_addr", c->peer_name() )("error", err.message() ) );
                                  }
                               });
   }

   void chainnet_plugin_impl::connect( connection_ptr c, tcp::resolver::iterator endpoint_itr ) {
      if( c->no_retry != leave_reason::no_reason) {
		  Chain::string rsn = reason_str(c->no_retry);
         return;
      }
      auto current_endpoint = *endpoint_itr;
      ++endpoint_itr;
      c->connecting = true;
      c->socket->async_connect( current_endpoint, [c, endpoint_itr, this] ( const boost::system::error_code& err ) {
            if( !err ) {
               start_session( c );
               c->send_handshake ();
            } else {
               if( endpoint_itr != tcp::resolver::iterator() ) {
                  c->close();
                  connect( c, endpoint_itr );
               }
               else {
                  elog( "connection failed to ${peer}: ${error}",
                        ( "peer", c->peer_name())("error",err.message()));
                  c->connecting = false;
                  cnet_impl->close(c);
               }
            }
         } );
   }

   void chainnet_plugin_impl::start_session( connection_ptr con ) {
      boost::asio::ip::tcp::no_delay nodelay( true );
      con->socket->set_option( nodelay );
      start_read_message( con );
      ++started_sessions;

      // for now, we can just use the application main loop.
      //     con->readloop_complete  = bf::async( [=](){ read_loop( con ); } );
      //     con->writeloop_complete = bf::async( [=](){ write_loop con ); } );
   }


   void chainnet_plugin_impl::start_listen_loop( ) {
      auto socket = std::make_shared<tcp::socket>( std::ref( app().get_io_service() ) );
      acceptor->async_accept( *socket, [socket,this]( boost::system::error_code ec ) {
            if( !ec ) {
               uint32_t visitors = 0;
               for (auto &conn : connections) {
                  if(conn->current() && conn->peer_addr.empty()) {
                     visitors++;
                  }
               }
               if (num_clients != visitors) {
                  ilog ("checking max client, visitors = ${v} num clients ${n}",("v",visitors)("n",num_clients));
                  num_clients = visitors;
               }
               if( max_client_count == 0 || num_clients < max_client_count ) {
                  ++num_clients;
                  connection_ptr c = std::make_shared<connection_xmax>( socket );
                  connections.insert( c );
                  start_session( c );
               } else {
                  elog( "Error max_client_count ${m} exceeded",
                        ( "m", max_client_count) );
                  socket->close( );
               }
               start_listen_loop();
            } else {
               elog( "Error accepting connection: ${m}",( "m", ec.message() ) );
            }
         });
   }

   void chainnet_plugin_impl::start_read_message( connection_ptr conn ) {

      try {
         if(!conn->socket) {
            return;
         }
         conn->socket->async_read_some
            (conn->pending_message_buffer.get_buffer_sequence_for_boost_async_read(),
             [this,conn]( boost::system::error_code ec, std::size_t bytes_transferred ) {
               try {
                  if( !ec ) {
                     if (bytes_transferred > conn->pending_message_buffer.bytes_to_write()) {
                        elog("async_read_some callback: bytes_transfered = ${bt}, buffer.bytes_to_write = ${btw}",
                             ("bt",bytes_transferred)("btw",conn->pending_message_buffer.bytes_to_write()));
                     }
                     FC_ASSERT(bytes_transferred <= conn->pending_message_buffer.bytes_to_write());
                     conn->pending_message_buffer.advance_write_ptr(bytes_transferred);
                     while (conn->pending_message_buffer.bytes_to_read() > 0) {
                        uint32_t bytes_in_buffer = conn->pending_message_buffer.bytes_to_read();

                        if (bytes_in_buffer < message_header_size) {
                           break;
                        } else {
                           uint32_t message_length;
                           auto index = conn->pending_message_buffer.read_index();
                           conn->pending_message_buffer.peek(&message_length, sizeof(message_length), index);
                           if(message_length > def_send_buffer_size*2) {
                              elog("incoming message length unexpected (${i})", ("i", message_length));
                              close(conn);
                              return;
                           }
                           if (bytes_in_buffer >= message_length + message_header_size) {
                              conn->pending_message_buffer.advance_read_ptr(message_header_size);
                              if (!conn->process_next_message(*this, message_length)) {
                                 return;
                              }
                           } else {
                              conn->pending_message_buffer.add_space(message_length + message_header_size - bytes_in_buffer);
                              break;
                           }
                        }
                     }
                     start_read_message(conn);
                  } else {
                     auto pname = conn->peer_name();
                     if (ec.value() != boost::asio::error::eof) {
                        elog( "Error reading message from ${p}: ${m}",("p",pname)( "m", ec.message() ) );
                     } else {
                        ilog( "Peer ${p} closed connection",("p",pname) );
                     }
                     close( conn );
                  }
               }
               catch(const std::exception &ex) {
				   Chain::string pname = conn ? conn->peer_name() : "no connection name";
                  elog("Exception in handling read data from ${p} ${s}",("p",pname)("s",ex.what()));
                  close( conn );
               }
               catch(const fc::exception &ex) {
				   Chain::string pname = conn ? conn->peer_name() : "no connection name";
                  elog("Exception in handling read data ${s}", ("p",pname)("s",ex.to_string()));
                  close( conn );
               }
               catch (...) {
				   Chain::string pname = conn ? conn->peer_name() : "no connection name";
                  elog( "Undefined exception hanlding the read data from connection ${p}",( "p",pname));
                  close( conn );
               }
            } );
      } catch (...) {
		  Chain::string pname = conn ? conn->peer_name() : "no connection name";
         elog( "Undefined exception handling reading ${p}",("p",pname) );
         close( conn );
      }
   }

   size_t chainnet_plugin_impl::count_open_sockets() const
   {
      size_t count = 0;
      for( auto &c : connections) {
         if(c->socket->is_open())
            ++count;
      }
      return count;
   }


   template<typename VerifierFunc>
   void chainnet_plugin_impl::send_all( const net_message &msg, VerifierFunc verify) {
      for( auto &c : connections) {
         if( c->current() && verify( c)) {
            c->enqueue( msg );
         }
      }
   }

   bool chainnet_plugin_impl::is_valid( const handshake_message &msg) {
      // Do some basic validation of an incoming handshake_message, so things
      // that really aren't handshake messages can be quickly discarded without
      // affecting state.
      bool valid = true;
      if (msg.last_irreversible_block_num > msg.head_num) {
         wlog("Handshake message validation: last irreversible block (${i}) is greater than head block (${h})",
              ("i", msg.last_irreversible_block_num)("h", msg.head_num));
         valid = false;
      }
      if (msg.p2p_address.empty()) {
         wlog("Handshake message validation: p2p_address is null string");
         valid = false;
      }
      if (msg.os.empty()) {
         wlog("Handshake message validation: os field is null string");
         valid = false;
      }
      if ((msg.sig != ecc::compact_signature() || msg.token != sha256()) && (msg.token != fc::sha256::hash(msg.time))) {
         wlog("Handshake message validation: token field invalid");
         valid = false;
      }
      return valid;
   }

   void chainnet_plugin_impl::handle_message( connection_ptr c, const handshake_message &msg) {
	   fc_dlog(logger, "got a handshake_message from ${p} ${h}", ("p", c->peer_addr)("h", msg.p2p_address));
	   if (!is_valid(msg)) {
		   elog("Invalid handshake message received from ${p} ${h}", ("p", c->peer_addr)("h", msg.p2p_address));
		   c->msg_enqueue(leave_message(fatal_other));
		   return;
	   }
	   chain_xmax& cc = chain_plug->getchain();
	   uint32_t liblock_num = cc.get_dynamic_states().last_irreversible_block_num;
	   uint32_t peer_liblock = msg.last_irreversible_block_num;
	   if (c->connecting) {
		   c->connecting = false;
	   }
	   if (msg.generation == 1) {
		   if (msg.node_id == node_id) {
			   elog("Self connection detected. Closing connection");
			   c->msg_enqueue(leave_message(self));
			   return;
		   }

		   if (c->peer_addr.empty() || c->last_handshake_recv.node_id == fc::sha256()) {
			   fc_dlog(logger, "checking for duplicate");
			   for (const auto &check : connections) {
				   if (check == c)
					   continue;
				   if (check->connected() && check->peer_name() == msg.p2p_address) {
					   // It's possible that both peers could arrive here at relatively the same time, so
					   // we need to avoid the case where they would both tell a different connection to go away.
					   // Using the sum of the initial handshake times of the two connections, we will
					   // arbitrarily (but consistently between the two peers) keep one of them.
					   if (msg.time + c->last_handshake_sent.time <= check->last_handshake_sent.time + check->last_handshake_recv.time)
						   continue;

					   fc_dlog(logger, "sending leave duplicate to ${ep}", ("ep", msg.p2p_address));
					   leave_message gam(duplicate);
					   gam.node_id = node_id;
					   c->msg_enqueue(gam);
					   c->no_retry = duplicate;
					   return;
				   }
			   }
		   }
		   else {
			   fc_dlog(logger, "skipping duplicate check, addr == ${pa}, id = ${ni}", ("pa", c->peer_addr)("ni", c->last_handshake_recv.node_id));
		   }

		   if (msg.chain_id != chain_id) {
			   elog("Peer on a different chain. Closing connection");
			   c->msg_enqueue(leave_message(leave_reason::wrong_chain));
			   return;
		   }
		   if (msg.network_version != network_version) {
			   if (network_version_match) {
				   elog("Peer network version does not match expected ${nv} but got ${mnv}",
					   ("nv", network_version)("mnv", msg.network_version));
				   c->msg_enqueue(leave_message(wrong_version));
				   return;
			   }
			   else {
				   wlog("Peer network version does not match expected ${nv} but got ${mnv}",
					   ("nv", network_version)("mnv", msg.network_version));
			   }
		   }

		   if (c->node_id != msg.node_id) {
			   c->node_id = msg.node_id;
		   }

		   if (!authenticate_peer(msg)) {
			   elog("Peer not authenticated.  Closing connection.");
			   c->msg_enqueue(leave_message(authentication));
			   return;
		   }

		   bool on_fork = false;
		   fc_dlog(logger, "liblock_num = ${ln} peer_liblock = ${pl}", ("ln", liblock_num)("pl", peer_liblock));

		   if (peer_liblock <= liblock_num && peer_liblock > 0) {
			   try {
				   xmax_type_block_id peer_lib_id = cc.get_blockid_from_num(peer_liblock);
				   on_fork = (msg.last_irreversible_block_id != peer_lib_id);
			   }
			   catch (const unknown_block_exception &ex) {
				   wlog("peer last irreversible block ${pl} is unknown", ("pl", peer_liblock));
				   on_fork = true;
			   }
			   catch (...) {
				   wlog("caught an exception getting block id for ${pl}", ("pl", peer_liblock));
				   on_fork = true;
			   }
			   if (on_fork) {
				   elog("Peer chain is forked");
				   c->msg_enqueue(leave_message(forked));
				   return;
			   }
		   }

		   if (c->sent_handshake_count == 0) {
			   c->send_handshake();
		   }
	   }

	   c->last_handshake_recv = msg;
	   //TODO: resolve message
   }

   void chainnet_plugin_impl::handle_message( connection_ptr c, const leave_message &msg ) {
	   Chain::string rsn = reason_str( msg.reason );
      ilog( "received a go away message from ${p}, reason = ${r}",
            ("p", c->peer_name())("r",rsn));
      c->no_retry = msg.reason;
      if(msg.reason == duplicate ) {
         c->node_id = msg.node_id;
      }
      c->flush_queues();
      close (c);
   }

   void chainnet_plugin_impl::handle_message(connection_ptr c, const time_message &msg) {
      /* We've already lost however many microseconds it took to dispatch
       * the message, but it can't be helped.
       */
      msg.dst = c->get_time();

      // If the transmit timestamp is zero, the peer is horribly broken.
      if(msg.xmt == 0)
         return;                 /* invalid timestamp */

      if(msg.xmt == c->xmt)
         return;                 /* duplicate packet */

      c->xmt = msg.xmt;
      c->rec = msg.rec;
      c->dst = msg.dst;

      if(msg.org == 0)
         {
            c->send_time(msg);
            return;  // We don't have enough data to perform the calculation yet.
         }

      c->offset = (double(c->rec - c->org) + double(msg.xmt - c->dst)) / 2;
      double NsecPerUsec{1000};

      if(logger.is_enabled(fc::log_level::all))
         logger.log(FC_LOG_MESSAGE(all, "Clock offset is ${o}ns (${us}us)", ("o", c->offset)("us", c->offset/NsecPerUsec)));
      c->org = 0;
      c->rec = 0;
   }

   void chainnet_plugin_impl::handle_message( connection_ptr c, const notice_message &msg) {
      // peer tells us about one or more blocks or txns. When done syncing, forward on
      // notices of previously unknown blocks or txns,
      //
      fc_dlog(logger, "got a notice_message from ${p}", ("p",c->peer_name()));
      request_message req;
      bool send_req = false;
      switch (msg.known_trx.mode) {
      case none:
         break;
      case last_irr_catch_up: {
         c->last_handshake_recv.head_num = msg.known_trx.pending;
         req.req_trx.mode = none;
         break;
      }
      case catch_up : {
         if( msg.known_trx.pending > 0) {
            // plan to get all except what we already know about.
            req.req_trx.mode = catch_up;
            send_req = true;
            size_t known_sum = local_txns.size();
            if( known_sum ) {
               for( const auto& t : local_txns.get<by_id>( ) ) {
                  req.req_trx.ids.push_back( t.id );
               }
            }
         }
         break;
      }
      case normal: {
         //TODO
      }
      }

      fc_dlog(logger,"this is a ${m} notice with ${n} blocks", ("m",modes_str(msg.known_blocks.mode))("n",msg.known_blocks.pending));

      switch (msg.known_blocks.mode) {
      case none : {
         if (msg.known_trx.mode != normal) {
            return;
         }
         break;
      }
      case last_irr_catch_up:
      case catch_up: {
         //TODO
         break;
      }
      case normal : {
         //TODO
         break;
      }
      default: {
         fc_dlog(logger, "received a bogus known_blocks.mode ${m} from ${p}",("m",static_cast<uint32_t>(msg.known_blocks.mode))("p",c->peer_name()));
      }
      }
      fc_dlog(logger, "send req = ${sr}", ("sr",send_req));
      if( send_req) {
         c->msg_enqueue(req);
      }
   }

   void chainnet_plugin_impl::handle_message( connection_ptr c, const request_message &msg) {
      switch (msg.req_blocks.mode) {
      case catch_up :
         fc_dlog( logger,  "got a catch_up request_message from ${p}", ("p",c->peer_name()));
         c->blk_send_branch( );
         break;
      case normal :
         fc_dlog(logger, "got a normal block request_message from ${p}", ("p",c->peer_name()));
         c->blk_send(msg.req_blocks.ids);
         break;
      default:;
      }


      switch (msg.req_trx.mode) {
      case catch_up :
         c->txn_send_pending(msg.req_trx.ids);
         break;
      case normal :
         c->txn_send(msg.req_trx.ids);
         break;
      case none :
         if(msg.req_blocks.mode == none)
            c->stop_send();
         break;
      default:;
      }

   }

   void chainnet_plugin_impl::handle_message( connection_ptr c, const sync_request_message &msg) {
      if( msg.end_block == 0) {
         c->sync_requested.reset();
         c->flush_queues();
      } else {
         c->sync_requested.reset(new sync_state( msg.start_block,msg.end_block,msg.start_block-1));
         c->enqueue_sync_block();
      }
   }

   void chainnet_plugin_impl::handle_message( connection_ptr , const block_summary_message &) {
#if 0 // function is obsolete
#endif
   }


   void chainnet_plugin_impl::handle_message( connection_ptr c, const Chain::signed_transaction &msg) {
	   //TODO

   }

   void chainnet_plugin_impl::handle_message( connection_ptr c, const signed_block &msg) {
		//TODO
   }

   void chainnet_plugin_impl::start_conn_timer( ) {
      connector_check->expires_from_now( connector_period);
      connector_check->async_wait( [&](boost::system::error_code ec) {
            if( !ec) {
               connection_monitor( );
            }
            else {
               elog( "Error from connection check monitor: ${m}",( "m", ec.message()));
               start_conn_timer( );
            }
         });
   }

   void chainnet_plugin_impl::start_txn_timer() {
      transaction_check->expires_from_now( txn_exp_period);
      transaction_check->async_wait( [&](boost::system::error_code ec) {
            if( !ec) {
               expire_txns( );
            }
            else {
               elog( "Error from connection check monitor: ${m}",( "m", ec.message()));
               start_txn_timer( );
            }
         });
   }

   void chainnet_plugin_impl::ticker() {
      keepalive_timer->expires_from_now (keepalive_interval);
      keepalive_timer->async_wait ([&](boost::system::error_code ec) {
            ticker ();
            if (ec) {
               wlog ("Peer keepalive ticked sooner than expected: ${m}", ("m", ec.message()));
            }
            for (auto &c : connections ) {
               if (c->socket->is_open()) {
                  c->send_time();
               }
            }
         });
   }

   void chainnet_plugin_impl::start_monitors() {
      connector_check.reset(new boost::asio::steady_timer( app().get_io_service()));
      transaction_check.reset(new boost::asio::steady_timer( app().get_io_service()));
      start_conn_timer();
      start_txn_timer();
   }

   void chainnet_plugin_impl::expire_txns() {
      start_txn_timer( );
      auto &old = local_txns.get<by_expiry>();
      auto ex_up = old.upper_bound( time_point::now());
      auto ex_lo = old.lower_bound( fc::time_point_sec( 0));
      old.erase( ex_lo, ex_up);

      auto &stale = local_txns.get<by_block_num>();
      chain_xmax &cc = chain_plug->getchain();
      uint32_t bn = cc.get_dynamic_states().last_irreversible_block_num;
      auto bn_up = stale.upper_bound(bn);
      auto bn_lo = stale.lower_bound(1);
      stale.erase( bn_lo, bn_up);
   }

   void chainnet_plugin_impl::connection_monitor( ) {
      start_conn_timer();
	  Chain::vector <connection_ptr> discards;
      num_clients = 0;
      for( auto &c : connections ) {
         if( !c->socket->is_open() && !c->connecting) {
            if( c->peer_addr.length() > 0) {
               connect(c);
            }
            else {
               discards.push_back( c);
            }
         } else {
            if( c->peer_addr.empty()) {
               num_clients++;
            }
         }
      }
      if( discards.size( ) ) {
         for( auto &c : discards) {
            connections.erase( c );
            c.reset();
         }
      }
   }

   void chainnet_plugin_impl::close( connection_ptr c ) {
      if( c->peer_addr.empty( ) ) {
         --num_clients;
      }
      c->close();
   }

   /**
    * This one is necessary to hook into the boost notifier api
    **/
   void chainnet_plugin_impl::transaction_ready( const Chain::signed_transaction& txn) {
      fc_ilog (logger, "TODO broadcasting txn id ${t}", ("t",txn.id()));
      //TODO
   }

   void chainnet_plugin_impl::broadcast_block_impl( const Chain::signed_block &sb) {
	   //TODO
   }

   bool chainnet_plugin_impl::authenticate_peer(const handshake_message& msg) const {
      //TODO
      return true;
   }


   fc::ecc::compact_signature chainnet_plugin_impl::sign_compact(const Chain::public_key_type& signer, const fc::sha256& digest) const
   {
      //TODO
      return ecc::compact_signature();
   }

   chainnet_plugin::chainnet_plugin()
      :my( new chainnet_plugin_impl ) {
	   cnet_impl = my.get();
   }

   chainnet_plugin::~chainnet_plugin() {
   }

   void chainnet_plugin::set_program_options( options_description& /*cli*/, options_description& cfg )
   {
	   cfg.add_options()
		   ("chainnet_plugin-log-level", bpo::value<Chain::string>()->default_value("all"), "Log level: one of 'all', 'debug', 'info', 'warn', 'error', or 'off'")
		   ("p2p-listen-endpoint", bpo::value<Chain::string>()->default_value("0.0.0.0:19876"), "Host:port used to listen incoming p2p connections.")
		   ("p2p-peer-address", bpo::value< Chain::vector<Chain::string> >()->composing(), "The public endpoint of a peer node to connect to. Multiple p2p-peer-address can be used if need to compose a network.")
		   ("max-clients", bpo::value<int>()->default_value(def_max_clients), "Maximum clients from which connections are accepted, (0)zero means unlimit")
		   ("connection-cleanup-period", bpo::value<int>()->default_value(def_conn_retry_wait), "Seconds to wait before cleaning up dead connections")
		   ("network-version-match", bpo::value<bool>()->default_value(false),"If require exact match of peer network version.")
			   ;
   }

   template<typename T>
   T dejsonify(const Chain::string& s) {
      return fc::json::from_string(s).as<T>();
   }

   void chainnet_plugin::plugin_initialize( const variables_map& options ) {
      ilog("chainnet_plugin::plugin_initialize");

      // Housekeeping so fc::logger::get() will work as expected
      fc::get_logger_map()[connection_xmax::logger_name] = connection_xmax::logger;
      fc::get_logger_map()[chainnet_plugin_impl::logger_name] = chainnet_plugin_impl::logger;


      // Setting a parent would in theory get us the default appenders for free but
      // a) the parent's log level overrides our own in that case; and
      // b) fc library's logger was never finished - the _additivity flag tested is never true.
      for(fc::shared_ptr<fc::appender>& appender : fc::logger::get().get_appenders()) {
		  connection_xmax::logger.add_appender(appender);
         chainnet_plugin_impl::logger.add_appender(appender);
      }

      if( options.count( "chainnet_plugin-log-level" ) ) {
         fc::log_level logl;

         fc::from_variant(options.at("chainnet_plugin-log-level").as<Chain::string>(), logl);
         ilog("Setting chainnet_plugin logging level to ${level}", ("level", logl));
		 connection_xmax::logger.set_log_level(logl);
         chainnet_plugin_impl::logger.set_log_level(logl);
      }

      my->network_version = static_cast<uint16_t>(app().version());
      my->network_version_match = options.at("network-version-match").as<bool>();
      my->send_whole_blocks = def_send_whole_blocks;

      my->connector_period = std::chrono::seconds(options.at("connection-cleanup-period").as<int>());
      my->txn_exp_period = def_txn_expire_wait;
      my->resp_expected_period = def_resp_expected_wait;
      my->max_client_count = options.at("max-clients").as<int>();

      my->num_clients = 0;
      my->started_sessions = 0;

      my->resolver = std::make_shared<tcp::resolver>( std::ref( app().get_io_service() ) );
      if(options.count("p2p-listen-endpoint")) {
         my->p2p_address = options.at("p2p-listen-endpoint").as< Chain::string >();
         auto host = my->p2p_address.substr( 0, my->p2p_address.find(':') );
         auto port = my->p2p_address.substr( host.size()+1, my->p2p_address.size() );
         idump((host)(port));
         tcp::resolver::query query( tcp::v4(), host.c_str(), port.c_str() );
         // Note: need to add support for IPv6 too?

         my->listen_endpoint = *my->resolver->resolve( query);

         my->acceptor.reset( new tcp::acceptor( app().get_io_service() ) );
      }
      if(options.count("p2p-server-address")) {
         my->p2p_address = options.at("p2p-server-address").as< Chain::string >();
      }
      else {
         if(my->listen_endpoint.address().to_v4() == address_v4::any()) {
            boost::system::error_code ec;
            auto host = host_name(ec);
            if( ec.value() != boost::system::errc::success) {

               FC_THROW_EXCEPTION( fc::invalid_arg_exception,
                                   "Unable to retrieve host_name. ${msg}",( "msg",ec.message()));

            }
            auto port = my->p2p_address.substr( my->p2p_address.find(':'), my->p2p_address.size());
            my->p2p_address = host + port;
         }
      }

      if(options.count("p2p-peer-address")) {
         my->supplied_peers = options.at("p2p-peer-address").as<Chain::vector<Chain::string> >();
      }
      if(options.count("agent-name")) {
         my->user_agent_name = options.at("agent-name").as<Chain::string>();
      }

      if(options.count("allowed-connection")) {
         const std::vector<std::string> allowed_remotes = options["allowed-connection"].as<std::vector<std::string>>();
         for(const std::string& allowed_remote : allowed_remotes)
            {
               if(allowed_remote == "any")
                  my->allowed_connections |= chainnet_plugin_impl::Any;
               else if(allowed_remote == "builders")
                  my->allowed_connections |= chainnet_plugin_impl::Builders;
               else if(allowed_remote == "specified")
                  my->allowed_connections |= chainnet_plugin_impl::Specified;
               else if(allowed_remote == "none")
                  my->allowed_connections = chainnet_plugin_impl::None;
            }
      }

      if(options.count("peer-key")) {
         const std::vector<std::string> key_strings = options["peer-key"].as<std::vector<std::string>>();
         for(const std::string& key_string : key_strings)
            {
               my->allowed_peers.push_back(dejsonify<Chain::public_key_type>(key_string));
            }
      }

      if( options.count( "send-whole-blocks")) {
         my->send_whole_blocks = options.at( "send-whole-blocks" ).as<bool>();
      }

      my->chain_plug = app().find_plugin<blockchain_plugin>();
      my->chain_plug->get_chain_id(my->chain_id);
      fc::rand_pseudo_bytes(my->node_id.data(), my->node_id.data_size());
      ilog("my node_id is ${id}",("id",my->node_id));

      my->keepalive_timer.reset(new boost::asio::steady_timer(app().get_io_service()));
      my->ticker();
   }

   void chainnet_plugin::plugin_startup() {
	   ilog("chainnet_plugin::plugin_startup");
      if( my->acceptor ) {
         my->acceptor->open(my->listen_endpoint.protocol());
         my->acceptor->set_option(tcp::acceptor::reuse_address(true));
         my->acceptor->bind(my->listen_endpoint);
         my->acceptor->listen();
         ilog("starting listener, max clients is ${mc}",("mc",my->max_client_count));
         my->start_listen_loop();
      }

      my->chain_plug->getchain().on_pending_transaction.connect( &chainnet_plugin_impl::transaction_ready);
      my->start_monitors();

      for( auto seed_node : my->supplied_peers ) {
         connect( seed_node );
      }
   }

   void chainnet_plugin::plugin_shutdown() {
      try {
         ilog( "shutdown.." );
         my->done = true;
         if( my->acceptor ) {
            ilog( "close acceptor" );
            my->acceptor->close();

            ilog( "close ${s} connections",( "s",my->connections.size()) );
            auto cons = my->connections;
            for( auto con : cons ) {
               my->close( con);
            }

            my->acceptor.reset(nullptr);
         }
         ilog( "exit shutdown" );
      }
      FC_CAPTURE_AND_RETHROW()
   }

   void chainnet_plugin::broadcast_block( const Chain::signed_block &sb) {
      fc_dlog(my->logger, "broadcasting block #${num}",("num",sb.block_num()) );
      my->broadcast_block_impl( sb);
   }

   size_t chainnet_plugin::num_peers() const {
      return my->count_open_sockets();
   }

   /**
    *  Used to trigger a new connection from RPC API
    */
   Chain::string chainnet_plugin::connect( const Chain::string& host ) {
      if( my->find_connection( host ) )
         return "already connected";

      connection_ptr c = std::make_shared<connection_xmax>(host);
      fc_dlog(my->logger,"adding new connection to the list");
      my->connections.insert( c );
      fc_dlog(my->logger,"calling active connector");
      my->connect( c );
      return "added connection";
   }

   Chain::string chainnet_plugin::disconnect( const Chain::string& host ) {
      for( auto itr = my->connections.begin(); itr != my->connections.end(); ++itr ) {
         if( (*itr)->peer_addr == host ) {
            (*itr)->reset();
            my->close(*itr);
            my->connections.erase(itr);
            return "connection removed";
         }
      }
      return "no known connection for host";
   }

   optional<connection_status> chainnet_plugin::status( const Chain::string& host )const {
      auto con = my->find_connection( host );
      if( con )
         return con->get_status();
      return optional<connection_status>();
   }

   Chain::vector<connection_status> chainnet_plugin::connections()const {
	   Chain::vector<connection_status> result;
      result.reserve( my->connections.size() );
      for( const auto& c : my->connections ) {
         result.push_back( c->get_status() );
      }
      return result;
   }
   connection_ptr chainnet_plugin_impl::find_connection( Chain::string host )const {
      for( const auto& c : connections )
         if( c->peer_addr == host ) return c;
      return connection_ptr();
   }


   void connection_xmax::txn_send_pending(const Chain::vector<xmax_type_transaction_id> &ids) {
	   for (auto tx = cnet_impl->local_txns.begin(); tx != cnet_impl->local_txns.end(); ++tx) {
		   if (tx->packed_transaction.size() && tx->block_num == 0) {
			   bool found = false;
			   for (auto known : ids) {
				   if (known == tx->id) {
					   found = true;
					   break;
				   }
			   }
			   if (!found) {
				   cnet_impl->local_txns.modify(tx, incr_in_flight);
				   queue_write(std::make_shared<Chain::vector<char>>(tx->packed_transaction),
					   true,
					   [this, tx](boost::system::error_code ec, std::size_t) {
					   cnet_impl->local_txns.modify(tx, decr_in_flight);
				   });
			   }
		   }
	   }
   }

   void connection_xmax::txn_send(const Chain::vector<xmax_type_transaction_id> &ids) {
	   for (auto t : ids) {
		   auto tx = cnet_impl->local_txns.get<by_id>().find(t);
		   if (tx != cnet_impl->local_txns.end() && tx->packed_transaction.size()) {
			   cnet_impl->local_txns.modify(tx, incr_in_flight);
			   queue_write(std::make_shared<Chain::vector<char>>(tx->packed_transaction),
				   true,
				   [this, tx](boost::system::error_code ec, std::size_t) {
				   cnet_impl->local_txns.modify(tx, decr_in_flight);
			   });
		   }
	   }
   }

   void connection_xmax::blk_send_branch() {
	   Chain::chain_xmax &cc = cnet_impl->chain_plug->getchain();
	   uint32_t head_num = cc.get_dynamic_states().head_block_number;
	   notice_message note;
	   note.known_blocks.mode = normal;
	   note.known_blocks.pending = 0;
	   fc_dlog(logger, "head_num = ${h}", ("h", head_num));
	   if (head_num == 0) {
		   msg_enqueue(note);
		   return;
	   }
	   xmax_type_block_id head_id;
	   xmax_type_block_id lib_id;
	   uint32_t lib_num;
	   try {
		   lib_num = cc.get_dynamic_states().last_irreversible_block_num;
		   if (lib_num != 0)
			   lib_id = cc.get_blockid_from_num(lib_num);
		   head_id = cc.get_blockid_from_num(head_num);
	   }
	   catch (const assert_exception &ex) {
		   elog("unable to retrieve block info: ${n} for ${p}", ("n", ex.to_string())("p", peer_name()));
		   msg_enqueue(note);
		   return;
	   }
	   catch (const fc::exception &ex) {
	   }
	   catch (...) {
	   }

	   Chain::vector<optional<signed_block> > bstack;
	   xmax_type_block_id null_id;
	   for (auto bid = head_id; bid != null_id && bid != lib_id; ) {
		   try {
			   optional<signed_block> b = cc.get_block_from_id(bid);
			   if (b) {
				   bid = b->previous;
				   bstack.push_back(b);
			   }
			   else {
				   break;
			   }
		   }
		   catch (...) {
			   break;
		   }
	   }
	   size_t count = 0;
	   if (bstack.back()->previous == lib_id) {
		   count = bstack.size();
		   while (bstack.size()) {
			   msg_enqueue(*bstack.back());
			   bstack.pop_back();
		   }
	   }

	   fc_ilog(logger, "Sent ${n} blocks on my fork", ("n", count));
	   syncing = false;
   }

   void connection_xmax::blk_send(const Chain::vector<xmax_type_block_id> &ids) {
	   chain_xmax &cc = cnet_impl->chain_plug->getchain();
	   int count = 0;
	   for (auto &blkid : ids) {
		   ++count;
		   try {
			   optional<signed_block> b = cc.get_block_from_id(blkid);
			   if (b) {
				   fc_dlog(logger, "get block from id at num ${n}", ("n", b->block_num()));
				   msg_enqueue(*b);
			   }
			   else {
				   ilog("get block from id returned null, id ${id} on block ${c} of ${s} for ${p}",
					   ("id", blkid)("c", count)("s", ids.size())("p", peer_name()));
				   break;
			   }
		   }
		   catch (const assert_exception &ex) {
			   elog("caught assert on get_block_from_id, ${ex}, id ${id} on block ${c} of ${s} for ${p}",
				   ("ex", ex.to_string())("id", blkid)("c", count)("s", ids.size())("p", peer_name()));
			   break;
		   }
		   catch (...) {
			   elog("caught othser exception getting block id ${id} on block ${c} of ${s} for ${p}",
				   ("id", blkid)("c", count)("s", ids.size())("p", peer_name()));
			   break;
		   }
	   }

   }

   void connection_xmax::do_queue_write() {
	   if (write_queue.empty())
		   return;
	   write_depth++;
	   connection_wptr c(shared_from_this());
	   boost::asio::async_write(*socket, boost::asio::buffer(*write_queue.front().buff), [c](boost::system::error_code ec, std::size_t w) {
		   try {
			   auto conn = c.lock();
			   if (!conn)
				   return;

			   if (conn->write_queue.size()) {
				   conn->write_queue.front().cb(ec, w);
			   }
			   conn->write_depth--;

			   if (ec) {
				   Chain::string pname = conn ? conn->peer_name() : "no connection name";
				   if (ec.value() != boost::asio::error::eof) {
					   elog("Error sending to peer ${p}: ${i}", ("p", pname)("i", ec.message()));
				   }
				   else {
					   ilog("connection closure detected on write to ${p}", ("p", pname));
				   }
				   cnet_impl->close(conn);
				   return;
			   }
			   conn->write_queue.pop_front();
			   conn->enqueue_sync_block();
			   conn->do_queue_write();
		   }
		   catch (const std::exception &ex) {
			   auto conn = c.lock();
			   Chain::string pname = conn ? conn->peer_name() : "no connection name";
			   elog("Exception in do_queue_write to ${p} ${s}", ("p", pname)("s", ex.what()));
		   }
		   catch (const fc::exception &ex) {
			   auto conn = c.lock();
			   Chain::string pname = conn ? conn->peer_name() : "no connection name";
			   elog("Exception in do_queue_write to ${p} ${s}", ("p", pname)("s", ex.to_string()));
		   }
		   catch (...) {
			   auto conn = c.lock();
			   Chain::string pname = conn ? conn->peer_name() : "no connection name";
			   elog("Exception in do_queue_write to ${p}", ("p", pname));
		   }
	   });
   }

   void connection_xmax::msg_enqueue(const net_message &m, bool trigger_send) {
	   bool close_after_send = false;
	   if (m.contains<sync_request_message>()) {
		   sync_wait();
	   }
	   else if (m.contains<request_message>()) {
		   pending_fetch = m.get<request_message>();
		   fetch_wait();
	   }
	   else {
		   close_after_send = m.contains<leave_message>();
	   }

	   uint32_t payload_size = fc::raw::pack_size(m);
	   char * header = reinterpret_cast<char*>(&payload_size);
	   size_t header_size = sizeof(payload_size);

	   size_t buffer_size = header_size + payload_size;

	   auto send_buffer = std::make_shared<Chain::vector<char>>(buffer_size);
	   fc::datastream<char*> ds(send_buffer->data(), buffer_size);
	   ds.write(header, header_size);
	   fc::raw::pack(ds, m);
	   write_depth++;
	   queue_write(send_buffer, trigger_send,
		   [this, close_after_send](boost::system::error_code ec, std::size_t) {
		   write_depth--;
		   if (close_after_send) {
			   elog("sent a go away message, closing connection to ${p}", ("p", peer_name()));
			   cnet_impl->close(shared_from_this());
			   return;
		   }
	   });
   }

   void connection_xmax::sync_wait() {
	   response_expected->expires_from_now(cnet_impl->resp_expected_period);
	   connection_wptr c(shared_from_this());
	   response_expected->async_wait([c](boost::system::error_code ec) {
		   connection_ptr conn = c.lock();
		   if (!conn) {
			   // connection was destroyed before this lambda was delivered
			   return;
		   }

		   conn->sync_timeout(ec);
	   });
   }

   void connection_xmax::fetch_wait() {
	   response_expected->expires_from_now(cnet_impl->resp_expected_period);
	   connection_wptr c(shared_from_this());
	   response_expected->async_wait([c](boost::system::error_code ec) {
		   connection_ptr conn = c.lock();
		   if (!conn) {
			   // connection was destroyed before this lambda was delivered
			   return;
		   }

		   conn->fetch_timeout(ec);
	   });
   }

   bool connection_xmax::process_next_message(chainnet_plugin_impl& impl, uint32_t message_length) {
	   try {
		   cancel_wait();
		   // If it is a signed_block, then save the raw message for the cache
		   // This must be done before we unpack the message.
		   // This code is copied from fc::io::unpack(..., unsigned_int)
		   auto index = pending_message_buffer.read_index();
		   uint64_t which = 0; char b = 0; uint8_t by = 0;
		   do {
			   pending_message_buffer.peek(&b, 1, index);
			   which |= uint32_t(uint8_t(b) & 0x7f) << by;
			   by += 7;
		   } while (uint8_t(b) & 0x80);

		   if (which == uint64_t(net_message::tag<signed_block>::value)) {
			   blk_buffer.resize(message_length);
			   auto index = pending_message_buffer.read_index();
			   pending_message_buffer.peek(blk_buffer.data(), message_length, index);
		   }
		   auto ds = pending_message_buffer.create_datastream();
		   net_message msg;
		   fc::raw::unpack(ds, msg);
		   msgHandler m(impl, shared_from_this());
		   msg.visit(m);
	   }
	   catch (const fc::exception& e) {
		   edump((e.to_detail_string()));
		   impl.close(shared_from_this());
		   return false;
	   }
	   return true;
   }
   void handshake_initializer::setup(handshake_message &handshake)
   {
	   handshake.time = std::chrono::system_clock::now().time_since_epoch().count();
	   handshake.network_version = cnet_impl->network_version;


	   handshake.chain_id = cnet_impl->chain_id;
	   handshake.node_id = cnet_impl->node_id;
	   //hello.key = empty();//TODO
	   
	   handshake.token = fc::sha256::hash(handshake.time);
	   handshake.sig = cnet_impl->sign_compact(handshake.key, handshake.token);

	   if (handshake.sig == ecc::compact_signature())
		   handshake.token = sha256();


	   handshake.p2p_address = cnet_impl->p2p_address + " - " + handshake.node_id.str().substr(0, 7);
#if defined( __APPLE__ )
	   handshake.os = "osx";
#elif defined( __linux__ )
	   handshake.os = "linux";
#elif defined( _MSC_VER )
	   handshake.os = "win32";
#else
	   handshake.os = "other";
#endif
	   handshake.agent = cnet_impl->user_agent_name;


	   chain_xmax& cx = cnet_impl->chain_plug->getchain();
	   handshake.head_id = fc::sha256();
	   handshake.last_irreversible_block_id = fc::sha256();
	   handshake.head_num = cx.head_block_num();
	   handshake.last_irreversible_block_num = cx.get_dynamic_states().last_irreversible_block_num;
	   if (handshake.last_irreversible_block_num) {
		   try {
			   handshake.last_irreversible_block_id = cx.get_blockid_from_num(handshake.last_irreversible_block_num);
		   }
		   catch (const unknown_block_exception &ex) {
			   handshake.last_irreversible_block_num = 0;
		   }
	   }
	   if (handshake.head_num) {
		   try {
			   handshake.head_id = cx.get_blockid_from_num(handshake.head_num);
		   }
		   catch (const unknown_block_exception &ex) {
			   handshake.head_num = 0;
		   }
	   }
   }
}
