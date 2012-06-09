#pragma once
#include "wrap-container.hpp"
#include <algorithm>
#include <boost/iterator/filter_iterator.hpp>

template <typename D, typename C, typename Derived, typename F>
struct wrap_filter_container_tmpl : public wrap_container_tmpl<D, C, Derived, true>
{
	typedef typename wrap_container_tmpl<D, C, Derived, true>::base_container wrap_container;
	typedef wrap_filter_container_tmpl<D, C, Derived, F> base_container;
	typedef typename base_container::const_reference const_reference;
	typedef typename base_container::size_type size_type;
	typedef boost::filter_iterator<F, typename base_container::const_iterator> const_iterator;
	const_reference container_getitem(const size_t idx) const
	{
		return getitem(begin(), end(), base_container::s_index_range_error, idx);
	}
	const_iterator begin() const
	{
		return {wrap_container::begin(), wrap_container::end()};
	}
	const_iterator end() const
	{
		return {wrap_container::end(), wrap_container::end()};
	}
	size_type size() const
	{
		return std::distance(begin(), end());
	}
};
