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
    };

    struct builder_rule
    {
    public:
        uint32_t            version;   ///< sequentially incrementing version number
        xmax_builders       builders;

        inline builder_rule()
        {
            version = 0;
        }

        void set_builders(const xmax_builders& list, uint32_t vers)
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

		bool operator == (const builder_rule& rh) const
		{
			return (this->version == rh.version) && (this->builders == rh.builders);
		}


    };



}}
