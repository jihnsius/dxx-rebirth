#ifdef USE_PYTHON
#include <boost/format.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/import.hpp>
#include <boost/python/scope.hpp>
#include "../python/exception.hpp"
#include "../python/wrap-object.hpp"
#include "../python/pretty.hpp"
#include "../python/script-controls.hpp"
#include "pybinding.h"
#include "kconfig.h"
#include "gr.h"

using namespace boost::python;

extern script_control_info ScriptControls;

object gpy__main__;

template <>
str pretty<vms_vector>(const vms_vector& v)
{
	return (boost::format("{x: %i, y: %i, z: %i} <dxx.vms_vector at %p>") % (v.x >> 16) % (v.y >> 16) % (v.z >> 16) % &v).str().c_str();
}

#if 0
namespace pretty
{

#if 0
static str dxxpowerup(const dxxpowerup& v)
{
	return (boost::format("{subtype: %s} <dxx.powerup at %p>") % extract<const char *>(str(v.id)) % &v).str().c_str();
}
#endif

}
#endif

void define_object_base_class(scope& scope_dxx);
void define_robot_module(object& __main__, scope& scope_dxx);
void define_weapon_module(object& __main__, scope& scope_dxx);
void define_player_module(object& __main__, scope& scope_dxx);
void define_powerup_module(object& __main__, scope& scope_dxx);
void define_input_class();

static void define_dxx_modules(object& __main__)
{
	load_traceback_class();
	object mod_dxx{handle<>(borrowed(PyImport_AddModule("dxx")))};
	setattr(__main__, "dxx", mod_dxx);
	scope scope_dxx(mod_dxx);
	define_object_base_class(scope_dxx);
	defstr(class_<vms_vector>("vms_vector", no_init)
		.add_property("x", &vms_vector::x)
		.add_property("y", &vms_vector::y)
		.add_property("z", &vms_vector::z)
		);
	define_robot_module(__main__, scope_dxx);
	define_weapon_module(__main__, scope_dxx);
	define_player_module(__main__, scope_dxx);
	define_powerup_module(__main__, scope_dxx);
	define_input_class();
}

static void con_pyerr_print()
{
	try {
		python_exception pe;
		pe.fetch();
		const python_exception::frame_container& frames = pe.tbframes();
		const python_exception::frame_container::const_iterator cib = frames.begin();
		const python_exception::frame_container::const_iterator cie = frames.end();
		for (python_exception::frame_container::const_iterator cii = cib; cii != cie; ++cii)
		{
			python_exception::frame_container::const_reference cr = *cii;
			con_printf(CON_URGENT, "! %s!%s:%u\n", cr.filename().c_str(), cr.function().c_str(), cr.lineno());
		}
		con_printf(CON_URGENT, "!> %s : %s\n", pe.exception_class_name().c_str(), pe.exception_description().c_str());
	} catch (const error_already_set &) {
		con_printf(CON_URGENT, "!! exception formatting exception\n");
		PyErr_Print();
	}
}

template <typename F>
static inline void guarded_py_call(const F& f)
{
	try {
		f();
	} catch (const error_already_set &) {
		con_pyerr_print();
	}
}

void scripting_init()
{
	Py_Initialize();
	try {
		object __main__(import("__main__"));
		define_dxx_modules(gpy__main__ = __main__);
		if (GameArg.SysPythonSrc)
		{
			dict nsmain{extract<dict>(getattr(__main__, "__dict__"))()};
			exec_file(GameArg.SysPythonSrc, nsmain, nsmain);
		}
	} catch (const error_already_set &) {
		con_pyerr_print();
	}
}

void cxx_script_hook_controls()
{
	ScriptControls.enable_ship_orientation = false;
	ScriptControls.enable_guided_orientation = false;
	ScriptControls.enable_destination = false;
	guarded_py_call([]() {
		object& __main__ = gpy__main__;
		object nsmain{getattr(__main__, "__dict__")};
		exec("if control_hook is not None:\n\tcontrol_hook()\n", nsmain, dict());
	});
}

static object dispatch_input(const char *const input)
{
	object& __main__ = gpy__main__;
	object nsmain{getattr(__main__, "__dict__")};
	if (input[1] == ' ')
	{
		if (input[0] == 'p')
			return eval(input + 2, nsmain, nsmain);
		if (input[0] == '.')
			return exec_file(input + 2, nsmain, nsmain);
	}
	return exec(input, nsmain, nsmain);
}

void scripting_input_enter(const char *const input)
{
	guarded_py_call([input]() {
		object e(dispatch_input(input));
		con_printf(CON_URGENT, "%s -> %s\n", e.is_none() ? "" : extract<std::string>(str(e))().c_str(), input);
	});
}

static fixang bound_angle(const fix v, const fix b)
{
	if (v > b)
		return b;
	else if (v < - b)
		return - b;
	return v;
}

static fix pick_minimal_angle(const fix rawdelta)
{
	const fix delta = rawdelta % F1_0;
	if (delta > F1_0/2)
		return delta - F1_0;
	if (delta < -F1_0/2)
		return delta + F1_0;
	return delta;
}

void cxx_script_get_guided_missile_rotang(vms_angvec *const gmav)
{
	(void)gmav;
}

void cxx_script_get_player_ship_rotthrust(dxxobject *const obj)
{
	if (!ScriptControls.enable_ship_orientation)
		return;
	vms_vector tv;
	vms_vector& sop = ScriptControls.ship_orientation_position;
	vm_vec_sub(&tv, &sop, &obj->pos);
	vm_vec_copy_normalize(&tv, &tv);
	vms_angvec dest_angles;
	vms_angvec cur_angles;
	vm_extract_angles_vector_normalized(&dest_angles, &tv);
	vm_extract_angles_vector(&cur_angles, &obj->orient.fvec);
	const fix delta_p = pick_minimal_angle(dest_angles.p - cur_angles.p);
	const fix delta_h = pick_minimal_angle(dest_angles.h - cur_angles.h);
	vms_vector *const psr = &obj->mtype.phys_info.rotthrust;
	const fix frametime = FrameTime;
	psr->x = bound_angle(delta_p, frametime / 2);
	psr->y = bound_angle(delta_h, frametime);
	psr->z = 0; // dest_angles.b; // always zero anyway
}

unsigned cxx_script_get_player_ship_vecthrust(dxxobject *const obj, const fix script_max_forward_thrust)
{
	if (!ScriptControls.enable_destination)
		return 0;
	const fix frametime = FrameTime;
	const int afterburner = (script_max_forward_thrust != frametime);
	vms_vector vec_afterburner;
	if (afterburner)
	{
		// Afterburner engaged, processing is much harder
		vm_vec_copy_scale(&vec_afterburner, &obj->orient.fvec, script_max_forward_thrust);
		return 0;
	}
	else
		vm_vec_zero(&vec_afterburner);
	vms_vector tempv;
	vm_vec_sub(&tempv, &ScriptControls.destination_position, &obj->pos);
	const fix afx = abs(tempv.x);
	const fix afy = abs(tempv.y);
	const fix afz = abs(tempv.z);
	/*
	 * In the easy case, the destination is reachable within a single
	 * frame, so we just set thrust to move directly to that location.
	 */
	if (afx <= frametime && afy <= frametime && afz <= frametime)
		obj->mtype.phys_info.thrust = tempv;
	else
	{
		/*
		 * In the hard case, setting thrust=tempv would give the ship a
		 * greater velocity than is normally legal, so we must instead
		 * set thrust to full speed, scaled to avoid going off course if
		 * two vectors are outside frametime by differing amounts.
		 */
		obj->mtype.phys_info.thrust.x = 0;
		obj->mtype.phys_info.thrust.y = 0;
		obj->mtype.phys_info.thrust.z = 0;
		const unsigned long greatest = std::max(std::max(afx, afy), afz);
		const fix sfx = static_cast<fix>((static_cast<unsigned long>(afx) * frametime) / greatest);
		const bool negx = (tempv.x < 0);
		obj->mtype.phys_info.thrust.x = (negx ? - sfx : sfx);
		const fix sfy = static_cast<fix>((static_cast<unsigned long>(afy) * frametime) / greatest);
		const bool negy = (tempv.y < 0);
		obj->mtype.phys_info.thrust.y = (negy ? - sfy : sfy);
		const fix sfz = static_cast<fix>((static_cast<unsigned long>(afz) * frametime) / greatest);
		const bool negz = (tempv.z < 0);
		obj->mtype.phys_info.thrust.z = (negz ? - sfz : sfz);
	}
	return 1;
}
#endif
