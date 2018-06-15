/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once
#include <blockchain_types.hpp>
#include <vector>


namespace Xmaxplatform { namespace Chain {

    struct builder_info
    {
        account_name        builder_name;
        public_key_type     block_signing_key;

		builder_info(account_name name, const public_key_type& key)
			: builder_name(name)
			, block_signing_key(key)
		{

		}
		builder_info()
		{

		}
    };

	inline bool operator == (const builder_info& lh, const builder_info& rh)
	{
		return (lh.builder_name == rh.builder_name) && (lh.block_signing_key == rh.block_signing_key);
	}

	inline bool operator != (const builder_info& lh, const builder_info& rh)
	{
		return (lh.builder_name != rh.builder_name) || (lh.block_signing_key != rh.block_signing_key);
	}

	typedef std::vector<builder_info> xmax_builder_infos;

    struct builder_rule
    {
    public:
        uint32_t			version;   ///< sequentially incrementing version number
		xmax_builder_infos  builders;

        inline builder_rule()
        {
            version = 0;
        }

        void set_builders(const xmax_builder_infos& list, uint32_t vers)
        {
            builders = list;
			version = vers;
        }

		void reset()
		{
			builders.clear();
			version = 0;
		}

		inline int number() const
		{
			return builders.size();
		}

		inline bool is_empty() const
		{
			return builders.size() <= 0;
		}

		public_key_type get_sign_key(account_name name) const
		{
			for (const auto& it : builders)
			{
				if (it.builder_name == name)
				{
					return it.block_signing_key;
				}
			}
			return empty_public_key;
		}

    };

	inline bool operator == (const builder_rule& a, const builder_rule& b)
	{
		if (a.version != b.version) return false;
		if (a.builders.size() != b.builders.size()) return false;
		for (uint32_t i = 0; i < a.builders.size(); ++i)
			if (a.builders[i] != b.builders[i]) return false;
		return true;
	}
	inline bool operator != (const builder_rule& a, const builder_rule& b)
	{
		return !(a == b);
	}

	struct shared_builder_rule {
		shared_builder_rule(Chain::allocator<char> alloc)
			:builders(alloc) {}

		shared_builder_rule& operator=(const builder_rule& a) {
			version = a.version;
			builders.clear();
			builders.reserve(a.builders.size());
			for (const auto& p : a.builders)
				builders.push_back(p);
			return *this;
		}

		operator builder_rule() const {
			builder_rule result;
			result.version = version;
			result.builders.reserve(builders.size());
			for (const auto& p : builders)
				result.builders.push_back(p);
			return result;
		}

		void set_builders(const xmax_builder_infos& list, uint32_t vers)
		{
			builders.clear();
			builders.assign(list.begin(), list.end());
			version = vers;
		}
		void set_builders(const shared_vector<builder_info>& list, uint32_t vers)
		{
			builders = list;
			version = vers;
		}		void reset()
		{
			builders.clear();
			version = 0;
		}
		inline int number() const
		{
			return builders.size();
		}

		inline bool is_empty() const
		{
			return builders.size() <= 0;
		}
		uint32_t                                       version = 0; ///< sequentially incrementing version number
		shared_vector<builder_info>                    builders;
	};

}}
FC_REFLECT(Xmaxplatform::Chain::builder_info, (builder_name)(block_signing_key))
FC_REFLECT(Xmaxplatform::Chain::builder_rule, (version)(builders))
