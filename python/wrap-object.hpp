#pragma once
#include <boost/python/bases.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/class.hpp>
#include "wrap-container.hpp"

using boost::python::class_;
using boost::python::bases;

using boost::python::enum_;

#include "object.h"

#define DECLARE_DXX_OBJECT_TYPESAFE_SUBTYPE(N)	\
	struct dxx##N : public dxxobject			\
	{	\
		typedef N##_type_t subtype_t;			\
	}	\

#define DECLARE_DXX_OBJECT_TYPESAFE_CONTAINER(N, OBJTYPE)	\
	struct N##_container : public wrap_filter_container_tmpl<dxx##N, container_lookup_object, N##_container, filter<dxx##N, filter_object_type<OBJTYPE>>> {}

#define DEFINE_DXX_OBJECT_SUBTYPE_STATIC(N)	\
	template <>	\
	const char N##_container::wrap_container::s_index_range_error[] = "index out of range for " #N

#define DEFINE_DXX_OBJECT_TYPESAFE_SUBTYPE(N, OBJTYPE)	\
	DECLARE_DXX_OBJECT_TYPESAFE_SUBTYPE(N);	\
	DECLARE_DXX_OBJECT_TYPESAFE_CONTAINER(N, OBJTYPE);	\
	DEFINE_DXX_OBJECT_SUBTYPE_STATIC(N)

#define DEFINE_COMMON_OBJECT_EXPORTS(N)	\
	define_common_object_exports<dxx##N, N##_container>(__main__, #N, #N "_type");	\

struct container_lookup_object : public container_lookup_tmpl<dxxobject, MAX_OBJECTS, Objects> {};

template <typename E>
static void define_enum_values(enum_<E>&);

template <typename T, class X1, class X2, class X3>
static class_<T, X1, X2, X3>& define_class_properties(class_<T, X1, X2, X3>& c)
{
	return c;
}

template <typename T, typename CT>
static void define_common_object_exports(boost::python::object& __main__, const char *N, const char *N_type)
{
	using namespace boost::python;
	enum_<typename T::subtype_t> e(N_type);
	define_enum_values(e);
	class_<T, bases<dxxobject>, boost::noncopyable> c(N, no_init);
	c.add_property("subtype", &CT::template get_contained_value<typename T::subtype_t, ubyte, &T::id>);
	setattr(__main__, N, define_class_properties(c));
}
