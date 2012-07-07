#pragma once
#include "filter.hpp"
#include "wrap-object.hpp"
#include "wrap-filter-container.hpp"

enum reactor_type_t
{
};

DECLARE_DXX_OBJECT_TYPESAFE_SUBTYPE(reactor);
DECLARE_DXX_OBJECT_TYPESAFE_CONTAINER(reactor, OBJ_CNTRLCEN);
