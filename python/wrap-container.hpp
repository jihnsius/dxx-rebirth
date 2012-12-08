#pragma once
#include <iterator>
#include <boost/python/iterator.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/scope.hpp>
#include "setattr.hpp"

using boost::python::scope;

#define DEFINE_COMMON_CONTAINER_EXPORTS(N)	\
	DEFINE_COMMON_OBJECT_EXPORTS(N)	\
	define_common_container_exports<N##_container>(scope_dxx, #N "_container_base", #N "_container", #N "s")

template <typename CT, class X1, class X2, class X3>
static void define_specific_container_exports(scope&, const char *, boost::python::class_<CT, X1, X2, X3>&) {}

template <typename CT>
static void define_common_container_exports(scope& s, const char *N_container_base, const char *N_container, const char *Ns)
{
	using namespace boost::python;
	class_<typename CT::base_container>(N_container_base, no_init);
	class_<CT, bases<typename CT::base_container>> c(N_container, no_init);
	c
		.def("__setattr__", &refuse_setattr<CT>)
		.def("__getitem__", &CT::container_getitem, return_internal_reference<>())
		.def("__iter__", range<return_internal_reference<>>(&CT::begin, &CT::end))
		.def("__len__", &CT::size);
	define_specific_container_exports(s, Ns, c);
	setattr(s, Ns, CT());
}

template <typename iterator>
static typename iterator::reference getitem(const iterator& ib, const iterator& ie, const char *const errmsg, const size_t idx)
{
	iterator ii = ib;
	for (size_t i = idx; ii != ie; --i, ++ii)
	{
		if (!i)
			return *ii;
	}
	PyErr_SetString(PyExc_IndexError, errmsg);
	boost::python::throw_error_already_set();
	// Unreached
	return *ii;
}

template <typename D, typename C, typename Derived, bool filter>
struct wrap_container_tmpl
{
	typedef wrap_container_tmpl<D, C, Derived, filter> base_container;
	typedef const D& const_reference;
	typedef C container_helper;
	typedef typename C::value_type T;
	typedef size_t size_type;
	static const char s_index_range_error[];
	/*
	 * If the results are filtered, then allow only a
	 * bidirectional_iterator_tag, since random access could misbehave
	 * in the presence of filtered elements.  If not filtering, then use
	 * random_access_iterator_tag for better performance.
	 */
	template <typename const_iterator, typename tag>
	struct const_bidirectional_iterator_tmpl : public std::iterator<tag, const D>
	{
		typedef const D& reference;
		const T *p;
		const_bidirectional_iterator_tmpl(const T *const _) : p(_) {}
		reference operator*() const
		{
			return static_cast<reference>(*p);
		}
		bool operator==(const const_iterator& that) const
		{
			return p == that.p;
		}
		bool operator!=(const const_iterator& that) const
		{
			return !(*this == that);
		}
		const_iterator& operator++()
		{
			++ p;
			return static_cast<const_iterator&>(*this);
		}
		const_iterator operator++(int)
		{
			const_iterator i(static_cast<const_iterator&>(*this));
			++ *this;
			return i;
		}
	};
	struct const_bidirectional_iterator : public const_bidirectional_iterator_tmpl<const_bidirectional_iterator, std::bidirectional_iterator_tag>
	{
		const_bidirectional_iterator(const T *const _) : const_bidirectional_iterator_tmpl<const_bidirectional_iterator, std::bidirectional_iterator_tag>(_) {}
	};
	struct const_random_iterator : public const_bidirectional_iterator_tmpl<const_random_iterator, std::random_access_iterator_tag>
	{
		const_random_iterator(const T *const _) : const_bidirectional_iterator_tmpl<const_random_iterator, std::random_access_iterator_tag>(_) {}
		const_random_iterator operator+(size_type s) const
		{
			return const_random_iterator(this->p + s);
		}
	};
	typedef typename std::conditional<filter, const_bidirectional_iterator, const_random_iterator>::type const_iterator;
	template <typename R, typename CT, CT T::*M>
		static R get_contained_value(const D& d)
		{
			return static_cast<R>(d.*M);
		}
	const_reference container_getitem(const size_t idx) const
	{
		const Derived *const self = static_cast<const Derived *>(this);
		return getitem(self->Derived::begin(), self->Derived::end(), s_index_range_error, idx);
	}
protected:
	const_iterator begin() const
	{
		return &container_helper::container()[0];
	}
	const_iterator end() const
	{
		return &container_helper::container()[0] + container_helper::array_size;
	}
	size_type size() const
	{
		const Derived *const self = static_cast<const Derived *>(this);
		return self->Derived::size();
	}
};

template <typename T, unsigned N, T (&C)[N]>
struct container_lookup_tmpl
{
	enum
	{
		array_size = N,
	};
	typedef T value_type;
	typedef T array_type[array_size];
	static const array_type& container()
	{
		return C;
	}
};
