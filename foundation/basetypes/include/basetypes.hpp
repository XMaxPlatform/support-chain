/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <basedef.hpp>

#include <shortname.hpp>
#include <longname.hpp>

#define STN(X) Xmaxplatform::Basetypes::name::to_name_code(#X)


namespace Xmaxplatform { namespace Basetypes {       

		using name = LongCode::long_name<Xmaxplatform::Basetypes::uint128>;
        struct field {
            field_name name;
            type_name  type;

            bool operator==(const field& other) const;
        };

        struct struct_t {
            type_name        name;
            type_name        base;
            vector<field>    fields;

            bool operator==(const struct_t& other) const;
        };

        using fields = vector<field>;

        template<typename T>
        struct get_struct{};

        template<> struct get_struct<field> {
            static const struct_t& type() {
                static struct_t result = { "field ", "", {
                        {"name", "field_name"},
                        {"type", "type_name"}
                }
                };
                return result;
            }
        };
        template<> struct get_struct<struct_t> {
            static const struct_t& type() {
                static struct_t result = { "struct_t ", "", {
                        {"name", "type_name"},
                        {"base", "type_name"},
                        {"fields", "field[]"}
                }
                };
                return result;
            }
        };


        /// TODO: make sure this works with FC raw
        template<typename Stream, typename Number>
        void from_binary(Stream& st, boost::multiprecision::number<Number>& value) {
            unsigned char data[(std::numeric_limits<decltype(value)>::digits+1)/8];
            st.read((char*)data, sizeof(data));
            boost::multiprecision::import_bits(value, data, data + sizeof(data), 1);
        }
        template<typename Stream, typename Number>
        void to_binary(Stream& st, const boost::multiprecision::number<Number>& value) {
            unsigned char data[(std::numeric_limits<decltype(value)>::digits+1)/8];
            boost::multiprecision::export_bits(value, data, 1);
            st.write((const char*)data, sizeof(data));
        }

    }} // namespace Xmaxplatform::Basetypes

namespace fc {
    void to_variant(const Xmaxplatform::Basetypes::name& c, fc::variant& v);
    void from_variant(const fc::variant& v, Xmaxplatform::Basetypes::name& check);
    void to_variant(const std::vector<Xmaxplatform::Basetypes::field>& c, fc::variant& v);
    void from_variant(const fc::variant& v, std::vector<Xmaxplatform::Basetypes::field>& check);
    void to_variant(const std::map<std::string,Xmaxplatform::Basetypes::struct_t>& c, fc::variant& v);
    void from_variant(const fc::variant& v, std::map<std::string,Xmaxplatform::Basetypes::struct_t>& check);
}

FC_REFLECT(Xmaxplatform::Basetypes::name, (namecode_))
FC_REFLECT(Xmaxplatform::Basetypes::field, (name)(type))
FC_REFLECT(Xmaxplatform::Basetypes::struct_t, (name)(base)(fields))

#include <fc/exception/exception.hpp>
#include <exception_macros.hpp>

namespace Xmaxplatform { namespace Basetypes {

        FC_DECLARE_EXCEPTION(type_exception, 4000000, "type exception")
        FC_DECLARE_DERIVED_EXCEPTION(unknown_type_exception, type_exception,
                                     4010000, "Could not find requested type")
        FC_DECLARE_DERIVED_EXCEPTION(duplicate_type_exception, type_exception,
                                     4020000, "Requested type already exists")
        FC_DECLARE_DERIVED_EXCEPTION(invalid_type_name_exception, type_exception,
                                     4030000, "Requested type name is invalid")
        FC_DECLARE_DERIVED_EXCEPTION(invalid_field_name_exception, type_exception,
                                     4040000, "Requested field name is invalid")
        FC_DECLARE_DERIVED_EXCEPTION(invalid_schema_exception, type_exception,
                                     4050000, "Schema is invalid")
		FC_DECLARE_DERIVED_EXCEPTION(invalid_operator_exception, type_exception,
									 4060000, "Operator is invalid")

    } } // Xmaxplatform::Basetypes