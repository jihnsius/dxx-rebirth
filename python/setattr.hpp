#pragma once
#include <boost/python/errors.hpp>

template <typename CT>
static void refuse_setattr(CT&, const char *, boost::python::object&)
{
	PyErr_SetString(PyExc_TypeError, "class object is immutable");
	boost::python::throw_error_already_set();
}
