#pragma once
#include <boost/serialization/strong_typedef.hpp>
#include <boost/graph/graph_traits.hpp>
#include <vector>

#include <cstdio>

enum
{
	e_max_sides_per_segment = 6,
};

struct weight_adaptor
{
	BOOST_STRONG_TYPEDEF(unsigned, value_type);
	BOOST_STRONG_TYPEDEF(unsigned, key_type);
	BOOST_STRONG_TYPEDEF(unsigned, category);
	BOOST_STRONG_TYPEDEF(unsigned, reference);
};

struct base_d2segment_adaptor
{
private:
	typedef struct {} *unspecified_t;
	BOOST_STRONG_TYPEDEF(unsigned, boost_int_vertex_descriptor);
protected:
	struct base_edge_descriptor;
public:
	typedef boost::directed_tag directed_category;
	typedef boost::disallow_parallel_edge_tag edge_parallel_category;
	typedef unsigned vertices_size_type;
	typedef unsigned edges_size_type;
	typedef unsigned degree_size_type;
	BOOST_STRONG_TYPEDEF(unsigned, segment_descriptor);
	BOOST_STRONG_TYPEDEF(unsigned, side_descriptor);
	struct vertex_descriptor : boost_int_vertex_descriptor
	{
		vertex_descriptor() {}
		vertex_descriptor(const vertex_descriptor& v) : boost_int_vertex_descriptor(v) {}
		explicit vertex_descriptor(const unsigned u) : boost_int_vertex_descriptor(u) {}
		vertex_descriptor(const segment_descriptor& v, const side_descriptor& s) :
			boost_int_vertex_descriptor((static_cast<unsigned>(v) * 8) | (static_cast<unsigned>(s) & 7))
		{
		}
		segment_descriptor extract_segment() const
		{
			const unsigned& u = *this;
			return segment_descriptor(u >> 3);
		}
		side_descriptor extract_side() const
		{
			const unsigned& u = *this;
			return side_descriptor(u & 7);;
		}
	};
	class vertex_iterator;
	struct traversal_category :
		public boost::bidirectional_graph_tag,
		public boost::adjacency_graph_tag,
		public boost::vertex_list_graph_tag
	{
	};
	typedef std::vector<vertex_descriptor> PredecessorMap;
	typedef std::vector<int> DistanceMap;
	PredecessorMap m_pred;
	DistanceMap m_dist;
	BOOST_STRONG_TYPEDEF(unspecified_t, adjacency_iterator);
	BOOST_STRONG_TYPEDEF(unspecified_t, in_edge_iterator);
	BOOST_STRONG_TYPEDEF(unspecified_t, edge_iterator);
	base_d2segment_adaptor(const unsigned& vertexes) :
		m_pred(vertexes), m_dist(vertexes)
	{
	}
};

class base_d2segment_adaptor::vertex_iterator : public boost::iterator_facade<vertex_iterator, vertex_descriptor, boost::bidirectional_traversal_tag, const vertex_descriptor&>
{
	vertex_descriptor segnum;
public:
	vertex_iterator()
	{
	}
	vertex_iterator(const vertex_descriptor& s) :
		segnum(s)
	{
	}
	reference dereference() const
	{
		return segnum;
	}
	void increment()
	{
		++ segnum;
	}
	void decrement()
		;
#if 0
	{
		if (!segnum)
			throw segnum;
		-- segnum;
	}
#endif
	bool equal(const vertex_iterator& that) const
	{
		return dereference() == that.dereference();
	}
};

struct base_d2segment_adaptor::base_edge_descriptor
{
	vertex_descriptor src;
	side_descriptor dstside;
	static side_descriptor extract_side(const vertex_descriptor& src)
	{
		const unsigned& usrc = src;
		return side_descriptor(usrc & 7);
	}
	segment_descriptor extract_segment() const
	{
		return src.extract_segment();
	}
	side_descriptor extract_srcside() const
	{
		return src.extract_side();
	}
	side_descriptor extract_dstside() const
	{
		return dstside;
	}
	base_edge_descriptor() :
		src(vertex_descriptor(segment_descriptor(0), side_descriptor(e_max_sides_per_segment))),
		dstside(e_max_sides_per_segment)
	{
	}
	base_edge_descriptor(const vertex_descriptor& v, const side_descriptor& d) :
		src(v), dstside(d)
	{
	}
	base_edge_descriptor(const segment_descriptor& v, const side_descriptor& s, const side_descriptor& d) :
		src(v, s), dstside(d)
	{
	}
	bool operator==(const base_edge_descriptor& that) const
	{
		const unsigned srcside = extract_side(src);
		const unsigned thatsrcside = extract_side(that.src);
//		std::printf("%s:%u: (%u,%u,%u) =?= (%u,%u,%u)\n", __func__, __LINE__, (unsigned)src.extract_segment(), (unsigned)srcside, (unsigned)dstside, (unsigned)that.src.extract_segment(), (unsigned)thatsrcside, (unsigned)dstside);
		if (srcside != thatsrcside)
			return false;
		if (srcside == e_max_sides_per_segment)
			return true;
		if (src != that.src)
			return false;
		if (dstside != that.dstside)
			return false;
		return true;
	}
	bool operator!=(const base_edge_descriptor& that) const
	{
		return !(*this == that);
	}
	bool operator<(const base_edge_descriptor& that) const
	{
		if (src < that.src)
			return true;
		if (!(src < that.src))
			return false;
		return (dstside < that.dstside);
	}
};

template <typename Graph>
struct tmpl_d2segment_adaptor : base_d2segment_adaptor
{
	struct edge_descriptor;
	class out_edge_iterator;
	typedef boost::property_map<Graph, boost::edge_weight_t> WeightMap;
	typedef boost::property_map<Graph, boost::vertex_index_t> VertexIndexMap;
	tmpl_d2segment_adaptor(const unsigned& vertexes) :
		base_d2segment_adaptor(vertexes)
	{
	}
};

template <typename Graph>
struct tmpl_d2segment_adaptor<Graph>::edge_descriptor : base_d2segment_adaptor::base_edge_descriptor
{
	typedef typename Graph::vertex_descriptor vertex_descriptor;
protected:
	/*
	 * This ought to be in vertex_descriptor, but Boost.Graph requires a
	 * lot of extra support if vertex_descriptor is not a simple type
	 * like 'int'.
	 */
public:
	const Graph *graph;
	edge_descriptor() :
		graph(NULL)
	{
	}
	edge_descriptor(const Graph& g, const vertex_descriptor& v, const side_descriptor& d) :
		base_edge_descriptor(v, d), graph(&g)
	{
	}
	edge_descriptor(const Graph& g, const segment_descriptor& v, const side_descriptor& s, const side_descriptor& d) :
		base_edge_descriptor(v, s, d),
		graph(&g)
	{
	}
	bool operator==(const edge_descriptor& that) const
	{
		if (graph != that.graph)
			return false;
		const base_edge_descriptor& bthis = *this;
		const base_edge_descriptor& bthat = that;
		return bthis == bthat;
	}
	bool operator!=(const edge_descriptor& that) const
	{
		return !(*this == that);
	}
	bool operator<(const edge_descriptor& that) const
	{
		if (graph < that.graph)
			return true;
		if (!(graph < that.graph))
			return false;
		const base_edge_descriptor& bthis = *this;
		const base_edge_descriptor& bthat = that;
		return (bthis < bthat);
	}
};

template <typename Graph>
class tmpl_d2segment_adaptor<Graph>::out_edge_iterator : public boost::iterator_facade<out_edge_iterator, typename Graph::edge_descriptor, boost::bidirectional_traversal_tag, const typename Graph::edge_descriptor&>
{
	typedef typename Graph::edge_descriptor edge_descriptor;
	typedef typename Graph::vertex_descriptor vertex_descriptor;
	edge_descriptor edge;
public:
	struct singular_iterator_dereference {};
	typedef boost::iterator_facade<out_edge_iterator, typename Graph::edge_descriptor, boost::bidirectional_traversal_tag, const typename Graph::edge_descriptor&> iterator_facade;
	typedef typename iterator_facade::reference reference;
	out_edge_iterator()
	{
	}
	out_edge_iterator(const Graph& g, const vertex_descriptor& v, const side_descriptor& s) :
		edge(g, v, s)
	{
	}
	reference dereference() const
	{
		return edge;
	}
	void increment()
	{
//		std::printf("%s:%u: (%u,%u,%u) ++\n", __func__, __LINE__, (unsigned)edge.extract_segment(), (unsigned)edge.extract_srcside(), (unsigned)edge.dstside);
		if (edge.dstside == e_max_sides_per_segment)
			throw singular_iterator_dereference();
		++ edge.dstside;
	}
	bool equal(const out_edge_iterator& that) const
	{
		return dereference() == that.dereference();
	}
};

namespace boost {

template <typename T>
struct property_map<T, typename std::enable_if<std::is_base_of<base_d2segment_adaptor, T>::value, boost::vertex_index_t>::type>
{
	typedef identity_property_map type;
	typedef type const_type;
};

template <typename T>
struct property_map<T, typename std::enable_if<std::is_base_of<base_d2segment_adaptor, T>::value, boost::edge_weight_t>::type>
{
	typedef weight_adaptor type;
	typedef type const_type;
};

}

template <typename T>
typename T::WeightMap::type get(const typename std::enable_if<std::is_base_of<base_d2segment_adaptor, T>::value, boost::edge_weight_t&>::type, const T&)
{
	return weight_adaptor();
}

template <typename T>
typename T::VertexIndexMap::type get(const typename std::enable_if<std::is_base_of<base_d2segment_adaptor, T>::value, boost::vertex_index_t&>::type, const T&)
{
	return boost::identity_property_map();
}

template <typename T>
static inline typename boost::graph_traits<T>::vertex_descriptor source(const typename std::enable_if<std::is_base_of<base_d2segment_adaptor, T>::value, typename boost::graph_traits<T>::edge_descriptor&>::type e, const T&)
{
	return e.src;
}

template <typename T>
static inline std::pair<typename boost::graph_traits<T>::out_edge_iterator, typename boost::graph_traits<T>::out_edge_iterator> out_edges(const typename std::enable_if<std::is_base_of<base_d2segment_adaptor, T>::value, typename boost::graph_traits<T>::vertex_descriptor&>::type v, const T& g)
{
	typedef typename T::out_edge_iterator out_edge_iterator;
	typedef typename T::side_descriptor side_descriptor;
	side_descriptor s(0);
	const side_descriptor numsides(e_max_sides_per_segment);
	for (; s != numsides; ++s)
		if (g.has_usable_edge(v, s))
			break;
	return std::make_pair(out_edge_iterator(g, v, side_descriptor(s)), out_edge_iterator(g, v, side_descriptor(e_max_sides_per_segment)));
}

template <typename T>
static inline std::pair<typename boost::graph_traits<T>::vertex_iterator, typename boost::graph_traits<T>::vertex_iterator> vertices(const T& g, typename std::enable_if<std::is_base_of<base_d2segment_adaptor, T>::value, int>::type = 0)
{
	typedef typename boost::graph_traits<T>::vertex_iterator vertex_iterator;
	typedef typename boost::graph_traits<T>::vertex_descriptor vertex_descriptor;
	return std::make_pair(vertex_iterator(vertex_descriptor(0)), vertex_iterator(vertex_descriptor(num_vertices(g))));
}
