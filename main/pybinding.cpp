#ifdef USE_PYTHON
#include <boost/format.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/import.hpp>
#include <boost/python/scope.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <array>

#include "../python/exception.hpp"
#include "../python/wrap-object.hpp"
#include "../python/pretty.hpp"
#include "../python/script-controls.hpp"
#include "pybinding.h"
#include "kconfig.h"
#include "gr.h"
#include "args.h"

using namespace boost::python;

extern script_control_info ScriptControls;

object gpy__main__;

template <>
str pretty<vms_vector>(const vms_vector& v)
{
	return (boost::format("{x: %i, y: %i, z: %i} <dxx.vms_vector at %p>") % (v.x >> 16) % (v.y >> 16) % (v.z >> 16) % &v).str().c_str();
}

template <>
str pretty<vms_matrix>(const vms_matrix& m)
{
	return (boost::format("{r: {x: %i, y: %i, z: %i}, u: {x: %i, y: %i, z: %i}, f: {x: %i, y: %i, z: %i}} <dxx.vms_matrix at %p>") %
		(m.rvec.x) % (m.rvec.y) % (m.rvec.z) %
		(m.uvec.x) % (m.uvec.y) % (m.uvec.z) %
		(m.fvec.x) % (m.fvec.y) % (m.fvec.z) %
		&m).str().c_str();
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
void define_reactor_module(object& __main__, scope& scope_dxx);
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
	defstr(class_<vms_matrix>("vms_matrix", no_init)
		.add_property("r", &vms_matrix::rvec)
		.add_property("u", &vms_matrix::uvec)
		.add_property("f", &vms_matrix::fvec)
		);
	define_reactor_module(__main__, scope_dxx);
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

template <typename F, typename E>
static inline void guarded_py_call(const F& f, const E& e)
{
	if (GameArg.SysNoPython)
		return;
	try {
		f();
	} catch (const error_already_set &) {
		e();
		con_pyerr_print();
	}
}

template <typename F>
static inline void guarded_py_call(const F& f)
{
	guarded_py_call(f, [](){});
}

template <typename... Args>
static void py_call_maybe_null_object(object& m, const char *f, Args&&... args)
{
	object o{getattr(m, f, object())};
	if (!o.is_none())
		o(std::forward<Args>(args)...);
}

void scripting_init()
{
	if (GameArg.SysNoPython)
		return;
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
	guarded_py_call([]() {
		py_call_maybe_null_object(gpy__main__, "control_hook");
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
	}, [input]() {
		con_printf(CON_URGENT, "!: %s\n", input);
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
	if (!ScriptControls.ship_orientation.enable_position)
		return;
	vms_vector tv;
	const vms_vector& sop = ScriptControls.ship_orientation.pos;
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

static int djverbose;

static int16_t get_desired_segment(int16_t& cached_segnum, const script_control_info::location& loc)
{
	if (loc.enable_segment)
	{
		const int requested_segment = loc.segment;
		if (static_cast<unsigned>(requested_segment) <= static_cast<unsigned>(Highest_segment_index))
			return requested_segment;
		return -1;
	}
	const int cseg = cached_segnum;
	const int segnum = find_point_seg(&loc.pos, (static_cast<unsigned>(cseg) > static_cast<unsigned>(Highest_segment_index) ? -1 : cseg));
	cached_segnum = segnum;
	return segnum;
}

static vms_vector get_player_thrust(const dxxobject& plr)
{
	static int16_t s_target_segnum = -1;
	const int16_t segnum = get_desired_segment(s_target_segnum, ScriptControls.ship_destination);
	vms_vector vs;
	if (segnum == -1 || static_cast<unsigned>(segnum) > static_cast<unsigned>(Highest_segment_index))
	{
		vm_vec_zero(&vs);
		return vs;
	}
	if (segnum == plr.segnum)
	{
		if (!ScriptControls.ship_destination.enable_position)
		{
			vm_vec_zero(&vs);
			return vs;
		}
		/*
		 * TODO: Do not assume that the path is clear just because the
		 * path does not cross segments.  A player or robot could be in
		 * the path.
		 */
		vm_vec_sub(&vs, &ScriptControls.ship_destination.pos, &plr.pos);
		return vs;
	}
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int> > Graph;
	typedef Graph::vertex_descriptor vertex_descriptor;
	Graph g(Highest_segment_index + 1);
	boost::property_map<Graph, boost::edge_weight_t>::type wmap = get(boost::edge_weight, g);
	unsigned i = 0;
	const unsigned hsi = static_cast<unsigned>(Highest_segment_index);
	for (; i <= hsi; ++i)
	{
		const segment& s = Segments[i];
		for (unsigned j = 0; j != MAX_SIDES_PER_SEGMENT; ++j)
		{
			const int ichild = s.children[j];
			if (ichild == -1 || ichild == -2)
				continue;
			if (ichild > Highest_segment_index)
				continue;
			// TODO: account for walls, doors, etc.
			const std::pair<Graph::edge_descriptor, bool> r = add_edge(i, ichild, g);
			vms_vector s1;
			compute_segment_center(&s1, &Segments[i]);
			vms_vector s2;
			compute_segment_center(&s2, &Segments[ichild]);
			vms_vector v;
			vm_vec_sub(&v, &s1, &s2);
			wmap[r.first] = vm_vec_mag(&v);
		}
	}
	const unsigned vertexes = num_vertices(g);
	std::vector<vertex_descriptor> vpred(vertexes);
	std::vector<int> vdist(vertexes);
	const vertex_descriptor vd_segnum = vertex(plr.segnum, g);
	boost::dijkstra_shortest_paths(g, vd_segnum, &vpred[0], &vdist[0], get(boost::edge_weight, g), get(boost::vertex_index, g), std::less<int>(), boost::closed_plus<int>(), std::numeric_limits<int>::max(), 0, boost::default_dijkstra_visitor());
	unsigned steps = 0;
	unsigned idx_predecessor = segnum;
	const int verbose = djverbose;
	djverbose = 0;
	if (idx_predecessor >= vpred.size())
	{
		vm_vec_zero(&vs);
		if (verbose)
			con_printf(CON_NORMAL, "idx_predecessor=%u vpred.size()=%zu\n", idx_predecessor, vpred.size());
		return vs;
	}
	for (;;)
	{
		const unsigned n = vpred[idx_predecessor];
		if (verbose)
		{
			con_printf(CON_NORMAL, "vpred[%u]=%u\n", idx_predecessor, n);
		}
		if (n == static_cast<unsigned>(plr.segnum))
		{
			/*
			 * Found idx_predecessor to be an immediate successor of segnum.
			 */
			vms_vector segcen;
			compute_segment_center(&segcen, &Segments[idx_predecessor]);
			vm_vec_sub(&vs, &segcen, &plr.pos);
			return vs;
		}
		if (n == idx_predecessor)
		{
			vm_vec_zero(&vs);
			if (verbose)
				con_printf(CON_NORMAL, "n=%u idx_predecessor=%u\n", n, idx_predecessor);
			return vs;
		}
		if (n >= vpred.size())
		{
			vm_vec_zero(&vs);
			if (verbose)
				con_printf(CON_NORMAL, "n=%u vpred.size()=%zu\n", n, vpred.size());
			return vs;
		}
		idx_predecessor = n;
		if (++ steps > 16000)
		{
			vm_vec_zero(&vs);
			if (verbose)
				con_printf(CON_NORMAL, "steps=%u idx_predecessor=%u\n", steps, idx_predecessor);
			return vs;
		}
	}
}

unsigned cxx_script_get_player_ship_vecthrust(dxxobject *const obj, const fix script_max_forward_thrust)
{
	if (!ScriptControls.ship_destination.enable_position && !ScriptControls.ship_destination.enable_segment)
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
	const vms_vector tempv = get_player_thrust(*obj);
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


extern "C" unsigned py_get_glow_point(g3s_point (*)[3]);

unsigned py_get_glow_point(g3s_point (*const gp)[3])
{
	if (!ScriptControls.glow_destination.enable_position && !ScriptControls.glow_destination.enable_segment)
		return 0;
	g3s_point (&p)[3] = *gp;
	static int16_t s_target_segnum = -1;
	const int16_t segnum = get_desired_segment(s_target_segnum, ScriptControls.glow_destination);
	if (segnum == -1 || static_cast<unsigned>(segnum) > static_cast<unsigned>(Highest_segment_index))
		return 0;
	if (!Viewer)
		return 0;
	const dxxobject& objplayer = *Viewer;
	if (segnum == objplayer.segnum)
		return 0;
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int> > Graph;
	typedef Graph::vertex_descriptor vertex_descriptor;
	Graph g(Highest_segment_index + 1);
	boost::property_map<Graph, boost::edge_weight_t>::type wmap = get(boost::edge_weight, g);
	unsigned i = 0;
	const unsigned hsi = static_cast<unsigned>(Highest_segment_index);
	for (; i <= hsi; ++i)
	{
		const segment& s = Segments[i];
		for (unsigned j = 0; j != MAX_SIDES_PER_SEGMENT; ++j)
		{
			const int ichild = s.children[j];
			if (ichild == -1 || ichild == -2)
				continue;
			if (ichild > Highest_segment_index)
				continue;
			// TODO: account for walls, doors, etc.
			const std::pair<Graph::edge_descriptor, bool> r = add_edge(i, ichild, g);
			vms_vector s1;
			compute_segment_center(&s1, &Segments[i]);
			vms_vector s2;
			compute_segment_center(&s2, &Segments[ichild]);
			vms_vector v;
			vm_vec_sub(&v, &s1, &s2);
			wmap[r.first] = vm_vec_mag(&v);
		}
	}
	const unsigned vertexes = num_vertices(g);
	std::vector<vertex_descriptor> vpred(vertexes);
	std::vector<int> vdist(vertexes);
	const vertex_descriptor vd_segnum = vertex(objplayer.segnum, g);
	boost::dijkstra_shortest_paths(g, vd_segnum, &vpred[0], &vdist[0], get(boost::edge_weight, g), get(boost::vertex_index, g), std::less<int>(), boost::closed_plus<int>(), std::numeric_limits<int>::max(), 0, boost::default_dijkstra_visitor());
	typedef std::array<unsigned, sizeof(p) / sizeof(*p)> predecessor_circular_buffer;
	predecessor_circular_buffer idx_predecessor;
	idx_predecessor.fill(~0u);
	const predecessor_circular_buffer::iterator ib_predecessor = idx_predecessor.begin();
	const predecessor_circular_buffer::iterator ie_predecessor = idx_predecessor.end();
	predecessor_circular_buffer::iterator ii_predecessor = ib_predecessor;
	*ii_predecessor = segnum;
	if (idx_predecessor.front() >= vpred.size())
		return 0;
	unsigned steps = 0;
	unsigned lookup_predecessor = segnum;
	unsigned skipped = 0;
	for (;;)
	{
		const unsigned n = vpred[lookup_predecessor];
		if (n == static_cast<unsigned>(objplayer.segnum))
		{
			/*
			 * Found idx_predecessor to be an immediate successor of segnum.
			 */
			vms_vector segcen;
			unsigned count = 0;
			for (; count < (sizeof(p) / sizeof(*p)); ++count)
			{
				const unsigned i = *ii_predecessor;
				if (i >= vertexes)
					break;
				if (ii_predecessor == ib_predecessor)
					ii_predecessor = ie_predecessor;
				-- ii_predecessor;
				compute_segment_center(&segcen, &Segments[i]);
				g3_rotate_point(&p[count], &segcen);
			}
			return count;
		}
		if (n == lookup_predecessor)
			return 0;
		if (n >= vpred.size())
			return 0;
		if (skipped || static_cast<unsigned>(Segments[n].children[WBACK]) != lookup_predecessor)
		{
			++ ii_predecessor;
			if (ii_predecessor == ie_predecessor)
				ii_predecessor = ib_predecessor;
			*ii_predecessor = n;
			skipped = 0;
		}
		else
			skipped = 1;
		lookup_predecessor = n;
		if (++ steps > 16000)
			return 0;
	}
}
#endif
