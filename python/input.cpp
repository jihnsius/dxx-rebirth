#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/scope.hpp>
#include "kconfig.h"
#include "playsave.h"
#include "gauges.h"
#include "script-controls.hpp"
#include "wrap-container.hpp"
#include "wrap-object.hpp"
#include "setattr.hpp"
#include "args.h"
#include "player.hpp"
#include "defmodules.hpp"

using namespace boost::python;

script_control_info ScriptControls;

struct container_lookup_inset_window
{
	enum
	{
		array_size = script_control_info::count_inset_windows,
	};
	typedef script_control_info::per_inset_window value_type;
	typedef script_control_info::per_inset_window_container array_type;
	static const array_type& container()
	{
		return ScriptControls.inset;
	}
};

struct inset_window_container : public wrap_container_tmpl<script_control_info::per_inset_window, container_lookup_inset_window, inset_window_container, false>
{
	typedef wrap_container_tmpl<script_control_info::per_inset_window, container_lookup_inset_window, inset_window_container, false> base_container;
	const_iterator begin() const
	{
		return base_container::begin();
	}
	const_iterator end() const
	{
		return begin() + size();
	}
	size_type size() const
	{
		return ScriptControls.inset.size();
	}
};

template <>
const char inset_window_container::base_container::s_index_range_error[] = "index out of range for inset window";

enum
{
	e_script_fire = 0x40,
};

struct tag_input {};

template <typename T, T control_info::*M, T mask = static_cast<T>(~0)>
static T read_flag_control()
{
	return (Controls.*M & mask);
}

template <typename T, T control_info::*M, T mask = static_cast<T>(~0)>
static T write_flag_control(const T value)
{
	const T unmodified_bits = (Controls.*M & static_cast<T>(~mask));
	const T modified_bits = value ? mask : 0;
	return (Controls.*M = (unmodified_bits | modified_bits));
}

template <typename T, T mask, T control_info::*M1, typename C1, typename C2>
static void define_input_flag_property(C1& c1, C2& c2, const char *const property)
{
	c1.add_static_property(property, &read_flag_control<T, M1>, &write_flag_control<T, M1>);
	c2.add_static_property(property, &read_flag_control<T, M1, mask>, &write_flag_control<T, M1, mask>);
}

template <fix control_info::*M>
static fix read_user_attitude_control()
{
	return Controls.*M;
}

template <fix control_info::*M>
static void write_user_attitude_control(const fix f)
{
	Controls.*M = f;
}

static void define_user_input_class(class_<control_info, boost::noncopyable>& ui)
{
	freeze_attributes(ui);
	ui.add_static_property("pitch", &read_user_attitude_control<&control_info::pitch_time>, &write_user_attitude_control<&control_info::pitch_time>);
	ui.add_static_property("vertical_thrust", &read_user_attitude_control<&control_info::vertical_thrust_time>, &write_user_attitude_control<&control_info::vertical_thrust_time>);
	ui.add_static_property("heading", &read_user_attitude_control<&control_info::heading_time>, &write_user_attitude_control<&control_info::heading_time>);
	ui.add_static_property("sideways_thrust", &read_user_attitude_control<&control_info::sideways_thrust_time>, &write_user_attitude_control<&control_info::sideways_thrust_time>);
	ui.add_static_property("bank", &read_user_attitude_control<&control_info::bank_time>, &write_user_attitude_control<&control_info::bank_time>);
	ui.add_static_property("forward_thrust", &read_user_attitude_control<&control_info::forward_thrust_time>, &write_user_attitude_control<&control_info::forward_thrust_time>);
}

template <fix vms_vector::*M>
static fix read_script_attitude_control(const script_control_info::location& l)
{
	return l.pos.*M;
}

template <fix vms_vector::*M>
static void write_script_attitude_control(script_control_info::location& l, const fix f)
{
	l.pos.*M = f;
	l.enable_position = true;
}

static void write_script_attitude_vector(script_control_info::location& l, const vms_vector& v)
{
	l.pos = v;
	l.enable_position = true;
}

static void write_script_attitude_segment(script_control_info::location& l, const int s)
{
	l.segment = segnum_t(s);
	l.enable_segment = true;
}

static void write_script_attitude_object_segment(script_control_info::location& l, const dxxobject& o)
{
	l.segment = o.segnum;
	l.enable_segment = true;
}

static void write_script_attitude_player_segment(script_control_info::location& l, const player& o)
{
	const dxxplayer_object& po = get_player_object(o);
	l.segment = po.segnum;
	l.enable_segment = true;
}

static void clear_script_attitude(script_control_info::location& l)
{
	l.enable_position = false;
	l.enable_segment = false;
}

template <script_control_info::location script_control_info::*L>
static void define_script_input_xyz(scope& s, const char *N)
{
	script_control_info::location& l = ScriptControls.*L;
	setattr(s, N, object(boost::ref(l)));
}

static void define_script_input_class(class_<script_control_info, boost::noncopyable>& si)
{
	class_<script_control_info::location, boost::noncopyable> cl("location", no_init);
	cl
		.add_property("s", &script_control_info::location::segment, &write_script_attitude_segment)
		.add_property("v", &script_control_info::location::pos, &write_script_attitude_vector)
		.add_property("x", &read_script_attitude_control<&vms_vector::x>, &write_script_attitude_control<&vms_vector::x>)
		.add_property("y", &read_script_attitude_control<&vms_vector::y>, &write_script_attitude_control<&vms_vector::y>)
		.add_property("z", &read_script_attitude_control<&vms_vector::z>, &write_script_attitude_control<&vms_vector::z>)
		.def("set", &write_script_attitude_object_segment)
		.def("set", &write_script_attitude_player_segment)
		.def("clear", &clear_script_attitude)
		;
	scope s(si);
	define_script_input_xyz<&script_control_info::ship_orientation>(s, "ship_orientation");
	define_script_input_xyz<&script_control_info::guided_destination>(s, "guided_destination");
	define_script_input_xyz<&script_control_info::ship_destination>(s, "ship_destination");
	freeze_attributes(si);
}

static void check_player_index_valid(unsigned ip)
{
	if (ip >= MAX_PLAYERS)
	{
		PyErr_SetString(PyExc_IndexError, "player index above maximum");
		boost::python::throw_error_already_set();
	}
}

static void check_player_index_used(unsigned ip)
{
	if (ip >= N_players)
	{
		PyErr_SetString(PyExc_IndexError, "player index not in use");
		boost::python::throw_error_already_set();
	}
}

static unsigned check_inset_window_index(unsigned which)
{
	if (which >= sizeof(PlayerCfg.Cockpit3DView) / sizeof(PlayerCfg.Cockpit3DView[0]))
	{
		PyErr_SetString(PyExc_IndexError, "invalid inset window");
		boost::python::throw_error_already_set();
	}
	return which;
}

static unsigned check_inset_window_index(script_control_info::per_inset_window& w)
{
	return check_inset_window_index(&w - &ScriptControls.inset[0]);
}

static void set_main_view_to_player_idx(unsigned ip)
{
	check_player_index_used(ip);
	const objnum_t objnum = Players[ip].objnum;
	if (objnum > Highest_object_index)
		return;
	Viewer = &Objects[objnum];
}

static void set_main_view_to_player_ref(const player& o)
{
	set_main_view_to_player_idx(&o - Players);
}

static cockpit_view_t get_view_type(script_control_info::per_inset_window& w)
{
	return PlayerCfg.Cockpit3DView[check_inset_window_index(w)];
}

static unsigned set_view_to_value(script_control_info::per_inset_window& w, cockpit_view_t value)
{
	const unsigned which = check_inset_window_index(w);
	PlayerCfg.Cockpit3DView[which] = value;
	return which;
}

static object get_view_player(script_control_info::per_inset_window& w)
{
	const unsigned which = check_inset_window_index(w);
	if (PlayerCfg.Cockpit3DView[which] != CV_COOP || Coop_view_player[which] >= N_players)
		return object();
	return object(boost::ref(Players[which]));
}

template <cockpit_view_t cv>
static void set_view_to_cockpit_view(script_control_info::per_inset_window& w)
{
	set_view_to_value(w, cv);
}

static void set_view_to_player_idx(script_control_info::per_inset_window& w, unsigned ip)
{
	check_player_index_valid(ip);
	Coop_view_player[set_view_to_value(w, CV_COOP)] = ip;
}

static void set_view_to_player_ref(script_control_info::per_inset_window& w, const player& o)
{
	set_view_to_player_idx(w, &o - Players);
}

static void define_output_class()
{
	struct tag_output {};
	scope s(class_<tag_output, boost::noncopyable>("output", no_init));
	enum_<cockpit_view_t>("view_type")
		.value("none", CV_NONE)
		.value("escort", CV_ESCORT)
		.value("rear", CV_REAR)
		.value("coop", CV_COOP)
		.value("marker", CV_MARKER)
		;
	{
		struct tag_main_window {};
		scope mw(class_<tag_main_window, boost::noncopyable>("main", no_init));
		define_script_input_xyz<&script_control_info::glow_main>(mw, "glow");
		def("set_view_player", set_main_view_to_player_ref);
		def("set_view_player", set_main_view_to_player_idx);
	}
	class_<script_control_info::per_inset_window, boost::noncopyable> piw("_per_inset_window", no_init);
	piw
		.add_property("glow", &script_control_info::per_inset_window::glow)
		.add_property("view_type", get_view_type)
		.def("get_view_player", get_view_player)
		.def("set_view_none", set_view_to_cockpit_view<CV_NONE>)
		.def("set_view_rear", set_view_to_cockpit_view<CV_REAR>)
		.def("set_view_player", set_view_to_player_ref)
		.def("set_view_player", set_view_to_player_idx)
		;
	freeze_attributes(piw);
	class_<inset_window_container> iwc("inset", no_init);
	define_common_container_exports(s, iwc, "inset");
}

void define_input_class()
{
	class_<tag_input, boost::noncopyable> i("input", no_init);
	i.add_static_property("forbid_grab", make_getter(&GameArg.DbgForbidConsoleGrab), make_setter(&GameArg.DbgForbidConsoleGrab));
	{
	scope s(i);
	class_<control_info, boost::noncopyable> ui("user", no_init);
	class_<script_control_info, boost::noncopyable> si("script", no_init);
	define_input_flag_property<ubyte, e_script_fire, &control_info::fire_primary_state>(ui, si, "fire_primary");
	define_input_flag_property<ubyte, e_script_fire, &control_info::fire_secondary_state>(ui, si, "fire_secondary");
	define_user_input_class(ui);
	define_script_input_class(si);
	}
	define_output_class();
}
