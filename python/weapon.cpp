#include "filter.hpp"
#include "wrap-filter-container.hpp"
#include "wrap-object.hpp"
#include "weapon.h"
#include "laser.h"

using namespace boost::python;

template <>
void define_enum_values<weapon_type_t>(enum_<weapon_type_t>&)
{
}

DEFINE_DXX_OBJECT_TYPESAFE_SUBTYPE(weapon, OBJ_WEAPON);

void define_weapon_module(object& __main__, scope& scope_dxx)
{
	DEFINE_COMMON_CONTAINER_EXPORTS(weapon);
}
