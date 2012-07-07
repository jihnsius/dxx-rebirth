#include "filter.hpp"
#include "wrap-filter-container.hpp"
#include "reactor.hpp"

using namespace boost::python;

template <>
void define_enum_values<reactor_type_t>(enum_<reactor_type_t>&)
{
}

DEFINE_DXX_OBJECT_SUBTYPE_STATIC(reactor);

void define_reactor_module(object& __main__, scope& scope_dxx)
{
	DEFINE_COMMON_CONTAINER_EXPORTS(reactor);
}
