#include "filter.hpp"
#include "wrap-filter-container.hpp"
#include "wrap-object.hpp"
#include "robot.hpp"

using namespace boost::python;

template <>
void define_enum_values<robot_type_t>(enum_<robot_type_t>&)
{
}

DEFINE_DXX_OBJECT_SUBTYPE_STATIC(robot);

void define_robot_module(object& __main__, scope& scope_dxx)
{
	DEFINE_COMMON_CONTAINER_EXPORTS(robot);
}
