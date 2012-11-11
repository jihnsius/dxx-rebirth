#pragma once
#include "player.h"
#include "wrap-object.hpp"
#include "wrap-filter-container.hpp"
#include "filter.hpp"

enum player_object_type_t
{
	player_0,
	player_1,
	player_2,
	player_3,
	player_4,
	player_5,
	player_6,
	player_7,
	player_8,
	player_9,
	player_10,
	player_11,
	player_12
};

DECLARE_DXX_OBJECT_TYPESAFE_SUBTYPE(player_object);
DECLARE_DXX_OBJECT_TYPESAFE_CONTAINER(player_object, OBJ_PLAYER);

const dxxplayer_object& get_player_object(const player& player);
