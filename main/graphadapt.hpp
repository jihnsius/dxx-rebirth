#pragma once
#include "tmplgraphadapt.hpp"
#include "segment.h"
#include <array>

struct dxx_segment_adaptor : tmpl_d2segment_adaptor<dxx_segment_adaptor>
{
	struct weight_map;
	bool has_usable_edge(const vertex_descriptor&, const unsigned) const
	{
		return true;
	}
	dxx_segment_adaptor(const weight_map& wmap) :
		tmpl_d2segment_adaptor<dxx_segment_adaptor>(Num_segments << 3),
		m_wmap(&wmap)
	{
	}
	~dxx_segment_adaptor()
	{
#if 0
		const auto a = [](const std::pair<edge_descriptor, unsigned>& p) {
			std::printf("weight: (%u,%u,%u) = %u\n", (unsigned) p.first.extract_segment(), (unsigned) p.first.extract_srcside(), (unsigned) p.first.extract_dstside(), (unsigned) p.second);
		};
		std::for_each(weightlook.begin(), weightlook.end(), a);
#endif
	}
	inline weight_adaptor::value_type get_weight(const edge_descriptor& e) const;
	const weight_map *get_wmap() const
	{
		return m_wmap;
	}
protected:
	const weight_map *m_wmap;
};

struct dxx_segment_adaptor::weight_map
{
	struct per_dstside_t
	{
		weight_adaptor::value_type m_dist;
	};
	struct per_srcside_t
	{
		std::array<per_dstside_t, e_max_sides_per_segment> dstside;
		vms_vector center;
	};
	struct per_segment_t
	{
		std::array<per_srcside_t, e_max_sides_per_segment> srcside;
	};
	typedef std::vector<per_segment_t> all_segments_t;
protected:
	all_segments_t m_wmap;
	weight_adaptor::value_type compute_weight(const segment_descriptor& seg, per_segment_t& ps, const side_descriptor& srcside, per_srcside_t& pss, const side_descriptor& dstside, per_dstside_t& pds)
	{
		(void)seg;
		(void)pds;
		if (srcside == dstside)
			/*
			 * Special case: connection to next segment.
			 */
			return weight_adaptor::value_type(0);
		assert(&ps.srcside[srcside] == &pss);
		const vms_vector& s1 = pss.center;
		const vms_vector& s2 = ps.srcside[dstside].center;
		vms_vector v;
		vm_vec_sub(&v, &s1, &s2);
		return weight_adaptor::value_type(vm_vec_mag(&v));
	}
	void compute_dstside(const segment_descriptor& seg, per_segment_t& ps, const side_descriptor& srcside, per_srcside_t& pss, const side_descriptor& dstside, per_dstside_t& pds)
	{
		pds.m_dist = compute_weight(seg, ps, srcside, pss, dstside, pds);
	}
	void compute_srcside(const segment_descriptor& seg, per_segment_t& ps, const side_descriptor& srcside, per_srcside_t& pss)
	{
		(void)ps;
		compute_center_point_on_side(&pss.center, &Segments[seg], srcside);
	}
	void compute_srcside2(const segment_descriptor& seg, per_segment_t& ps, const side_descriptor& srcside, per_srcside_t& pss)
	{
		const side_descriptor numsides(e_max_sides_per_segment);
		for (side_descriptor dstside(0); dstside != numsides; ++dstside)
			compute_dstside(seg, ps, srcside, pss, dstside, pss.dstside[dstside]);
	}
	void compute_segment(const segment_descriptor& seg, per_segment_t& ps)
	{
		const side_descriptor numsides(e_max_sides_per_segment);
		for (side_descriptor srcside(0); srcside != numsides; ++srcside)
			compute_srcside(seg, ps, srcside, ps.srcside[srcside]);
		for (side_descriptor srcside(0); srcside != numsides; ++srcside)
			compute_srcside2(seg, ps, srcside, ps.srcside[srcside]);
	}
public:
	weight_map() :
		m_wmap(Num_segments)
	{
		for (segment_descriptor seg(segment_first); seg < Num_segments; ++seg)
			compute_segment(seg, m_wmap[seg]);
	}
	const per_srcside_t& get_srcside(const per_segment_t& seg, const side_descriptor& srcside) const
	{
		return seg.srcside.at(srcside);
	}
	const per_srcside_t& get_srcside(const segment_descriptor& srcseg, const side_descriptor& srcside) const
	{
		return get_segment(srcseg).srcside.at(srcside);
	}
	const per_segment_t& get_segment(const segment_descriptor& srcseg) const
	{
		return m_wmap.at(srcseg);
	}
	weight_adaptor::value_type get_weight(const edge_descriptor& e) const
	{
		const per_segment_t& srcseg = get_segment(e.extract_segment());
		const per_srcside_t& srcside = get_srcside(srcseg, e.extract_srcside());
		const per_dstside_t& dstside = srcside.dstside.at(e.extract_dstside());
		return dstside.m_dist;
	}
};

weight_adaptor::value_type dxx_segment_adaptor::get_weight(const edge_descriptor& e) const
{
	return m_wmap->get_weight(e);
}

static inline typename boost::graph_traits<dxx_segment_adaptor>::vertices_size_type num_vertices(const dxx_segment_adaptor&)
{
	return Num_segments << 3;
}

static inline typename boost::graph_traits<dxx_segment_adaptor>::vertex_descriptor target(const typename boost::graph_traits<dxx_segment_adaptor>::edge_descriptor e, const dxx_segment_adaptor&)
{
	typedef dxx_segment_adaptor::segment_descriptor segment_descriptor;
	typedef dxx_segment_adaptor::side_descriptor side_descriptor;
	typedef dxx_segment_adaptor::vertex_descriptor vertex_descriptor;
	const segment_descriptor& srcseg = e.extract_segment();
	const side_descriptor& srcside = e.extract_srcside();
	const side_descriptor& dstside = e.extract_dstside();
	if (srcside != dstside)
		return vertex_descriptor(srcseg, dstside);
	const segnum_t child = Segments[srcseg].children[dstside];
	if ((static_cast<unsigned>(child) >= static_cast<unsigned>(Num_segments)))
		/* BUG */
		return vertex_descriptor(srcseg, dstside);
	const int fcs = find_connect_side(&Segments[srcseg], &Segments[child]);
	if (static_cast<unsigned>(fcs) >= e_max_sides_per_segment)
		/* BUG */
		return vertex_descriptor(srcseg, dstside);
	return vertex_descriptor(segment_descriptor(child), side_descriptor(fcs));
}

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <stdint.h>

static inline weight_adaptor::value_type get(const weight_adaptor&, const dxx_segment_adaptor::edge_descriptor& e)
{
	return e.graph->get_weight(e);
}

dxx_segment_adaptor::degree_size_type out_degree(const dxx_segment_adaptor::vertex_descriptor&, const dxx_segment_adaptor&);

#if 0
boost::two_bit_color_map<boost::typed_identity_property_map<long unsigned int> >::value_type get(boost::two_bit_color_map<boost::typed_identity_property_map<long unsigned int> >&, dxx_segment_adaptor::vertex_descriptor&);
int get(dxx_segment_adaptor::DistanceMap::pointer p, const dxx_segment_adaptor::vertex_descriptor&);
int ain(int, char **)
{
	typedef dxx_segment_adaptor Graph;
    BOOST_CONCEPT_ASSERT(( boost::VertexListGraphConcept<Graph> ));
	dxx_segment_adaptor g;
	boost::dijkstra_shortest_paths(g, dxx_segment_adaptor::vertex_descriptor(0), boost::predecessor_map(&g.m_pred[0]).distance_map(&g.m_dist[0]));
	return 0;
}
#endif
