#pragma once
#include "object.h"

template <typename, typename...>
struct filter;

template <typename T>
struct filter<T>
{
	bool operator()(const T&)
	{
		return true;
	}
};

template <typename T, typename V, V T::*M, V E>
struct filter_member_equal
{
	bool operator()(const T& t)
	{
		return (t.*M == E);
	}
};

template <typename T, typename F, typename... Filters>
struct filter<T, F, Filters...> : public F, protected filter<T, Filters...>
{
	bool operator()(const T& t)
	{
		return this->F::operator()(t) && this->filter<T, Filters...>::operator()(t);
	}
};

template <object_type_t type>
struct filter_object_type : filter_member_equal<dxxobject, ubyte, &dxxobject::type, type> {};
