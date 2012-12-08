#ifdef USE_PYTHON
#include <boost/format.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/exec.hpp>
#include <boost/python/import.hpp>
#include <boost/python/scope.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <array>

#include "../python/defmodules.hpp"
#include "../python/exception.hpp"
#include "../python/wrap-object.hpp"
#include "../python/pretty.hpp"
#include "../python/script-controls.hpp"
#include "pybinding.h"
#include "kconfig.h"
#include "gr.h"
#include "args.h"
#include "vecmat.hpp"

#include "graphadapt.hpp"

using namespace boost::python;

extern script_control_info ScriptControls;
int py_get_glow_path(g3s_point (*)[3], unsigned);

boost::scoped_ptr<dxx_segment_adaptor::weight_map> g_wmap;

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
	define_segment_module(__main__, scope_dxx);
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

void scripting_close()
{
	guarded_py_call([]() {
		g_wmap.reset();
		gpy__main__ = object();
	});
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

struct pathfinder_types_t
{
	typedef dxx_segment_adaptor Graph;
	typedef Graph::segment_descriptor segment_descriptor;
	typedef Graph::vertex_descriptor vertex_descriptor;
	typedef Graph::weight_map weight_map;
};

class pathfinder_t : public pathfinder_types_t
{
public:
	enum
	{
		dsp_ok,
		dsp_invalid_viewer,
		dsp_at_destination,
		dsp_max = dsp_at_destination,
	};
	enum
	{
		v_ok,
		v_invalid_predecessor,
		v_self_predecessor,
		v_excessive_steps,
	};
	pathfinder_t() :
		g(get_create_weight_map())
	{
	}
	const Graph& graph()
	{
		return g;
	}
	int dijkstra_shortest_paths(const dxxobject *viewer, const segnum_t segnum)
	{
		if (!viewer)
			return -dsp_invalid_viewer;
		if (segnum == viewer->segnum)
			return -dsp_at_destination;
		const vertex_descriptor vd_segnum = vertex_descriptor(Graph::segment_descriptor(viewer->segnum), Graph::side_descriptor(0));
		boost::dijkstra_shortest_paths(g, vd_segnum, boost::predecessor_map(&g.m_pred[0]).distance_map(&g.m_dist[0]));
		return dsp_ok;
	}
	template <typename T>
		int visit(const vertex_descriptor& vd_start, T& t)
		{
			vertex_descriptor lookup_predecessor = vd_start;
			t.visit_first_vertex(lookup_predecessor);
			const Graph::PredecessorMap& pred = g.m_pred;
			unsigned steps = 0;
			for (;;)
			{
				const unsigned ulp = static_cast<unsigned>(lookup_predecessor);
				if (ulp >= pred.size())
					return -v_invalid_predecessor;
				const vertex_descriptor& n = pred[ulp];
				if (t.at_destination(n, lookup_predecessor, g))
					return t.finalize(n, lookup_predecessor, g);
				if (n == lookup_predecessor)
					return -v_self_predecessor;
				t.visit_step_vertex(n, lookup_predecessor);
				if (++ steps > MAX_SEGMENTS * 36)
					return -v_excessive_steps;
				lookup_predecessor = n;
			}
		}
	struct null_vertex_visitor_t
	{
		void visit_first_vertex(const vertex_descriptor&) {}
		void visit_step_vertex(const vertex_descriptor&, const vertex_descriptor&) {}
		bool at_destination(const vertex_descriptor&, const vertex_descriptor&, const Graph&) { return true; }
		int finalize(const vertex_descriptor&, const vertex_descriptor&, const Graph&) { return 0; }
	};
	template <unsigned N>
	struct nearest_vertex_visitor_t : pathfinder_types_t
	{
		enum
		{
			vertex_count = N,
		};
		typedef std::array<vertex_descriptor, N> predecessor_circular_buffer;
		predecessor_circular_buffer idx_predecessor;
		typename predecessor_circular_buffer::iterator ii_predecessor;
		const segment_descriptor sd_destination;
		nearest_vertex_visitor_t(const segment_descriptor& destination) :
			ii_predecessor(idx_predecessor.begin()),
			sd_destination(destination)
		{
			idx_predecessor.fill(typename predecessor_circular_buffer::value_type(~0u));
		}
		void visit_first_vertex(const vertex_descriptor& vd)
		{
			*ii_predecessor = vd;
			++ ii_predecessor;
		}
		void visit_step_vertex(const vertex_descriptor& vd, const vertex_descriptor&)
		{
			*ii_predecessor = vd;
			++ ii_predecessor;
			if (ii_predecessor == idx_predecessor.end())
				ii_predecessor = idx_predecessor.begin();
		}
		bool at_destination(const vertex_descriptor& vd, const vertex_descriptor&, const Graph&)
		{
			return vd.extract_segment() == sd_destination;
		}
	};
private:
	Graph g;
	static weight_map& get_create_weight_map()
	{
		if (!g_wmap.get())
			g_wmap.reset(new weight_map);
		return *g_wmap.get();
	}
};

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
	const vms_vector& sop = ScriptControls.ship_orientation.pos;
	vms_vector tv = sop - obj->pos;
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

static segnum_t get_desired_segment(segnum_t& cached_segnum, const script_control_info::location& loc)
{
	if (loc.enable_segment)
	{
		const segnum_t requested_segment = loc.segment;
		if (requested_segment <= Highest_segment_index)
		{
			cached_segnum = requested_segment;
			return requested_segment;
		}
		return segment_none;
	}
	const segnum_t cseg = cached_segnum;
	const segnum_t segnum = find_point_seg(&loc.pos, (cseg > Highest_segment_index ? static_cast<segnum_t>(segment_none) : cseg));
	cached_segnum = segnum;
	return segnum;
}

struct thrust_nearest_vertex_visitor_t : pathfinder_t::nearest_vertex_visitor_t<2>
{
	typedef nearest_vertex_visitor_t<2> base_t;
	const dxxobject& player;
	vms_vector& vs;
	thrust_nearest_vertex_visitor_t(const segment_descriptor& destination, const dxxobject& p, vms_vector& v) :
		base_t(destination), player(p), vs(v)
	{
		vm_vec_zero(&vs);
	}
	int finalize(const vertex_descriptor&, const vertex_descriptor&, const Graph& g)
	{
		const unsigned vertexes = num_vertices(g);
		typename predecessor_circular_buffer::iterator ip = ii_predecessor;
		if (ip == idx_predecessor.begin())
			ip = idx_predecessor.end();
		-- ip;
		const vertex_descriptor& i = *ip;
		if (i >= vertexes)
			return -1;
		const weight_map& wmap = *g.get_wmap();
		const weight_map::per_srcside_t& pss = wmap.get_srcside(i.extract_segment(), i.extract_side());
		const vms_vector& segcen = pss.center;
		vs = segcen - player.pos;
		return 1;
	}
};

static vms_vector get_player_thrust(const dxxobject& plr)
{
	static segnum_t s_target_segnum = segment_none;
	const segnum_t segnum = get_desired_segment(s_target_segnum, ScriptControls.ship_destination);
	vms_vector vs;
	if (segnum == segment_none || static_cast<unsigned>(segnum) > static_cast<unsigned>(Highest_segment_index))
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
		return ScriptControls.ship_destination.pos - plr.pos;
	}
	pathfinder_t path;
	const int rcpath = path.dijkstra_shortest_paths(&plr, segnum);
	if (rcpath < 0)
	{
		vm_vec_zero(&vs);
		return vs;
	}
	typedef dxx_segment_adaptor Graph;
	typedef Graph::vertex_descriptor vertex_descriptor;
	thrust_nearest_vertex_visitor_t tnvv(Graph::segment_descriptor(plr.segnum), plr, vs);
	const int rcvisit = path.visit(vertex_descriptor(Graph::segment_descriptor(segnum), Graph::side_descriptor(0)), tnvv);
	if (rcvisit < 0)
		vm_vec_zero(&vs);
	return vs;
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

struct glow_nearest_vertex_visitor_t : pathfinder_t::nearest_vertex_visitor_t<3>
{
	typedef nearest_vertex_visitor_t<3> base_t;
	unsigned m_skipped_previous;
	std::array<vms_vector, base_t::vertex_count> m_nodes;
	glow_nearest_vertex_visitor_t(const segment_descriptor& destination) :
		base_t(destination),
		m_skipped_previous(0)
	{
	}
	void visit_step_vertex(const vertex_descriptor& vd, const vertex_descriptor& vdp)
	{
		if (skip_vertex(vd, vdp))
			return;
		base_t::visit_step_vertex(vd, vdp);
	}
	int finalize(const vertex_descriptor&, const vertex_descriptor&, const Graph& g)
	{
		const unsigned vertexes = num_vertices(g);
		unsigned count = 0;
		typename predecessor_circular_buffer::iterator ip = ii_predecessor;
		for (; count != m_nodes.size(); ++count)
		{
			if (ip == idx_predecessor.begin())
				ip = idx_predecessor.end();
			-- ip;
			const vertex_descriptor& i = *ip;
			if (i >= vertexes)
				break;
			const weight_map& wmap = *g.get_wmap();
			const weight_map::per_srcside_t& pss = wmap.get_srcside(i.extract_segment(), i.extract_side());
			const vms_vector& segcen = pss.center;
			m_nodes[count] = segcen;
		}
		return count;
	}
protected:
	bool skip_vertex(const vertex_descriptor& vd, const vertex_descriptor& vdp)
	{
		const segment_descriptor& sdvd = vd.extract_segment();
		const segment_descriptor& sdvdp = vdp.extract_segment();
		if (sdvd == sdvdp)
		{
			if (m_skipped_previous || Segments[sdvd].children[WBACK] != sdvdp)
			{
				m_skipped_previous = 0;
				return false;
			}
			++ m_skipped_previous;
			return true;
		}
		else
			return true;
	}
};

struct glow_path_cache_t
{
	segnum_t dstseg, srcseg;
	uint16_t count_nodes;
	std::array<vms_vector, glow_nearest_vertex_visitor_t::vertex_count> nodes;
};

static int py_rotate_glow_path(g3s_point (&gp)[3], const glow_path_cache_t& cache)
{
	const unsigned count_nodes = cache.count_nodes;
	for (unsigned u = 0; u != count_nodes && u != cache.nodes.size(); ++u)
		g3_rotate_point(&gp[u], &cache.nodes[u]);
	return count_nodes;
}

static int py_get_internal_glow_path(const script_control_info::location& l, const unsigned window_num, g3s_point (&gp)[3])
{
	static glow_path_cache_t s_cache[MAX_RENDERED_WINDOWS] = {
#define BOOST_PP_LOCAL_LIMITS (0,MAX_RENDERED_WINDOWS-1)
#define BOOST_PP_LOCAL_MACRO(N)	{segment_none, segment_none, 0, {{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}}},
#include BOOST_PP_LOCAL_ITERATE()
	};
	if (!l.enable_position && !l.enable_segment)
		return -1;
	glow_path_cache_t& cache = s_cache[window_num];
	const segnum_t cseg = cache.dstseg;
	const segnum_t segnum = get_desired_segment(cache.dstseg, l);
	if (segnum == segment_none || static_cast<unsigned>(segnum) > static_cast<unsigned>(Highest_segment_index))
	{
		cache.srcseg = segment_none;
		return -2;
	}
	const dxxobject *const viewer = Viewer;
	if (!viewer)
	{
		cache.srcseg = segment_none;
		return -3;
	}
	if (cseg == segnum && viewer->segnum == cache.srcseg)
		return py_rotate_glow_path(gp, cache);
	pathfinder_t path;
	const int rcpath = path.dijkstra_shortest_paths(viewer, segnum);
	if (rcpath < 0)
	{
		cache.srcseg = segment_none;
		return rcpath - 2;
	}
	typedef dxx_segment_adaptor Graph;
	typedef Graph::vertex_descriptor vertex_descriptor;
	glow_nearest_vertex_visitor_t rnvv(Graph::segment_descriptor(viewer->segnum));
	const int rcvisit = path.visit(vertex_descriptor(Graph::segment_descriptor(segnum), Graph::side_descriptor(0)), rnvv);
	if (rcvisit < 0)
	{
		cache.srcseg = segment_none;
		return rcvisit - 10;
	}
	cache.srcseg = viewer->segnum;
	cache.count_nodes = rcvisit;
	cache.nodes = rnvv.m_nodes;
	return py_rotate_glow_path(gp, cache);
}

int py_get_glow_path(g3s_point (*const gp)[3], unsigned window_num)
{
	const script_control_info::location *l;
	if (window_num == 0)
		l = &ScriptControls.glow_main;
	else if (window_num - 1 < sizeof(ScriptControls.inset) / sizeof(ScriptControls.inset[0]))
		l = &ScriptControls.inset[window_num - 1].glow;
	else
		return -1;
	return py_get_internal_glow_path(*l, window_num, *gp);
}

void py_load_level_hit()
{
	guarded_py_call([](){
		g_wmap.reset();
		py_call_maybe_null_object(gpy__main__, "notify_load_level");
	});
}
#endif
