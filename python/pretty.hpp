#pragma once
#include <boost/python/class.hpp>
#include <boost/python/str.hpp>

using boost::python::class_;
using boost::python::str;

template <typename T>
str pretty(const T&);

template <typename T, class X1, class X2, class X3>
class_<T, X1, X2, X3>& defstr(class_<T, X1, X2, X3>& c)
{
	return c.def("__str__", &pretty<T>);
}
