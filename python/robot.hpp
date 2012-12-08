#pragma once
#include "filter.hpp"
#include "wrap-object.hpp"
#include "wrap-filter-container.hpp"

enum robot_type_t
{
	hulk_concussion,
	medium_lifter,	// "green guy"
	spider_energy_ball,	// with mini-bots inside
	cls1_drone,
	medium_scout,	// blue, fast
	cloaked_vulcan,
	cloaked_hulk_concussion,
	supervisor1,	// blue, no weapons
	screaming_laser,	// one leg, right side
	plasma,
	supervisor2,	// white, laser
	platform_laser,
	platform_missile,
	triangle_useless,	// no weapons, slow
	mini,	// dropped by spider
	hulk_fusion,
	hulk_homing,
	d1_boss_lvl7,
	cloaked_medium_lifter,
	vulcan,
	hulk_green_laser,
	spider_four_claw_melee,
	multi_laser,
	d1_final_boss,
	smart_mine_energy_claws,
	phoenix_smelter,
	spreadfire_spider,
	gauss,
	spider_many_small_globes,
	spreadfire_spider_squad,
	smart_mine_energy_flash_missile,
	d2_red_fatty_boss1,
	d1_boss_lvl7_not_boss,
	guide,
	plasma_body_but_homing_missile,
	kamakazie_guide_body,
	vulcan_concussion_hybrid,
	pest,
	internal_tactical_droid,
	pig,
	diamond_claw,
	snake_red,
	thief,
	mercury,
	ebandit,
	d2_megamissile_phoenix_boss,
	d2_smartmine_mercury_boss,
	boarhead,
	missile_spider,
	kamakazie_fish,
	mini_flash_missile,
	lou_guard,
	d2_mercury_boss,
	yellow_fatty_miniboss_phoenix_mercury,
	cloaked_diamond_claw,
	cloaked_phoenix_smelter,
	unarmed_fish,
	phoenix_smelter2,
	kamakazie_fish2,
	smart_mine_energy_claws2,
	missile_spider2,
	snake_green,
	d2_omega_homing_flash_boss,
	snake_green2,
	d2_earthshaker_boss,
	reactor,
};

DECLARE_DXX_OBJECT_TYPESAFE_SUBTYPE(robot);
DECLARE_DXX_OBJECT_TYPESAFE_CONTAINER(robot, OBJ_ROBOT);
