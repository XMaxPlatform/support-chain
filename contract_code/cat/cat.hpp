/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#include <xmaxlib/xmax.hpp>
#include <xmaxlib/core.h>
#include <xmaxlib/database.hpp>
#ifndef MYNAME
#define MYNAME cat
#endif

namespace MYNAME {
    /**
     * @brief Set the destination ABI struct to write
     * @param output ABI destination
     * @abi action msg
     */
    struct sellmsg {
        uint64_t name;
        uint64_t id;
    };

    struct buymsg {
        uint64_t name;
        uint64_t id;
    };

    struct bearmsg {
        uint64_t name;
        uint64_t id;
        uint64_t to;
    };

    struct coolmsg {
        uint64_t name;
        uint64_t id;
        uint64_t amount;
    };

    /**
     * @brief Set the destination ABI struct to write
     * @param output ABI destination
     * @abi table cat
     */
    struct cat {
        uint64_t id;
        uint64_t name;
        uint64_t price;
        uint64_t sell;
        uint64_t growth;
        uint64_t bear;
        uint64_t smart;
        uint64_t charm;
        uint64_t generation;
        uint64_t feedvalue;
        uint64_t bearcount;
        uint64_t canbear;
        uint64_t cooltime;
        uint64_t gene;
        bool can_bear() const { return canbear && (cooltime <= xmax_now()); }
    };

    /**
     * @brief Set the destination ABI struct to write
     * @param output ABI destination
     * @abi table account
     */
    struct account {
        uint64_t key = XNAME(cat);
        uint64_t feedcount;
        uint64_t reward;
    };

    /**
     * @brief Set the destination ABI struct to write
     * @param output ABI destination
     * @abi table catown
     */
    struct catown {
        uint64_t id;
        uint64_t sell;
    };

    /**
     * @brief Set the destination ABI struct to write
     * @param output ABI destination
     * @abi table catsell
     */
    struct catsell {
        uint64_t id;
        uint64_t price;
    };

    using accounts = Xmaxplatform::table<XNAME(defaultscope), XNAME(cat), XNAME(account), account, uint64_t>;
    using cats = Xmaxplatform::table<XNAME(defaultscope), XNAME(cat), XNAME(cat), cat, uint64_t>;
    using catowns = Xmaxplatform::table<XNAME(defaultscope), XNAME(cat), XNAME(catown), catown, uint64_t>;
    using catsells = Xmaxplatform::table<XNAME(defaultscope), XNAME(cat), XNAME(catsell), catsell, uint64_t>;

    inline account get_account(uint64_t name) {
        account owner;
        accounts::get(owner, name);
        return owner;
    }
}
