/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <blockchain_types.hpp>

namespace Xmaxplatform { namespace Chain {

	template<typename _Ty>
	class mapped_ptr
	{
	public:

		using value_type = _Ty;
		using alloc_type = allocator<_Ty>;
		using pointer = typename alloc_type::pointer;

		template<typename _Any_alloc>
		mapped_ptr(_Any_alloc al)
			: _al(al)
		{

		}
		mapped_ptr(const mapped_ptr& opt)
			: _al(opt._al)
		{

		}

		~mapped_ptr()
		{
			reset();
		}

		bool valid() const
		{
			return bool(getpointer());
		}

		bool empty() const
		{
			return !getpointer();
		}

		void reset()
		{
			if (valid())
			{
				dealloc();
			}
		}

		void init()
		{
			assign(value_type());
		}

		explicit operator bool() const
		{
			return valid();
		}

		bool operator!() const
		{
			return empty();
		}


		value_type& operator*()
		{
			assert(valid());
			return getref();
		}
		const value_type& operator*() const
		{
			assert(valid());
			return getref();
		}

		value_type* operator->()
		{
			assert(valid());
			return getptr();
		}
		const value_type* operator->() const
		{
			assert(valid());
			return getptr();
		}

		mapped_ptr& operator=(std::nullptr_t)
		{
			reset();
			return *this;
		}

		mapped_ptr& operator = (const mapped_ptr& opt)
		{
			assign(opt);
			return *this;
		}

		mapped_ptr& operator = (mapped_ptr&& opt)
		{
			assign(std::forward<mapped_ptr>(opt));
			return *this;
		}

		friend bool operator < (const mapped_ptr a, mapped_ptr b)
		{
			if (a.valid() && b.valid())
				return *a < *b;
			return a.valid() < b.valid();
		}
		friend bool operator == (const mapped_ptr a, mapped_ptr b)
		{
			if (a.valid() && b.valid())
				return *a == *b;
			return a.valid() == b.valid();
		}

		void assign(const value_type& val)
		{
			if (empty())
			{
				alloc();
			}
			getref() = val;
		}

		void assign(value_type&& val)
		{
			if (empty())
			{
				alloc();
			}
			getref() = std::forward<value_type>(val);
		}

		void assign(const mapped_ptr& val)
		{
			if (val.valid())
			{
				assign(val.getref());
			}
			else
			{
				reset();
			}
		}

		void assign(mapped_ptr&& val)
		{
			if (val.valid())
			{
				assign(std::forward<value_type>(val.getref()));
			}
			else
			{
				reset();
			}
		}

		void assign(const mapped_ptr* val)
		{
			if (val)
			{
				assign(*val);
			}
			else
			{
				reset();
			}
		}

	private:

		value_type* alloc()
		{
			assert(empty());
			
			_pointer = getal().allocate(valuesize());//boost::interprocess::offset_ptr
			auto ptr = getptr();
			new (ptr) value_type(_al);
			return ptr;
		}

		void dealloc()
		{
			assert(valid());
			getal().deallocate(getpointer(), valuesize());
			_pointer = pointer();
		}

		alloc_type& getal()
		{
			return _al;
		}
		const alloc_type& getal() const
		{
			return _al;
		}

		size_t valuesize() const
		{
			return sizeof(value_type);
		}
		value_type* getptr() const
		{
			return getpointer().get();
		}

		value_type& getref()
		{
			return *getptr();
		}
		const value_type& getref() const
		{
			return *getptr();
		}

		pointer& getpointer()
		{
			return _pointer;
		}

		const pointer& getpointer() const
		{
			return _pointer;
		}

		alloc_type _al;
		pointer _pointer;
	};

}}


namespace fc {

	namespace raw {
		// mapped_ptr
		template<typename Stream, typename T>
		void pack(Stream& s, Xmaxplatform::Chain::mapped_ptr<T>& v) {
			fc::raw::pack(s, bool(!!v));
			if (!!v) fc::raw::pack(s, *v);
		}

		template<typename Stream, typename T>
		void unpack(Stream& s, Xmaxplatform::Chain::mapped_ptr<T>& v)
		{
			try {
				bool b; fc::raw::unpack(s, b);
				if (b) { v = T(); fc::raw::unpack(s, *v); }
			} FC_RETHROW_EXCEPTIONS(warn, "optional<${type}>", ("type", fc::get_typename<T>::name()))
		}
	}

}