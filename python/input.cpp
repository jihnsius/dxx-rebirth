#include <boost/python/class.hpp>
#include <boost/python/scope.hpp>
#include "kconfig.h"
#include "script-controls.hpp"
#include "setattr.hpp"
#include "args.h"

using namespace boost::python;

script_control_info ScriptControls;

enum
{
	e_script_fire = 0x40,
};

struct tag_input {};
struct tag_user_input : public tag_input {};
struct tag_script_input : public tag_input {};

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

static void define_user_input_class(class_<tag_user_input>& ui)
{
	ui.add_static_property("pitch", &read_user_attitude_control<&control_info::pitch_time>, &write_user_attitude_control<&control_info::pitch_time>);
	ui.add_static_property("vertical_thrust", &read_user_attitude_control<&control_info::vertical_thrust_time>, &write_user_attitude_control<&control_info::vertical_thrust_time>);
	ui.add_static_property("heading", &read_user_attitude_control<&control_info::heading_time>, &write_user_attitude_control<&control_info::heading_time>);
	ui.add_static_property("sideways_thrust", &read_user_attitude_control<&control_info::sideways_thrust_time>, &write_user_attitude_control<&control_info::sideways_thrust_time>);
	ui.add_static_property("bank", &read_user_attitude_control<&control_info::bank_time>, &write_user_attitude_control<&control_info::bank_time>);
	ui.add_static_property("forward_thrust", &read_user_attitude_control<&control_info::forward_thrust_time>, &write_user_attitude_control<&control_info::forward_thrust_time>);
}

template <const script_control_info::location script_control_info::*L, fix vms_vector::*M>
static fix read_script_attitude_control()
{
	const script_control_info::location& l = ScriptControls.*L;
	return l.pos.*M;
}

template <const script_control_info::location script_control_info::*L>
static vms_vector read_script_attitude_vector()
{
	const script_control_info::location& l = ScriptControls.*L;
	return l.pos;
}

template <const script_control_info::location script_control_info::*L>
static int read_script_attitude_segment()
{
	const script_control_info::location& l = ScriptControls.*L;
	return l.segment;
}

template <script_control_info::location script_control_info::*L, fix vms_vector::*M>
static void write_script_attitude_control(const fix f)
{
	script_control_info::location& l = ScriptControls.*L;
	l.pos.*M = f;
	l.enable_position = true;
}

template <script_control_info::location script_control_info::*L>
static void write_script_attitude_vector(const vms_vector& v)
{
	script_control_info::location& l = ScriptControls.*L;
	l.pos = v;
	l.enable_position = true;
}

template <script_control_info::location script_control_info::*L>
static void write_script_attitude_segment(const int s)
{
	script_control_info::location& l = ScriptControls.*L;
	l.segment = s;
	l.enable_segment = true;
}

template <script_control_info::location script_control_info::*L>
static void clear_script_attitude()
{
	script_control_info::location& l = ScriptControls.*L;
	l.enable_position = false;
	l.enable_segment = false;
}

template <script_control_info::location script_control_info::*L, typename T>
static void define_script_input_xyz(class_<T>& so)
{
	so.add_static_property("x", &read_script_attitude_control<L, &vms_vector::x>, &write_script_attitude_control<L, &vms_vector::x>);
	so.add_static_property("y", &read_script_attitude_control<L, &vms_vector::y>, &write_script_attitude_control<L, &vms_vector::y>);
	so.add_static_property("z", &read_script_attitude_control<L, &vms_vector::z>, &write_script_attitude_control<L, &vms_vector::z>);
	so.add_static_property("v", &read_script_attitude_vector<L>, &write_script_attitude_vector<L>);
	so.add_static_property("s", &read_script_attitude_segment<L>, &write_script_attitude_segment<L>);
	so.def("clear", &clear_script_attitude<L>);
}

static void define_script_input_class(class_<tag_script_input>& si)
{
	si.def("__setattr__", &refuse_setattr<tag_script_input>);
	scope s(si);
	struct tag_script_ship_orientation : public tag_script_input {};
	class_<tag_script_ship_orientation> sso("ship_orientation");
	define_script_input_xyz<&script_control_info::ship_orientation>(sso);
	struct tag_script_guided_destination : public tag_script_input {};
	class_<tag_script_guided_destination> sgd("guided_destination");
	define_script_input_xyz<&script_control_info::guided_destination>(sgd);
	struct tag_script_ship_destination : public tag_script_input {};
	class_<tag_script_ship_destination> ssd("ship_destination");
	define_script_input_xyz<&script_control_info::ship_destination>(ssd);
	class_<tag_script_ship_destination> glow("glow_destination");
	define_script_input_xyz<&script_control_info::glow_destination>(glow);
}

void define_input_class()
{
	class_<tag_input, boost::noncopyable> i("input", no_init);
	i.add_static_property("forbid_grab", make_getter(&GameArg.DbgForbidConsoleGrab), make_setter(&GameArg.DbgForbidConsoleGrab));
	scope s(i);
	class_<tag_user_input> ui("user", no_init);
	class_<tag_script_input> si("script", no_init);
	define_input_flag_property<ubyte, e_script_fire, &control_info::fire_primary_state>(ui, si, "fire_primary");
	define_input_flag_property<ubyte, e_script_fire, &control_info::fire_secondary_state>(ui, si, "fire_secondary");
	define_user_input_class(ui);
	define_script_input_class(si);
}
