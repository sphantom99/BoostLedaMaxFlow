#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <queue>
#include <vector>
#include <chrono>
#include <boost/graph/graphviz.hpp>
#include "boostHelper/squareboost.h"
#include "leda/ledaRandomBiMaxFlow.h"
using namespace boost;
struct NodeInfo;
struct EdgeInfo;
typedef boost::adjacency_list<vecS, vecS, bidirectionalS, NodeInfo, EdgeInfo> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_d;
typedef boost::graph_traits<Graph>::edge_descriptor edge_d;
vertex_d s, t;
Graph G_random;

std::queue<vertex_d> overflowQ;


// vertex_d A = add_vertex(G_random);
// vertex_d B = add_vertex(G_random);
// vertex_d C = add_vertex(G_random);
// vertex_d D = add_vertex(G_random);
// vertex_d E = add_vertex(G_random);
// vertex_d F = add_vertex(G_random);

// std::pair<edge_d, bool> AB = add_boost::edge(A, B, G_random);
// std::pair<edge_d, bool> AC = add_boost::edge(A, C, G_random);
// std::pair<edge_d, bool> BD = add_boost::edge(B, D, G_random);
// std::pair<edge_d, bool> CE = add_boost::edge(C, E, G_random);
// std::pair<edge_d, bool> DC = add_boost::edge(D, C, G_random);
// std::pair<edge_d, bool> EF = add_boost::edge(E, F, G_random);

// std::pair<edge_d, bool> BA = add_boost::edge(B, A, G_random);
// std::pair<edge_d, bool> CA = add_boost::edge(C, A, G_random);
// std::pair<edge_d, bool> DB = add_boost::edge(D, B, G_random);
// std::pair<edge_d, bool> EC = add_boost::edge(E, C, G_random);
// std::pair<edge_d, bool> CD = add_boost::edge(C, D, G_random);
// std::pair<edge_d, bool> FE = add_boost::edge(F, E, G_random);


int main()
{
	int n1_length = 0;
	bool logging;
	int a_vertices = 0, b_vertices = 0, m_edges = 0;
	std::cout << "Num of A vertices?\n";
	std::cin >> a_vertices;
	std::cout << "Num of B vertices?\n";
	std::cin >> b_vertices;
	std::cout << "Logging (1/0)\n";
	std::cin >> logging;
	// std::cout << "Num of M edges?\n";
	// std::cin >> m_edges;
	ledaRandomBiMaxFlow(G_random, s, t, a_vertices, b_vertices, m_edges, n1_length); //generate_square_bipartite(8, s, t, n1_length);
	// std::cout << "Returned";
	boost::property_map<Graph, int EdgeInfo::*>::type residual = get(&EdgeInfo::residual_flow, G_random);
	boost::property_map<Graph, int NodeInfo::*>::type v_id = get(&NodeInfo::id, G_random);
	boost::property_map<Graph, int NodeInfo::*>::type height = get(&NodeInfo::height, G_random);
	boost::property_map<Graph, int NodeInfo::*>::type excess = get(&NodeInfo::excess, G_random);
	// write_graphviz(std::cout, G_random, make_label_writer(v_id), make_label_writer(residual));
	auto end = std::chrono::system_clock::now(); //declaring timers
	auto start = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	start = std::chrono::system_clock::now();

	graph_traits<Graph>::vertex_iterator vi, vi_end;
	graph_traits<Graph>::edge_iterator ei, ei_end;
	// -----------------------------Initialization----------------------------------
	int nums = 0;
	for (tie(vi, vi_end) = vertices(G_random); vi != vi_end; vi++)
	{ //initialise heights and excess
		v_id[*vi] = nums;
		height[*vi] = 1000000;
		excess[*vi] = 0;
		nums++;
	}
	// std::cout << "S is: " << v_id[s] << " and t is: " << v_id[t] << "\n";

	// residual[AB.first] = 5;
	// residual[AC.first] = 4;
	// residual[BD.first] = 4;
	// residual[DC.first] = 2;
	// residual[CE.first] = 4;
	// residual[EF.first] = 4;
	// residual[BA.first] = 0;
	// residual[CA.first] = 0;
	// residual[DB.first] = 0;
	// residual[CD.first] = 0;
	// residual[EC.first] = 0;
	// residual[FE.first] = 0;
	// std::cout << "inited heights and excess\n";
	height[t] = 0;
	graph_traits<Graph>::out_edge_iterator edgei, edgei_end;
	graph_traits<Graph>::out_edge_iterator edgej, edgej_end;
	graph_traits<Graph>::in_edge_iterator bi, bi_end;

	for (tie(edgei, edgei_end) = out_edges(s, G_random); edgei != edgei_end; edgei++) // max flow on outgoing edges of S and update excess of S's neighbours, add residual edge
	{
		// std::cout << "from S to : " << target(*edgei, G_random) << "\n The edge is: " << residual[*edgei] << "\n";
		excess[target(*edgei, G_random)] = residual[*edgei];
		// std::cout << "updated excess of node\n";
		if (excess[target(*edgei, G_random)] != 0 && target(*edgei, G_random) != t)
			overflowQ.push(target(*edgei, G_random));
		// std::cout << "prob added q\n";
		// std::pair<edge_d, bool> resEdge = boost::edge(target(*edgei, G_random), s, G_random);
		// std::cout << "made edge to back\n";
		residual[boost::edge(target(*edgei, G_random), s, G_random).first] = residual[*edgei];
		// std::cout << "updated rev cap\n";
		residual[*edgei] = 0;
		// std::cout << "zeroed cap\n";
	}
	// std::cout << "inited S things\n";
	std::queue<vertex_d> q;
	q.push(t);
	vertex_d temp;

	while (!q.empty())
	{
		temp = q.front();
		q.pop();
		int alt = 0;
		for (tie(bi, bi_end) = in_edges(temp, G_random); bi != bi_end; bi++)
		{
			// std::cout << "Reading Edge: " << source(*bi, G_random) << " ---> " << target(*bi, G_random);
			alt = height[temp] + 1;
			if (alt < height[source(*bi, G_random)])
			{
				height[source(*bi, G_random)] = alt;
				q.push(source(*bi, G_random));
			}
		}
	}
	// std::cout << "n1_length: " << n1_length << "\n";
	height[s] = 2 * n1_length + 1;

	//-----------------------------------------------------------------------------------------
	//--------------------------Algorithm------------------------------------------------------
	bool admissableEdge1 = false;
	bool admissableEdge2 = false;
	bool elementExistsInQ = false;

	int j_minimum_height = 10000, i_minimum_height = 10000;
	int minimum_flow = 0;
	int times = 0;
	vertex_d k, j, i;
	std::pair<edge_d, bool> newEdge;
	while (!overflowQ.empty())
	{
		// std::cout << "The q has: " << overflowQ.size() << "elements \n";
		vertex_d overflower = overflowQ.front();
		overflowQ.pop();
		// std::cout << "The excess node is: " << overflower << "\n and it's excess is: " << excess[overflower] << "\n";
		admissableEdge1 = false;
		if (overflower == s || overflower == t || excess[overflower] == 0)
			continue;


		for (tie(edgei, edgei_end) = out_edges(overflower, G_random); edgei != edgei_end; edgei++) // max flow on outgoing edges of S and update excess of S's neighbours, add residual edge
		{
			j = target(*edgei, G_random);
			i = overflower;
			// std::cout << "the edge goes from " << i << " To " << j << "and has a residual of: " << residual[*edgei] << "\n";

			if (height[i] > height[j] && residual[*edgei] > 0)
			{

				admissableEdge1 = true;
				// std::cout << "admissable i-j: " << i << "-" << j << "\n";
				admissableEdge2 = false;
				for (tie(edgej, edgej_end) = out_edges(j, G_random); edgej != edgej_end; edgej++)
				{
					if (j == s)
					{
						// std::cout << "found only S to push";
						minimum_flow = std::min(excess[i], residual[*edgei]);
						residual[*edgei] -= minimum_flow;
						// std::cout << "MINFLOW: " << minimum_flow << "\n";
						residual[boost::edge(s, i, G_random).first] += minimum_flow;
						// std::cout << "Pushing from " << i << "to s, flow: " << minimum_flow << "\n";
						excess[i] -= minimum_flow;
						excess[s] += minimum_flow;
						admissableEdge2 = true;
						continue;
					}
					if (height[j] > height[target(*edgej, G_random)] && residual[*edgej] > 0)
					{

						k = target(*edgej, G_random);
						// std::cout << "admissable j-k :" << j << "-" << k << "\n";
						admissableEdge2 = true;
						minimum_flow = std::min({excess[i], residual[*edgei], residual[*edgej]});
						// std::cout << "minimum flow is: " << minimum_flow << "\n";
						excess[i] -= minimum_flow;
						residual[boost::edge(j, i, G_random).first] += minimum_flow;
						residual[*edgei] -= minimum_flow;

						residual[boost::edge(k, j, G_random).first] += minimum_flow;
						residual[boost::edge(j, k, G_random).first] -= minimum_flow;
						excess[k] += minimum_flow;
						if (k != t && k != s)
						{
							overflowQ.push(k);
						}

						// std::cout << "pushed: " << minimum_flow << "\n";
						// excess[j] = 0;
					}
				}
				if (!admissableEdge2)
				{
					// std::cout << "Relabel J\n";

					for (tie(edgej, edgej_end) = out_edges(j, G_random); edgej != edgej_end; edgej++)
					{
						// std::cout << "MinHJ: " << j_minimum_height << " and targetJ: " << height[target(*edgej, G_random)] << "\n";
						if (residual[*edgej] == 0)
							continue;
						if (j_minimum_height > height[target(*edgej, G_random)])
						{
							j_minimum_height = height[target(*edgej, G_random)];
						}
					}
					// std::cout << "The j minimum height is: " << j_minimum_height << " so j's height becomes: " << j_minimum_height + 1 << "\n";
					height[j] = j_minimum_height + 1;
					j_minimum_height = 10000;
				}
			}
		}
		if (!admissableEdge1)
		{
			// std::cout << "Relabel I, the overflower: " << overflower << "\n";
			for (tie(edgei, edgei_end) = out_edges(i, G_random); edgei != edgei_end; edgei++)
			{
				// std::cout << "MinH: " << i_minimum_height << " and target: " << height[target(*edgei, G_random)] << "\n";
				if (residual[*edgei] == 0)
					continue;
				if (i_minimum_height > height[target(*edgei, G_random)])
				{
					i_minimum_height = height[target(*edgei, G_random)];
				}
			}
			// std::cout << "The i minimum height is: " << i_minimum_height << " so i's height becomes: " << i_minimum_height + 1 << "\n";
			height[i] = i_minimum_height + 1;
			i_minimum_height = 10000;
		}
		if (excess[overflower] > 0 && overflower != s && overflower != t)
		{
			overflowQ.push(overflower);
		}

		// // 
		if (logging){
			std::cout << "--------------------------------\n";
			write_graphviz(std::cout, G_random, make_label_writer(excess), make_label_writer(residual));
		}
	}
	// std::cout << "Done";
	// write_graphviz(std::cout, G_random, make_label_writer(excess), make_label_writer(residual));

	end = std::chrono::system_clock::now();
	duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "-------------BOOST-------------\n";
	std::cout << "Duration :" << duration.count() << "\n";
	std::cout << "S is: " << s << "and t is: " << t << "\n";
	std::cout << "Maximum Flow is: " << excess[t] << "\n";
	std::cout << "To visualise the graphs visit: https://dreampuf.github.io/GraphvizOnline \n and paste the dot code which looks like digraph G {...}\n";
	return 0;
}
