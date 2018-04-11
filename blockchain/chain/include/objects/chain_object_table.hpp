/**
 *  @file
 *  @copyright defined in xmax/LICENSE.txt
 */
#pragma once

#include <objects/multi_index_includes.hpp>

#include <basechain.hpp>

namespace Xmaxplatform {
namespace Chain {

template<typename ObjectType, typename IndexedByType>
class db_object_table
{
public:
    typedef ObjectType object_type;
    typedef IndexedByType index_by_type;
    typedef typename Basechain::get_index_type< ObjectType >::type multi_index_type;

    template<typename CompatibleKey >
    const object_type* find(CompatibleKey& key)
    {
        return _database.find<object_type, index_by_type>(key);
    }

    template<typename Modifier>
    void modify( const object_type* obj, Modifier&& m )
    {
        if(obj)
            _database.modify(*obj, m);
    }

    template<typename Constructor>
    const object_type* emplace( Constructor&& c )
    {
        return &_database.create<object_type>(c);
    }

    template<typename ByIndex>
    auto get_index()const -> decltype( ((Basechain::generic_index<multi_index_type>*)( nullptr ))->indicies().template get<ByIndex>() )
    {
        return _database.get_index<multi_index_type, ByIndex>();
    }
    db_object_table(Basechain::database& database)
            : _database(database)
    {
    }

private:
    Basechain::database& _database;
};




}}
