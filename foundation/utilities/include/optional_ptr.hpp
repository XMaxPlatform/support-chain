/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once
#include <memory>

namespace Xmaxplatform {

	template<typename _Ty, typename _Alloc>
	class optional_ptr
	{
	public:

		using value_type = _Ty;
		using alloc_type = _Alloc;

		template<typename _Any_alloc>
		optional_ptr(_Any_alloc&& al)
			: _al(al)
		{

		}

		~optional_ptr()
		{
			reset();
		}

		bool valid() const
		{
			return getptr() != nullptr;
		}

		bool empty() const
		{
			return getptr() == nullptr;
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

		optional_ptr& operator=(std::nullptr_t)
		{
			reset();
			return *this;
		}

		optional_ptr& operator = (const optional_ptr& opt)
		{
			assign(opt);
			return *this;
		}

		optional_ptr& operator = (optional_ptr&& opt)
		{
			assign(std::forward<optional_ptr>(opt));
			return *this;
		}

		friend bool operator < (const optional_ptr a, optional_ptr b)
		{
			if (a.valid() && b.valid())
				return *a < *b;
			return a.valid() < b.valid();
		}
		friend bool operator == (const optional_ptr a, optional_ptr b)
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

		void assign(const optional_ptr& val)
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

		void assign(optional_ptr&& val)
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

		void assign(const optional_ptr* val)
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
			assert(nullptr == _ptr);
			
			auto v = getal().allocate(valuesize()).get();//boost::interprocess::offset_ptr

			_ptr = new ((value_type*)v) value_type();
			return _ptr;
		}

		void dealloc()
		{
			assert(_ptr);
			getal().deallocate(getptr(), valuesize());
			_ptr = nullptr;
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
			return _ptr;
		}

		value_type& getref()
		{
			return *_ptr;
		}
		const value_type& getref() const
		{
			return *_ptr;
		}

		alloc_type _al;
		value_type* _ptr = nullptr;
	};

}
