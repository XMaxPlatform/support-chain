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
class const_db_table
{
public:
    typedef ObjectType table_object_type;
    typedef IndexedByType table_indexed_by_type;
    typedef typename Basechain::get_index_type< ObjectType >::type multi_index_type;

    template<typename CompatibleKey >
    const table_object_type* find(CompatibleKey& key)
    {
        return _database.find<table_object_type, table_indexed_by_type>(key);
    }
    template<typename CompatibleKey >
    const table_object_type& get(CompatibleKey& key)
    {
        return _database.get<table_object_type, table_indexed_by_type>(key);
    }

    template<typename ByIndex>
    auto get_index()const -> decltype( ((Basechain::generic_index<multi_index_type>*)( nullptr ))->indicies().template get<ByIndex>() )
    {
        return _database.get_index<multi_index_type, ByIndex>();
    }

    const_db_table(const Basechain::database& database)
    : _database(database)
     {

     }

protected:
    const Basechain::database& _database;
};

template<typename ObjectType, typename IndexedByType>
class mutable_db_table : public const_db_table<ObjectType, IndexedByType>
{
public:
    typedef const_db_table<ObjectType, IndexedByType> super;
    typedef typename super::table_object_type table_object_type;
    typedef typename super::table_indexed_by_type table_indexed_by_type;
    typedef typename super::multi_index_type multi_index_type;

    template<typename Modifier>
    void modify( const table_object_type* obj, Modifier&& m )
    {
        if(obj)
            _db.modify(*obj, m);
    }
    template<typename Modifier>
    void modify( const table_object_type& obj, Modifier&& m )
    {
        _db.modify(obj, m);
    }

    template<typename Constructor>
    const table_object_type* emplace( Constructor&& c )
    {
        return &_db.create< table_object_type >(c);
    }

    mutable_db_table(Basechain::database& database)
            : super(database)
            , _db(database)

    {
    }

private:
    Basechain::database& _db;
};




}}
