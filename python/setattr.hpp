#pragma once
#include <boost/python/class.hpp>
#include <boost/python/errors.hpp>

template <typename CT>
static void refuse_delattr(CT&, const char *)
{
	PyErr_SetString(PyExc_TypeError, "class object is immutable");
	boost::python::throw_error_already_set();
}

template <typename CT>
static void refuse_setattr(CT&, const char *, boost::python::object&)
{
	PyErr_SetString(PyExc_TypeError, "class object is immutable");
	boost::python::throw_error_already_set();
}

template <typename T, class X1, class X2, class X3>
static boost::python::class_<T, X1, X2, X3>& freeze_attributes(boost::python::class_<T, X1, X2, X3>& c)
{
	return c
		.def("__delattr__", &refuse_delattr<T>)
		.def("__setattr__", &refuse_setattr<T>)
		;
}
