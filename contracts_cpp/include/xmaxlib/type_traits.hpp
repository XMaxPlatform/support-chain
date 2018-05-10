
#pragma once
#include <xmaxlib/core.h>

namespace  Xmaxplatform {

	template<typename T> struct remove_reference           { typedef T type; };
	template<typename T> struct remove_reference<T&>       { typedef T type; };
	template<typename T> struct remove_reference<const T&> { typedef T type; };

	
    template<typename T, typename U>
    constexpr decltype(auto) forward(U && u) noexcept
    {
       return static_cast<T &&>(u);
    }

    template< class T >
    constexpr typename remove_reference<T>::type&& move( T&& t ) noexcept {
       return static_cast<typename remove_reference<decltype(t)>::type&&>(t);
    }

  
    template<class T, T v>
    struct integral_constant {
        static constexpr T value = v;
        typedef T value_type;
        typedef integral_constant type; // using injected-class-name
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; } //since c++14
    };

 
    using true_type  = integral_constant<bool,true>;
    using false_type = integral_constant<bool,false>;


    template<class T, class U>
    struct is_same : false_type {};

    template<class T>
    struct is_same<T, T> : true_type {};

    template<class...> struct voidify { using type = void; };
    template<class... Ts> using void_t = typename voidify<Ts...>::type;

    template<class T, class = void>
    struct supports_arithmetic_operations : false_type {};

    //for no default ctor use Xmaxplatform::declval<T>() +-/* Xmaxplatform::declval<T>()
    template<class T>
    struct supports_arithmetic_operations<T,
            void_t<decltype(T() + T()),
                    decltype(T() - T()),
                    decltype(T() * T()),
                    decltype(T() / T())>>
            : true_type {};


    namespace detail {
        template<typename T,bool = supports_arithmetic_operations<T>::value>
        struct is_unsigned : integral_constant<bool, T(0) < T(-1)> {};

        template<typename T>
        struct is_unsigned<T,false> : false_type {};
    } // namespace detail

    template<typename T>
    struct is_unsigned : detail::is_unsigned<T>::type {};

    template<bool B, class T = void>
    struct enable_if {};

    template<class T>
    struct enable_if<true, T> { typedef T type; };


} // namespace Xmaxplatform
