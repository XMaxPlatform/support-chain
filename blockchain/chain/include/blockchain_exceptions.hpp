/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once

#include <fc/exception/exception.hpp>
#include <block.hpp>
#include <exception_macros.hpp>

namespace Xmaxplatform { namespace Chain {

   FC_DECLARE_EXCEPTION( chain_exception, 3000000, "blockchain exception" )
   FC_DECLARE_DERIVED_EXCEPTION( database_query_exception,          Xmaxplatform::Chain::chain_exception, 3010000, "database query exception" )
   FC_DECLARE_DERIVED_EXCEPTION( block_validate_exception,          Xmaxplatform::Chain::chain_exception, 3020000, "block validation exception" )
   FC_DECLARE_DERIVED_EXCEPTION( transaction_exception,             Xmaxplatform::Chain::chain_exception, 3030000, "transaction validation exception" )
   FC_DECLARE_DERIVED_EXCEPTION( message_validate_exception,        Xmaxplatform::Chain::chain_exception, 3040000, "message validation exception" )
   FC_DECLARE_DERIVED_EXCEPTION( message_precondition_exception,    Xmaxplatform::Chain::chain_exception, 3050000, "message precondition exception" )
   FC_DECLARE_DERIVED_EXCEPTION( message_evaluate_exception,        Xmaxplatform::Chain::chain_exception, 3060000, "message evaluation exception" )
   FC_DECLARE_DERIVED_EXCEPTION( utility_exception,                 Xmaxplatform::Chain::chain_exception, 3070000, "utility method exception" )
   FC_DECLARE_DERIVED_EXCEPTION( undo_database_exception,           Xmaxplatform::Chain::chain_exception, 3080000, "undo database exception" )
   FC_DECLARE_DERIVED_EXCEPTION( unlinkable_block_exception,        Xmaxplatform::Chain::chain_exception, 3090000, "unlinkable block" )
   FC_DECLARE_DERIVED_EXCEPTION( black_swan_exception,              Xmaxplatform::Chain::chain_exception, 3100000, "black swan" )
   FC_DECLARE_DERIVED_EXCEPTION( unknown_block_exception,           Xmaxplatform::Chain::chain_exception, 3110000, "unknown block" )
   FC_DECLARE_DERIVED_EXCEPTION( confirmation_validate_exception,	Xmaxplatform::Chain::chain_exception, 3120000, "confirmation validation exception")
   FC_DECLARE_DERIVED_EXCEPTION( block_attack_exception,			Xmaxplatform::Chain::chain_exception, 3130000, "malicious block attack exception")

   FC_DECLARE_DERIVED_EXCEPTION( block_tx_output_exception,         Xmaxplatform::Chain::block_validate_exception, 3020001, "transaction outputs in block do not match transaction outputs from applying block" )
   
   FC_DECLARE_DERIVED_EXCEPTION( tx_missing_auth,                   Xmaxplatform::Chain::transaction_exception, 3030001, "missing required authority" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_missing_sigs,                   Xmaxplatform::Chain::transaction_exception, 3030002, "signatures do not satisfy declared authorizations" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_irrelevant_auth,                Xmaxplatform::Chain::transaction_exception, 3030003, "irrelevant authority included" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_irrelevant_sig,                 Xmaxplatform::Chain::transaction_exception, 3030004, "irrelevant signature included" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_duplicate_sig,                  Xmaxplatform::Chain::transaction_exception, 3030005, "duplicate signature included" )
   FC_DECLARE_DERIVED_EXCEPTION( invalid_committee_approval,        Xmaxplatform::Chain::transaction_exception, 3030006, "committee account cannot directly approve transaction" )
   FC_DECLARE_DERIVED_EXCEPTION( insufficient_fee,                  Xmaxplatform::Chain::transaction_exception, 3030007, "insufficient fee" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_missing_scope,                  Xmaxplatform::Chain::transaction_exception, 3030008, "missing required scope" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_missing_recipient,              Xmaxplatform::Chain::transaction_exception, 3030009, "missing required recipient" )
   FC_DECLARE_DERIVED_EXCEPTION( checktime_exceeded,                Xmaxplatform::Chain::transaction_exception, 3030010, "allocated processing time was exceeded" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_duplicate,                      Xmaxplatform::Chain::transaction_exception, 3030011, "duplicate transaction" )
   FC_DECLARE_DERIVED_EXCEPTION( unknown_transaction_exception,     Xmaxplatform::Chain::transaction_exception, 3030012, "unknown transaction" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_scheduling_exception,           Xmaxplatform::Chain::transaction_exception, 3030013, "transaction failed during sheduling" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_unknown_argument,               Xmaxplatform::Chain::transaction_exception, 3030014, "transaction provided an unknown value to a system call" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_resource_exhausted,             Xmaxplatform::Chain::transaction_exception, 3030015, "transaction exhausted allowed resources" )
   FC_DECLARE_DERIVED_EXCEPTION( page_memory_error,                 Xmaxplatform::Chain::transaction_exception, 3030016, "error in WASM page memory" )
   FC_DECLARE_DERIVED_EXCEPTION( unsatisfied_permission,            Xmaxplatform::Chain::transaction_exception, 3030017, "Unsatisfied permission" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_msgs_auth_exceeded,             Xmaxplatform::Chain::transaction_exception, 3030018, "Number of transaction messages per authorized account has been exceeded" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_msgs_code_exceeded,             Xmaxplatform::Chain::transaction_exception, 3030019, "Number of transaction messages per code account has been exceeded" )
   FC_DECLARE_DERIVED_EXCEPTION( tx_code_db_limit_exceeded,         Xmaxplatform::Chain::transaction_exception, 3030020, "Database storage limit for code account has been exceeded in transaction message" )
   FC_DECLARE_DERIVED_EXCEPTION( msg_resource_exhausted,            Xmaxplatform::Chain::transaction_exception, 3030021, "message exhausted allowed resources" )
   FC_DECLARE_DERIVED_EXCEPTION( api_not_supported,                 Xmaxplatform::Chain::transaction_exception, 3030022, "API not currently supported" )

   FC_DECLARE_DERIVED_EXCEPTION( account_name_exists_exception,     Xmaxplatform::Chain::message_precondition_exception, 3050001, "account name already exists" )
   FC_DECLARE_DERIVED_EXCEPTION( invalid_pts_address,               Xmaxplatform::Chain::utility_exception, 3060001, "invalid pts address" )
   FC_DECLARE_DERIVED_EXCEPTION( insufficient_feeds,                Xmaxplatform::Chain::chain_exception, 37006, "insufficient feeds" )

   FC_DECLARE_DERIVED_EXCEPTION( pop_empty_chain,                   Xmaxplatform::Chain::undo_database_exception, 3070001, "there are no blocks to pop" )

	FC_DECLARE_DERIVED_EXCEPTION(script_runout, Xmaxplatform::Chain::transaction_exception, 3080001, "run out of script instrunction limit")

   #define XMAX_RECODE_EXC( cause_type, effect_type ) \
      catch( const cause_type& e ) \
      { throw( effect_type( e.what(), e.get_log() ) ); }

} } // Xmaxplatform::chain
