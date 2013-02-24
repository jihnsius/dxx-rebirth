#include "filter.hpp"
#include "wrap-filter-container.hpp"
#include "wrap-object.hpp"
#include "robot.hpp"
#include "defmodules.hpp"

using namespace boost::python;

template <>
void define_enum_values<robot_type_t>(enum_<robot_type_t>& e)
{
	e
		.value("hulk_concussion", hulk_concussion)
		.value("medium_lifter", medium_lifter)
		.value("spider_energy_ball", spider_energy_ball)
		.value("cls1_drone", cls1_drone)
		.value("medium_scout", medium_scout)
		.value("cloaked_vulcan", cloaked_vulcan)
		.value("cloaked_hulk_concussion", cloaked_hulk_concussion)
		.value("supervisor1", supervisor1)
		.value("screaming_laser", screaming_laser)
		.value("plasma", plasma)
		.value("supervisor2", supervisor2)
		.value("platform_laser", platform_laser)
		.value("platform_missile", platform_missile)
		.value("triangle_useless", triangle_useless)
		.value("mini", mini)
		.value("hulk_fusion", hulk_fusion)
		.value("hulk_homing", hulk_homing)
		.value("d1_boss_lvl7", d1_boss_lvl7)
		.value("cloaked_medium_lifter", cloaked_medium_lifter)
		.value("vulcan", vulcan)
		.value("hulk_green_laser", hulk_green_laser)
		.value("spider_four_claw_melee", spider_four_claw_melee)
		.value("multi_laser", multi_laser)
		.value("d1_final_boss", d1_final_boss)
		.value("smart_mine_energy_claws", smart_mine_energy_claws)
		.value("phoenix_smelter", phoenix_smelter)
		.value("spreadfire_spider", spreadfire_spider)
		.value("gauss", gauss)
		.value("spider_many_small_globes", spider_many_small_globes)
		.value("spreadfire_spider_squad", spreadfire_spider_squad)
		.value("smart_mine_energy_flash_missile", smart_mine_energy_flash_missile)
		.value("d2_red_fatty_boss1", d2_red_fatty_boss1)
		.value("d1_boss_lvl7_not_boss", d1_boss_lvl7_not_boss)
		.value("guide", guide)
		.value("plasma_body_but_homing_missile", plasma_body_but_homing_missile)
		.value("kamikaze_guide_body", kamikaze_guide_body)
		.value("vulcan_concussion_hybrid", vulcan_concussion_hybrid)
		.value("pest", pest)
		.value("internal_tactical_droid", internal_tactical_droid)
		.value("pig", pig)
		.value("diamond_claw", diamond_claw)
		.value("snake_red", snake_red)
		.value("thief", thief)
		.value("mercury", mercury)
		.value("ebandit", ebandit)
		.value("d2_megamissile_phoenix_boss", d2_megamissile_phoenix_boss)
		.value("d2_smartmine_mercury_boss", d2_smartmine_mercury_boss)
		.value("boarhead", boarhead)
		.value("missile_spider", missile_spider)
		.value("kamikaze_fish", kamikaze_fish)
		.value("mini_flash_missile", mini_flash_missile)
		.value("lou_guard", lou_guard)
		.value("d2_mercury_boss", d2_mercury_boss)
		.value("yellow_fatty_miniboss_phoenix_mercury", yellow_fatty_miniboss_phoenix_mercury)
		.value("cloaked_diamond_claw", cloaked_diamond_claw)
		.value("cloaked_phoenix_smelter", cloaked_phoenix_smelter)
		.value("unarmed_fish", unarmed_fish)
		.value("phoenix_smelter2", phoenix_smelter2)
		.value("kamikaze_fish2", kamikaze_fish2)
		.value("smart_mine_energy_claws2", smart_mine_energy_claws2)
		.value("missile_spider2", missile_spider2)
		.value("snake_green", snake_green)
		.value("d2_omega_homing_flash_boss", d2_omega_homing_flash_boss)
		.value("snake_green2", snake_green2)
		.value("d2_earthshaker_boss", d2_earthshaker_boss)
		.value("reactor", reactor)
		;
}

DEFINE_DXX_OBJECT_SUBTYPE_STATIC(robot);

void define_robot_module(object& __main__, scope& scope_dxx)
{
	DEFINE_COMMON_CONTAINER_EXPORTS(robot);
}
