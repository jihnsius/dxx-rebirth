#pragma once
#include "filter.hpp"
#include "wrap-object.hpp"
#include "wrap-filter-container.hpp"

enum robot_type_t
{
};

DECLARE_DXX_OBJECT_TYPESAFE_SUBTYPE(robot);
DECLARE_DXX_OBJECT_TYPESAFE_CONTAINER(robot, OBJ_ROBOT);
