#pragma once
namespace Xmaxplatform { namespace Basetypes {
    typedef name                             account_name;
    typedef name                             authority_name;
    typedef name                             func_name;
    typedef name                             event_name;
    typedef fixed_string32                   message_name;
    typedef fixed_string32                   type_name;
    struct account_auth { 
        account_auth() = default;
        account_auth(const account_name& account, const authority_name& authority)
           : account(account), authority(authority) {}

        account_name                     account;
        authority_name                   authority;
    };

    template<> struct get_struct<account_auth> { 
        static const struct_t& type() { 
           static struct_t result = { "account_auth", "", {
                {"account", "account_name"},
                {"authority", "authority_name"},
              }
           };
           return result;
         }
    };

    struct message { 
        message() = default;
        message(const account_name& code, const func_name& type, const vector<account_auth>& authorization, const bytes& data)
           : code(code), type(type), authorization(authorization), data(data) {}

        account_name                     code;
        func_name                        type;
        vector<account_auth>             authorization;
        bytes                            data;
    };

    template<> struct get_struct<message> { 
        static const struct_t& type() { 
           static struct_t result = { "message", "", {
                {"code", "account_name"},
                {"type", "func_name"},
                {"authorization", "account_auth[]"},
                {"data", "bytes"},
              }
           };
           return result;
         }
    };

    struct account_permission_weight { 
        account_permission_weight() = default;
        account_permission_weight(const account_auth& permission, const uint16& weight)
           : permission(permission), weight(weight) {}

        account_auth                     permission;
        uint16                           weight;
    };

    template<> struct get_struct<account_permission_weight> { 
        static const struct_t& type() { 
           static struct_t result = { "account_permission_weight", "", {
                {"permission", "account_auth"},
                {"weight", "uint16"},
              }
           };
           return result;
         }
    };

    struct transaction { 
        transaction() = default;
        transaction(const uint16& ref_block_num, const uint32& ref_block_prefix, const time& expiration, const vector<account_name>& scope, const vector<account_name>& read_scope, const vector<message>& messages, const account_name& gas_payer, const uint64& gas, const uint64& gaslimit)
           : ref_block_num(ref_block_num), ref_block_prefix(ref_block_prefix), expiration(expiration), scope(scope), read_scope(read_scope), messages(messages), gas_payer(gas_payer), gas(gas), gaslimit(gaslimit) {}

        uint16                           ref_block_num;
        uint32                           ref_block_prefix;
        time                             expiration;
        vector<account_name>             scope;
        vector<account_name>             read_scope;
        vector<message>                  messages;
        account_name                     gas_payer;
        uint64                           gas;
        uint64                           gaslimit;
    };

    template<> struct get_struct<transaction> { 
        static const struct_t& type() { 
           static struct_t result = { "transaction", "", {
                {"ref_block_num", "uint16"},
                {"ref_block_prefix", "uint32"},
                {"expiration", "time"},
                {"scope", "account_name[]"},
                {"read_scope", "account_name[]"},
                {"messages", "message[]"},
                {"gas_payer", "account_name"},
                {"gas", "uint64"},
                {"gaslimit", "uint64"},
              }
           };
           return result;
         }
    };

    struct signed_transaction : public transaction { 
        signed_transaction() = default;
        signed_transaction(const vector<signature>& signatures)
           : signatures(signatures) {}

        vector<signature>                signatures;
    };

    template<> struct get_struct<signed_transaction> { 
        static const struct_t& type() { 
           static struct_t result = { "signed_transaction", "transaction", {
                {"signatures", "signature[]"},
              }
           };
           return result;
         }
    };

    struct key_permission_weight { 
        key_permission_weight() = default;
        key_permission_weight(const public_key& key, const uint16& weight)
           : key(key), weight(weight) {}

        public_key                       key;
        uint16                           weight;
    };

    template<> struct get_struct<key_permission_weight> { 
        static const struct_t& type() { 
           static struct_t result = { "key_permission_weight", "", {
                {"key", "public_key"},
                {"weight", "uint16"},
              }
           };
           return result;
         }
    };

    struct authority { 
        authority() = default;
        authority(const uint32& threshold, const vector<key_permission_weight>& keys, const vector<account_permission_weight>& accounts)
           : threshold(threshold), keys(keys), accounts(accounts) {}

        uint32                           threshold;
        vector<key_permission_weight>    keys;
        vector<account_permission_weight> accounts;
    };

    template<> struct get_struct<authority> { 
        static const struct_t& type() { 
           static struct_t result = { "authority", "", {
                {"threshold", "uint32"},
                {"keys", "key_permission_weight[]"},
                {"accounts", "account_permission_weight[]"},
              }
           };
           return result;
         }
    };

    struct blockchain_configuration { 
        blockchain_configuration() = default;
        blockchain_configuration(const uint32& max_blk_size, const uint32& target_blk_size, const uint64& max_storage_size, const share_type& elected_pay, const share_type& runner_up_pay, const share_type& min_xmx_balance, const uint32& max_trx_lifetime, const uint16& auth_depth_limit, const uint32& max_trx_runtime, const uint16& in_depth_limit, const uint32& max_in_msg_size, const uint32& max_gen_trx_size)
           : max_blk_size(max_blk_size), target_blk_size(target_blk_size), max_storage_size(max_storage_size), elected_pay(elected_pay), runner_up_pay(runner_up_pay), min_xmx_balance(min_xmx_balance), max_trx_lifetime(max_trx_lifetime), auth_depth_limit(auth_depth_limit), max_trx_runtime(max_trx_runtime), in_depth_limit(in_depth_limit), max_in_msg_size(max_in_msg_size), max_gen_trx_size(max_gen_trx_size) {}

        uint32                           max_blk_size;
        uint32                           target_blk_size;
        uint64                           max_storage_size;
        share_type                       elected_pay;
        share_type                       runner_up_pay;
        share_type                       min_xmx_balance;
        uint32                           max_trx_lifetime;
        uint16                           auth_depth_limit;
        uint32                           max_trx_runtime;
        uint16                           in_depth_limit;
        uint32                           max_in_msg_size;
        uint32                           max_gen_trx_size;
    };

    template<> struct get_struct<blockchain_configuration> { 
        static const struct_t& type() { 
           static struct_t result = { "blockchain_configuration", "", {
                {"max_blk_size", "uint32"},
                {"target_blk_size", "uint32"},
                {"max_storage_size", "uint64"},
                {"elected_pay", "share_type"},
                {"runner_up_pay", "share_type"},
                {"min_xmx_balance", "share_type"},
                {"max_trx_lifetime", "uint32"},
                {"auth_depth_limit", "uint16"},
                {"max_trx_runtime", "uint32"},
                {"in_depth_limit", "uint16"},
                {"max_in_msg_size", "uint32"},
                {"max_gen_trx_size", "uint32"},
              }
           };
           return result;
         }
    };

    struct type_def { 
        type_def() = default;
        type_def(const type_name& new_type_name, const type_name& type)
           : new_type_name(new_type_name), type(type) {}

        type_name                        new_type_name;
        type_name                        type;
    };

    template<> struct get_struct<type_def> { 
        static const struct_t& type() { 
           static struct_t result = { "type_def", "", {
                {"new_type_name", "type_name"},
                {"type", "type_name"},
              }
           };
           return result;
         }
    };

    struct action { 
        action() = default;
        action(const name& action_name, const type_name& type)
           : action_name(action_name), type(type) {}

        name                             action_name;
        type_name                        type;
    };

    template<> struct get_struct<action> { 
        static const struct_t& type() { 
           static struct_t result = { "action", "", {
                {"action_name", "name"},
                {"type", "type_name"},
              }
           };
           return result;
         }
    };

    struct table { 
        table() = default;
        table(const name& table_name, const type_name& index_type, const vector<field_name>& key_names, const vector<type_name>& key_types, const type_name& type)
           : table_name(table_name), index_type(index_type), key_names(key_names), key_types(key_types), type(type) {}

        name                             table_name;
        type_name                        index_type;
        vector<field_name>               key_names;
        vector<type_name>                key_types;
        type_name                        type;
    };

    template<> struct get_struct<table> { 
        static const struct_t& type() { 
           static struct_t result = { "table", "", {
                {"table_name", "name"},
                {"index_type", "type_name"},
                {"key_names", "field_name[]"},
                {"key_types", "type_name[]"},
                {"type", "type_name"},
              }
           };
           return result;
         }
    };

    struct abi { 
        abi() = default;
        abi(const vector<type_def>& types, const vector<struct_t>& structs, const vector<action>& actions, const vector<table>& tables)
           : types(types), structs(structs), actions(actions), tables(tables) {}

        vector<type_def>                 types;
        vector<struct_t>                 structs;
        vector<action>                   actions;
        vector<table>                    tables;
    };

    template<> struct get_struct<abi> { 
        static const struct_t& type() { 
           static struct_t result = { "abi", "", {
                {"types", "type_def[]"},
                {"structs", "struct_t[]"},
                {"actions", "action[]"},
                {"tables", "table[]"},
              }
           };
           return result;
         }
    };

    struct transfer { 
        transfer() = default;
        transfer(const account_name& from, const account_name& to, const uint64& amount, const string& memo)
           : from(from), to(to), amount(amount), memo(memo) {}

        account_name                     from;
        account_name                     to;
        uint64                           amount;
        string                           memo;
    };

    template<> struct get_struct<transfer> { 
        static const struct_t& type() { 
           static struct_t result = { "transfer", "", {
                {"from", "account_name"},
                {"to", "account_name"},
                {"amount", "uint64"},
                {"memo", "string"},
              }
           };
           return result;
         }
    };

    struct nonce { 
        nonce() = default;
        nonce(const string& value)
           : value(value) {}

        string                           value;
    };

    template<> struct get_struct<nonce> { 
        static const struct_t& type() { 
           static struct_t result = { "nonce", "", {
                {"value", "string"},
              }
           };
           return result;
         }
    };

    struct lock { 
        lock() = default;
        lock(const account_name& from, const account_name& to, const share_type& amount)
           : from(from), to(to), amount(amount) {}

        account_name                     from;
        account_name                     to;
        share_type                       amount;
    };

    template<> struct get_struct<lock> { 
        static const struct_t& type() { 
           static struct_t result = { "lock", "", {
                {"from", "account_name"},
                {"to", "account_name"},
                {"amount", "share_type"},
              }
           };
           return result;
         }
    };

    struct unlock { 
        unlock() = default;
        unlock(const account_name& account, const share_type& amount)
           : account(account), amount(amount) {}

        account_name                     account;
        share_type                       amount;
    };

    template<> struct get_struct<unlock> { 
        static const struct_t& type() { 
           static struct_t result = { "unlock", "", {
                {"account", "account_name"},
                {"amount", "share_type"},
              }
           };
           return result;
         }
    };

    struct claim { 
        claim() = default;
        claim(const account_name& account, const share_type& amount)
           : account(account), amount(amount) {}

        account_name                     account;
        share_type                       amount;
    };

    template<> struct get_struct<claim> { 
        static const struct_t& type() { 
           static struct_t result = { "claim", "", {
                {"account", "account_name"},
                {"amount", "share_type"},
              }
           };
           return result;
         }
    };

    struct addaccount { 
        addaccount() = default;
        addaccount(const account_name& creator, const account_name& name, const authority& owner, const authority& active, const asset& deposit)
           : creator(creator), name(name), owner(owner), active(active), deposit(deposit) {}

        account_name                     creator;
        account_name                     name;
        authority                        owner;
        authority                        active;
        asset                            deposit;
    };

    template<> struct get_struct<addaccount> { 
        static const struct_t& type() { 
           static struct_t result = { "addaccount", "", {
                {"creator", "account_name"},
                {"name", "account_name"},
                {"owner", "authority"},
                {"active", "authority"},
                {"deposit", "asset"},
              }
           };
           return result;
         }
    };

    struct addcontract { 
        addcontract() = default;
        addcontract(const account_name& creator, const account_name& name, const bytes& code, const abi& code_abi)
           : creator(creator), name(name), code(code), code_abi(code_abi) {}

        account_name                     creator;
        account_name                     name;
        bytes                            code;
        abi                              code_abi;
    };

    template<> struct get_struct<addcontract> { 
        static const struct_t& type() { 
           static struct_t result = { "addcontract", "", {
                {"creator", "account_name"},
                {"name", "account_name"},
                {"code", "bytes"},
                {"code_abi", "abi"},
              }
           };
           return result;
         }
    };

    struct adderc20 { 
        adderc20() = default;
        adderc20(const account_name& creator, const asset_symbol& token_name, const asset& total_balance, const uint64& decimal)
           : creator(creator), token_name(token_name), total_balance(total_balance), decimal(decimal) {}

        account_name                     creator;
        asset_symbol                     token_name;
        asset                            total_balance;
        uint64                           decimal;
    };

    template<> struct get_struct<adderc20> { 
        static const struct_t& type() { 
           static struct_t result = { "adderc20", "", {
                {"creator", "account_name"},
                {"token_name", "asset_symbol"},
                {"total_balance", "asset"},
                {"decimal", "uint64"},
              }
           };
           return result;
         }
    };

    struct adderc721 { 
        adderc721() = default;
        adderc721(const account_name& creator, const asset_symbol& token_name)
           : creator(creator), token_name(token_name) {}

        account_name                     creator;
        asset_symbol                     token_name;
    };

    template<> struct get_struct<adderc721> { 
        static const struct_t& type() { 
           static struct_t result = { "adderc721", "", {
                {"creator", "account_name"},
                {"token_name", "asset_symbol"},
              }
           };
           return result;
         }
    };

    struct setcode { 
        setcode() = default;
        setcode(const account_name& account, const uint8& vm_type, const uint8& vm_version, const bytes& code, const abi& code_abi)
           : account(account), vm_type(vm_type), vm_version(vm_version), code(code), code_abi(code_abi) {}

        account_name                     account;
        uint8                            vm_type;
        uint8                            vm_version;
        bytes                            code;
        abi                              code_abi;
    };

    template<> struct get_struct<setcode> { 
        static const struct_t& type() { 
           static struct_t result = { "setcode", "", {
                {"account", "account_name"},
                {"vm_type", "uint8"},
                {"vm_version", "uint8"},
                {"code", "bytes"},
                {"code_abi", "abi"},
              }
           };
           return result;
         }
    };

    struct updateauth { 
        updateauth() = default;
        updateauth(const account_name& account, const authority_name& permission, const authority_name& parent, const authority& new_authority)
           : account(account), permission(permission), parent(parent), new_authority(new_authority) {}

        account_name                     account;
        authority_name                   permission;
        authority_name                   parent;
        authority                        new_authority;
    };

    template<> struct get_struct<updateauth> { 
        static const struct_t& type() { 
           static struct_t result = { "updateauth", "", {
                {"account", "account_name"},
                {"permission", "authority_name"},
                {"parent", "authority_name"},
                {"new_authority", "authority"},
              }
           };
           return result;
         }
    };

    struct deleteauth { 
        deleteauth() = default;
        deleteauth(const account_name& account, const authority_name& permission)
           : account(account), permission(permission) {}

        account_name                     account;
        authority_name                   permission;
    };

    template<> struct get_struct<deleteauth> { 
        static const struct_t& type() { 
           static struct_t result = { "deleteauth", "", {
                {"account", "account_name"},
                {"permission", "authority_name"},
              }
           };
           return result;
         }
    };

    struct linkauth { 
        linkauth() = default;
        linkauth(const account_name& account, const account_name& code, const func_name& type, const authority_name& requirement)
           : account(account), code(code), type(type), requirement(requirement) {}

        account_name                     account;
        account_name                     code;
        func_name                        type;
        authority_name                   requirement;
    };

    template<> struct get_struct<linkauth> { 
        static const struct_t& type() { 
           static struct_t result = { "linkauth", "", {
                {"account", "account_name"},
                {"code", "account_name"},
                {"type", "func_name"},
                {"requirement", "authority_name"},
              }
           };
           return result;
         }
    };

    struct unlinkauth { 
        unlinkauth() = default;
        unlinkauth(const account_name& account, const account_name& code, const func_name& type)
           : account(account), code(code), type(type) {}

        account_name                     account;
        account_name                     code;
        func_name                        type;
    };

    template<> struct get_struct<unlinkauth> { 
        static const struct_t& type() { 
           static struct_t result = { "unlinkauth", "", {
                {"account", "account_name"},
                {"code", "account_name"},
                {"type", "func_name"},
              }
           };
           return result;
         }
    };

    struct votebuilder { 
        votebuilder() = default;
        votebuilder(const account_name& voter, const account_name& proxy, const vector<account_name>& builders)
           : voter(voter), proxy(proxy), builders(builders) {}

        account_name                     voter;
        account_name                     proxy;
        vector<account_name>             builders;
    };

    template<> struct get_struct<votebuilder> { 
        static const struct_t& type() { 
           static struct_t result = { "votebuilder", "", {
                {"voter", "account_name"},
                {"proxy", "account_name"},
                {"builders", "account_name[]"},
              }
           };
           return result;
         }
    };

    struct regbuilder { 
        regbuilder() = default;
        regbuilder(const account_name& builder, const public_key& builder_key)
           : builder(builder), builder_key(builder_key) {}

        account_name                     builder;
        public_key                       builder_key;
    };

    template<> struct get_struct<regbuilder> { 
        static const struct_t& type() { 
           static struct_t result = { "regbuilder", "", {
                {"builder", "account_name"},
                {"builder_key", "public_key"},
              }
           };
           return result;
         }
    };

    struct unregbuilder { 
        unregbuilder() = default;
        unregbuilder(const account_name& builder)
           : builder(builder) {}

        account_name                     builder;
    };

    template<> struct get_struct<unregbuilder> { 
        static const struct_t& type() { 
           static struct_t result = { "unregbuilder", "", {
                {"builder", "account_name"},
              }
           };
           return result;
         }
    };

    struct regproxy { 
        regproxy() = default;
        regproxy(const account_name& proxy)
           : proxy(proxy) {}

        account_name                     proxy;
    };

    template<> struct get_struct<regproxy> { 
        static const struct_t& type() { 
           static struct_t result = { "regproxy", "", {
                {"proxy", "account_name"},
              }
           };
           return result;
         }
    };

    struct unregproxy { 
        unregproxy() = default;
        unregproxy(const account_name& proxy)
           : proxy(proxy) {}

        account_name                     proxy;
    };

    template<> struct get_struct<unregproxy> { 
        static const struct_t& type() { 
           static struct_t result = { "unregproxy", "", {
                {"proxy", "account_name"},
              }
           };
           return result;
         }
    };

    struct issueerc20 { 
        issueerc20() = default;
        issueerc20(const account_name& creator, const authority& owner, const authority& active, const asset_symbol& token_name, const asset& total_balance)
           : creator(creator), owner(owner), active(active), token_name(token_name), total_balance(total_balance) {}

        account_name                     creator;
        authority                        owner;
        authority                        active;
        asset_symbol                     token_name;
        asset                            total_balance;
    };

    template<> struct get_struct<issueerc20> { 
        static const struct_t& type() { 
           static struct_t result = { "issueerc20", "", {
                {"creator", "account_name"},
                {"owner", "authority"},
                {"active", "authority"},
                {"token_name", "asset_symbol"},
                {"total_balance", "asset"},
              }
           };
           return result;
         }
    };

    struct minterc20 { 
        minterc20() = default;
        minterc20(const asset_symbol& token_name, const asset& mint_amount)
           : token_name(token_name), mint_amount(mint_amount) {}

        asset_symbol                     token_name;
        asset                            mint_amount;
    };

    template<> struct get_struct<minterc20> { 
        static const struct_t& type() { 
           static struct_t result = { "minterc20", "", {
                {"token_name", "asset_symbol"},
                {"mint_amount", "asset"},
              }
           };
           return result;
         }
    };

    struct stopminterc20 { 
        stopminterc20() = default;
        stopminterc20(const asset_symbol& token_name)
           : token_name(token_name) {}

        asset_symbol                     token_name;
    };

    template<> struct get_struct<stopminterc20> { 
        static const struct_t& type() { 
           static struct_t result = { "stopminterc20", "", {
                {"token_name", "asset_symbol"},
              }
           };
           return result;
         }
    };

    struct revokeerc20 { 
        revokeerc20() = default;
        revokeerc20(const asset_symbol& token_name)
           : token_name(token_name) {}

        asset_symbol                     token_name;
    };

    template<> struct get_struct<revokeerc20> { 
        static const struct_t& type() { 
           static struct_t result = { "revokeerc20", "", {
                {"token_name", "asset_symbol"},
              }
           };
           return result;
         }
    };

    struct transferfromerc20 { 
        transferfromerc20() = default;
        transferfromerc20(const asset_symbol& token_name, const account_name& from, const account_name& to, const asset& value)
           : token_name(token_name), from(from), to(to), value(value) {}

        asset_symbol                     token_name;
        account_name                     from;
        account_name                     to;
        asset                            value;
    };

    template<> struct get_struct<transferfromerc20> { 
        static const struct_t& type() { 
           static struct_t result = { "transferfromerc20", "", {
                {"token_name", "asset_symbol"},
                {"from", "account_name"},
                {"to", "account_name"},
                {"value", "asset"},
              }
           };
           return result;
         }
    };

    struct issueerc721 { 
        issueerc721() = default;
        issueerc721(const account_name& creator, const authority& owner, const authority& active, const asset_symbol& token_name)
           : creator(creator), owner(owner), active(active), token_name(token_name) {}

        account_name                     creator;
        authority                        owner;
        authority                        active;
        asset_symbol                     token_name;
    };

    template<> struct get_struct<issueerc721> { 
        static const struct_t& type() { 
           static struct_t result = { "issueerc721", "", {
                {"creator", "account_name"},
                {"owner", "authority"},
                {"active", "authority"},
                {"token_name", "asset_symbol"},
              }
           };
           return result;
         }
    };

    struct minterc721 { 
        minterc721() = default;
        minterc721(const asset_symbol& token_name, const fixed_string32& token_id)
           : token_name(token_name), token_id(token_id) {}

        asset_symbol                     token_name;
        fixed_string32                   token_id;
    };

    template<> struct get_struct<minterc721> { 
        static const struct_t& type() { 
           static struct_t result = { "minterc721", "", {
                {"token_name", "asset_symbol"},
                {"token_id", "fixed_string32"},
              }
           };
           return result;
         }
    };

    struct stopminterc721 { 
        stopminterc721() = default;
        stopminterc721(const asset_symbol& token_name)
           : token_name(token_name) {}

        asset_symbol                     token_name;
    };

    template<> struct get_struct<stopminterc721> { 
        static const struct_t& type() { 
           static struct_t result = { "stopminterc721", "", {
                {"token_name", "asset_symbol"},
              }
           };
           return result;
         }
    };

    struct transferfromerc721 { 
        transferfromerc721() = default;
        transferfromerc721(const asset_symbol& token_name, const fixed_string32& token_id, const account_name& from, const account_name& to)
           : token_name(token_name), token_id(token_id), from(from), to(to) {}

        asset_symbol                     token_name;
        fixed_string32                   token_id;
        account_name                     from;
        account_name                     to;
    };

    template<> struct get_struct<transferfromerc721> { 
        static const struct_t& type() { 
           static struct_t result = { "transferfromerc721", "", {
                {"token_name", "asset_symbol"},
                {"token_id", "fixed_string32"},
                {"from", "account_name"},
                {"to", "account_name"},
              }
           };
           return result;
         }
    };

    struct revokeerc721 { 
        revokeerc721() = default;
        revokeerc721(const asset_symbol& token_name)
           : token_name(token_name) {}

        asset_symbol                     token_name;
    };

    template<> struct get_struct<revokeerc721> { 
        static const struct_t& type() { 
           static struct_t result = { "revokeerc721", "", {
                {"token_name", "asset_symbol"},
              }
           };
           return result;
         }
    };

    struct cash_input { 
        cash_input() = default;
        cash_input(const uint32& num, const uint16& slot)
           : num(num), slot(slot) {}

        uint32                           num;
        uint16                           slot;
    };

    template<> struct get_struct<cash_input> { 
        static const struct_t& type() { 
           static struct_t result = { "cash_input", "", {
                {"num", "uint32"},
                {"slot", "uint16"},
              }
           };
           return result;
         }
    };

    struct cash_output { 
        cash_output() = default;
        cash_output(const uint64& amount, const address& to)
           : amount(amount), to(to) {}

        uint64                           amount;
        address                          to;
    };

    template<> struct get_struct<cash_output> { 
        static const struct_t& type() { 
           static struct_t result = { "cash_output", "", {
                {"amount", "uint64"},
                {"to", "address"},
              }
           };
           return result;
         }
    };

    struct cash_detail { 
        cash_detail() = default;
        cash_detail(const vector<cash_input>& input, const vector<cash_output>& output, const signature& sig)
           : input(input), output(output), sig(sig) {}

        vector<cash_input>               input;
        vector<cash_output>              output;
        signature                        sig;
    };

    template<> struct get_struct<cash_detail> { 
        static const struct_t& type() { 
           static struct_t result = { "cash_detail", "", {
                {"input", "cash_input[]"},
                {"output", "cash_output[]"},
                {"sig", "signature"},
              }
           };
           return result;
         }
    };

    struct transfercash { 
        transfercash() = default;
        transfercash(const cash_detail& detail)
           : detail(detail) {}

        cash_detail                      detail;
    };

    template<> struct get_struct<transfercash> { 
        static const struct_t& type() { 
           static struct_t result = { "transfercash", "", {
                {"detail", "cash_detail"},
              }
           };
           return result;
         }
    };

}} // namespace Xmaxplatform::Basetypes
FC_REFLECT( Xmaxplatform::Basetypes::account_auth                     , (account)(authority) )
FC_REFLECT( Xmaxplatform::Basetypes::message                          , (code)(type)(authorization)(data) )
FC_REFLECT( Xmaxplatform::Basetypes::account_permission_weight        , (permission)(weight) )
FC_REFLECT( Xmaxplatform::Basetypes::transaction                      , (ref_block_num)(ref_block_prefix)(expiration)(scope)(read_scope)(messages)(gas_payer)(gas)(gaslimit) )
FC_REFLECT_DERIVED( Xmaxplatform::Basetypes::signed_transaction, (Xmaxplatform::Basetypes::transaction), (signatures) )
FC_REFLECT( Xmaxplatform::Basetypes::key_permission_weight            , (key)(weight) )
FC_REFLECT( Xmaxplatform::Basetypes::authority                        , (threshold)(keys)(accounts) )
FC_REFLECT( Xmaxplatform::Basetypes::blockchain_configuration         , (max_blk_size)(target_blk_size)(max_storage_size)(elected_pay)(runner_up_pay)(min_xmx_balance)(max_trx_lifetime)(auth_depth_limit)(max_trx_runtime)(in_depth_limit)(max_in_msg_size)(max_gen_trx_size) )
FC_REFLECT( Xmaxplatform::Basetypes::type_def                         , (new_type_name)(type) )
FC_REFLECT( Xmaxplatform::Basetypes::action                           , (action_name)(type) )
FC_REFLECT( Xmaxplatform::Basetypes::table                            , (table_name)(index_type)(key_names)(key_types)(type) )
FC_REFLECT( Xmaxplatform::Basetypes::abi                              , (types)(structs)(actions)(tables) )
FC_REFLECT( Xmaxplatform::Basetypes::transfer                         , (from)(to)(amount)(memo) )
FC_REFLECT( Xmaxplatform::Basetypes::nonce                            , (value) )
FC_REFLECT( Xmaxplatform::Basetypes::lock                             , (from)(to)(amount) )
FC_REFLECT( Xmaxplatform::Basetypes::unlock                           , (account)(amount) )
FC_REFLECT( Xmaxplatform::Basetypes::claim                            , (account)(amount) )
FC_REFLECT( Xmaxplatform::Basetypes::addaccount                       , (creator)(name)(owner)(active)(deposit) )
FC_REFLECT( Xmaxplatform::Basetypes::addcontract                      , (creator)(name)(code)(code_abi) )
FC_REFLECT( Xmaxplatform::Basetypes::adderc20                         , (creator)(token_name)(total_balance)(decimal) )
FC_REFLECT( Xmaxplatform::Basetypes::adderc721                        , (creator)(token_name) )
FC_REFLECT( Xmaxplatform::Basetypes::setcode                          , (account)(vm_type)(vm_version)(code)(code_abi) )
FC_REFLECT( Xmaxplatform::Basetypes::updateauth                       , (account)(permission)(parent)(new_authority) )
FC_REFLECT( Xmaxplatform::Basetypes::deleteauth                       , (account)(permission) )
FC_REFLECT( Xmaxplatform::Basetypes::linkauth                         , (account)(code)(type)(requirement) )
FC_REFLECT( Xmaxplatform::Basetypes::unlinkauth                       , (account)(code)(type) )
FC_REFLECT( Xmaxplatform::Basetypes::votebuilder                      , (voter)(proxy)(builders) )
FC_REFLECT( Xmaxplatform::Basetypes::regbuilder                       , (builder)(builder_key) )
FC_REFLECT( Xmaxplatform::Basetypes::unregbuilder                     , (builder) )
FC_REFLECT( Xmaxplatform::Basetypes::regproxy                         , (proxy) )
FC_REFLECT( Xmaxplatform::Basetypes::unregproxy                       , (proxy) )
FC_REFLECT( Xmaxplatform::Basetypes::issueerc20                       , (creator)(owner)(active)(token_name)(total_balance) )
FC_REFLECT( Xmaxplatform::Basetypes::minterc20                        , (token_name)(mint_amount) )
FC_REFLECT( Xmaxplatform::Basetypes::stopminterc20                    , (token_name) )
FC_REFLECT( Xmaxplatform::Basetypes::revokeerc20                      , (token_name) )
FC_REFLECT( Xmaxplatform::Basetypes::transferfromerc20                , (token_name)(from)(to)(value) )
FC_REFLECT( Xmaxplatform::Basetypes::issueerc721                      , (creator)(owner)(active)(token_name) )
FC_REFLECT( Xmaxplatform::Basetypes::minterc721                       , (token_name)(token_id) )
FC_REFLECT( Xmaxplatform::Basetypes::stopminterc721                   , (token_name) )
FC_REFLECT( Xmaxplatform::Basetypes::transferfromerc721               , (token_name)(token_id)(from)(to) )
FC_REFLECT( Xmaxplatform::Basetypes::revokeerc721                     , (token_name) )
FC_REFLECT( Xmaxplatform::Basetypes::cash_input                       , (num)(slot) )
FC_REFLECT( Xmaxplatform::Basetypes::cash_output                      , (amount)(to) )
FC_REFLECT( Xmaxplatform::Basetypes::cash_detail                      , (input)(output)(sig) )
FC_REFLECT( Xmaxplatform::Basetypes::transfercash                     , (detail) )
