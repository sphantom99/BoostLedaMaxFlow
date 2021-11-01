#include <LEDA/graph/graph.h>
#include <LEDA/graph/max_flow.h>
#include <LEDA/graph/graph_gen.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <math.h>
#include <vector>
#include <boost/graph/bipartite.hpp>
#include <chrono>
#include <ctime>

using namespace leda;
typedef leda::graph LGraph;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, NodeInfo, EdgeInfo> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_d;
typedef boost::graph_traits<Graph>::edge_descriptor edge_d;
int ledaRandomBiMaxFlow(Graph &BG, vertex_d &Bs, vertex_d &Bt, int num_A_nodes, int num_B_nodes, int num_edges, int &n1_length)
{
	boost::property_map<Graph, int EdgeInfo::*>::type residual = get(&EdgeInfo::residual_flow, BG);
	boost::property_map<Graph, int NodeInfo::*>::type v_id = get(&NodeInfo::id, BG);
	boost::property_map<Graph, int NodeInfo::*>::type height = get(&NodeInfo::height, BG);
	boost::property_map<Graph, int NodeInfo::*>::type excess = get(&NodeInfo::excess, BG);

	srand((int)time(0));
	LGraph G;
	list<node> A, B;
	complete_bigraph(G, num_A_nodes, num_B_nodes, A, B);
	node n;
	node_array<int> ids(G);
	int id_counter = 0;

	forall_nodes(n, G)
	{
		ids[n] = id_counter;
		id_counter++;
	}

	node s = G.first_node(), t = G.last_node();
	node n2;
	bool s_t_same_set = false;
	forall(n, A)
	{
		if (n == s)
		{
			forall(n2, A)
			{
				if (n2 == t)
				{
					s_t_same_set = true;
				}
			}
		}
	}
	leda::edge e_temp;
	bool edge_exists = false;
	leda::list<node>::iterator it;
	forall(n, B)
	{
		// forall_in_edges(e_temp, n)
		// {
		// 	G.new_edge(n, source(e_temp));
		// }
		int num_of_new_edges = rand() % 2;
		for (int j = 0; j < num_of_new_edges; j++)
		{
			int random_counter = 0;
			int random_counter_boundary = rand() % A.size();
			for (it = A.begin(); random_counter < random_counter_boundary; it++)
				random_counter++;
			forall_edges(e_temp, G)
			{
				if (source(e_temp) == n && target(e_temp) == A[it]){
					edge_exists = true;
					// std::cout << "edge from sth to sth exists";
				}
			}
			if (!edge_exists)
			{
				// std::cout << "edge not found, creating";
				G.new_edge(n, A[it]);
			}
			edge_exists = false;
		}
	}
	list<leda::edge> reverse_edges;
	G.make_bidirected(reverse_edges);
	// std::cout << "created "<< reverse_edges.size() << "reverse edges \n";

	auto end = std::chrono::system_clock::now(); //declaring timers
	auto start = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	edge_array<double> cap(G);

	edge_array<double> cap1 = cap; //archive original capacities
	leda::edge e;
	double temp;
	forall_edges(e, G)
	{
		temp = rand() % 100 + 1;
		cap[e] = (double)temp;
	}
	leda::list<leda::edge>::iterator edge_it;
	forall(e, reverse_edges){
		cap[e] = 0;
	}
	if (s_t_same_set)
	{
		// std::cout << "s and t are in same set";
		int sum = 35;
		node new_s = G.new_node();
		leda::edge old_s_edges;
		// leda::forall_out_edges(old_s_edges, s) {
		// 	sum += cap[old_s_edges];
		// }
		leda::edge new_s_edge = G.new_edge(new_s, s);
		cap[new_s_edge] = sum;
		s = new_s;
	}

	edge_array<double> flow(G);
	start = std::chrono::system_clock::now();
	double flow_value = MAX_FLOW(G, s, t, cap, flow);
	end = std::chrono::system_clock::now();
	duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "-------------LEDA---------------\n";
	std::cout << "The maximum flow has value: " << flow_value << std::endl;
	std::cout << "Time: " << duration.count() << "\n";
	std::cout << "starting copy from leda to boost \n";

	leda::node_array<vertex_d> copy_in_BG(G);
	edge_d Bedge;
	node v;
	forall_nodes(v, G)
	{
		copy_in_BG[v] = add_vertex(BG);
		v_id[copy_in_BG[v]] = ids[v];
		height[copy_in_BG[v]] = 0;
		excess[copy_in_BG[v]] = 0;
	}
	bool isAdded;
	forall_edges(e, G)
	{
		tie(Bedge, isAdded) = add_edge(copy_in_BG[source(e)], copy_in_BG[target(e)], BG);
		residual[Bedge] = cap[e];
	}
	Bs = copy_in_BG[s];
	Bt = copy_in_BG[t];
	n1_length = A.size();
	// std::cout << "returning from leda MAX_FLOW";
	return 0;
}