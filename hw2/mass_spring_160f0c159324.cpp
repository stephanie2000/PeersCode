/**
 * @file mass_spring.cpp
 * Implementation of mass-spring system using Graph
 *
 * @brief Reads in two files specified on the command line.
 * First file: 3D Points (one per line) defined by three doubles
 * Second file: Tetrahedra (one per line) defined by 4 indices into the point
 * list
 */

#include <fstream>
#include <chrono>
#include <thread>

#include "CME212/SFML_Viewer.hpp"
#include "CME212/Util.hpp"
#include "CME212/Color.hpp"
#include "CME212/Point.hpp"

#include "Graph.hpp"


// Gravity in meters/sec^2
static constexpr double grav = 9.81;

/** Custom structure of data to store with Nodes */
struct NodeData {
  Point vel;       //< Node velocity
  double mass;     //< Node mass
  NodeData() : vel(0), mass(1) {}
};

/** Custom structure of data to store with Edges */
struct EdgeData {
	double K;
	double L;
};

// Define the Graph type
using GraphType = Graph<NodeData, EdgeData>;
using Node = typename GraphType::node_type;
using Edge = typename GraphType::edge_type;


/** Change a graph's nodes according to a step of the symplectic Euler
 *    method with the given node force.
 * @param[in,out] g      Graph
 * @param[in]     t      The current time (useful for time-dependent forces)
 * @param[in]     dt     The time step
 * @param[in]     force  Function object defining the force per node
 * @return the next time step (usually @a t + @a dt)
 *
 * @tparam G::node_value_type supports ???????? YOU CHOOSE
 * @tparam F is a function object called as @a force(n, @a t),
 *           where n is a node of the graph and @a t is the current time.
 *           @a force must return a Point representing the force vector on
 *           Node n at time @a t.
 */
template <typename G, typename F>
double symp_euler_step(G& g, double t, double dt, F force) {
  // Compute the t+dt position
  for (auto it = g.node_begin(); it != g.node_end(); ++it) {
    auto n = *it;

    if(n.position()==Point(0,0,0) or n.position()==Point(1,0,0))
      continue;
    // Update the position of the node according to its velocity
    // x^{n+1} = x^{n} + v^{n} * dt
    n.position() += n.value().vel * dt;
  }

  // Compute the t+dt velocity
  for (auto it = g.node_begin(); it != g.node_end(); ++it) {
    auto n = *it;

    if(n.position()==Point(0,0,0) or n.position()==Point(1,0,0))
      continue;
    // v^{n+1} = v^{n} + F(x^{n+1},t) * dt / m
    n.value().vel += force(n, t) * (dt / n.value().mass);
  }

  return t + dt;
}

/** Force function object for HW2 #1. */
struct Problem1Force {
	/** Return the force applying to @a n at time @a t.
		*
		* For HW2 #1, this is a combination of mass-spring force and gravity,
		* except that points at (0, 0, 0) and (1, 0, 0) never move. We can
		* model that by returning a zero-valued force. */
	template <typename NODE>
		Point operator()(NODE n, double t){
		// HW2 #1: YOUR CODE HERE
			//Points at (0,0,0) and (1,0,0) never move
			if(n.position()==Point(0,0,0) or n.position()==Point(1,0,0))
				return Point(0,0,0);

			Point f_spring=Point(0,0,0), pi=n.position();
			/* Computes f_spring */
			for(auto iit=n.edge_begin() ; iit != n.edge_end() ; ++iit){
				double K=(*iit).value().K, L=(*iit).value().L;
				Point pj=(*iit).node2().position();
				f_spring -= K*(norm(pi-pj)-L)/norm(pi-pj)*(pi-pj);
			}
			return f_spring + Point(0,0,-grav*n.value().mass);
		}
};

/** Gravity force function object */
struct GravityForce {
	template <typename NODE>
	Point operator()(NODE n, double t){
		if (n.position() == Point(0,0,0) or n.position() == Point(1,0,0))
			return Point(0,0,0);

		return Point(0,0,-grav*n.value().mass);
	}
};

/** Spring force function object */
struct MassSpringForce {
	template <typename NODE>
		Point operator()(NODE n, double t){
			if (n.position() == Point(0,0,0) or n.position() == Point(1,0,0))
				return Point(0,0,0);

			Point f_spring = Point(0,0,0), pi=n.position();
			for(auto iit=n.edge_begin() ; iit!=n.edge_end() ; ++iit){
				double K = (*iit).value().K, L = (*iit).value().L;
				Point pj=(*iit).node2().position();
				f_spring -= K*(pi-pj)/norm(pi-pj)*(norm(pi-pj)-L);
			}
			return f_spring;
		}
};

/** Damping force function object */
struct DampingForce {
	template <typename NODE>
	Point operator()(NODE n, double t){
		return -c * n.value().vel;
	}
	private:
	  double c;
};

template <typename F1, typename F2>
struct CombinedForce2 {
	template<typename NODE>
		Point operator(NODE n, double t){
			return f1(n,t) + f2(n,t);
		}
	private:
		F1 f1;
		F2 f2;
};

template <typename F1, typename F2, typename F3>
struct CombinedForce3 {
	template<typename NODE>
		Point operator(NODE n, double t){
			return f1(n,t) + f2(n,t) + f3(n,t);
		}
	private:
		F1 f1;
		F2 f2;
		F3 f3;
};

template <typename F1, typename F2>
CombinedForce2 make_combined_force(F1 f1, F2 f2){
	return {f1, f2};
}

template <typename F1, typename F2, typename F3>
CombinedForce3 make_combined_force(F1 f1, F2 f2, F3 f3){
	return {f1, f2, f3};
}

/** Plane constraint function object */
struct PlaneConstraint {
	void operator()(GraphType graph, double t){
		for(nit=graph.node_begin() ; nit!=graph.node_end() ; ++nit){
			if((*nit).position().z < -0.75){
				(*nit).position*().z = -0.75;
				(*nit).value().vel.z = 0;
			}
		}	
	}
};

/** Sphere constraint function object */
struct SphereConstraint {
	void operator()(GraphType graph, double t){
		double r=0.15;
		Point c = Point(0.5, 0.5, -0.5);
		for(nit=graph.node_begin() ; nit!=graph.node_end() ; ++nit){
			Point p = (*nit).position();
			if(norm((p-c) < r){
				(*nit).position() = c+r/norm(p-c)*(p-c);
				(*nit).value().vel -= dot((*nit).value().vel, (p-c)/norm(p-c))*(p-c);
			}
		}	
	}
};

/** Sphere constraint with destruction function object */
struct SphereConstraintDestruction {
	void operator()(GraphType graph, double t){
		double r=0.15;
		Point c = Point(0.5, 0.5, -0.5);
		for(nit=graph.node_begin() ; nit!=graph.node_end() ; ++nit){
			Point p = (*nit).position();
			if(norm((p-c) < r)
					nit=graph.remove_node(nit);
			
		}	
	}
};


int main(int argc, char** argv)
{
  // Check arguments
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " NODES_FILE TETS_FILE\n";
    exit(1);
  }

  // Construct an empty graph
  GraphType graph;

  // Create a nodes_file from the first input argument
  std::ifstream nodes_file(argv[1]);
  // Interpret each line of the nodes_file as a 3D Point and add to the Graph
  Point p;
  std::vector<typename GraphType::node_type> nodes;
  while (CME212::getline_parsed(nodes_file, p))
    nodes.push_back(graph.add_node(p));

  // Create a tets_file from the second input argument
  std::ifstream tets_file(argv[2]);
  // Interpret each line of the tets_file as four ints which refer to nodes
  std::array<int,4> t;
  while (CME212::getline_parsed(tets_file, t)) {
    graph.add_edge(nodes[t[0]], nodes[t[1]]);
    graph.add_edge(nodes[t[0]], nodes[t[2]]);
#if 0
    // Diagonal edges: include as of HW2 #2
    graph.add_edge(nodes[t[0]], nodes[t[3]]);
    graph.add_edge(nodes[t[1]], nodes[t[2]]);
#endif
    graph.add_edge(nodes[t[1]], nodes[t[3]]);
    graph.add_edge(nodes[t[2]], nodes[t[3]]);
  }

  // HW2 #1 YOUR CODE HERE
  // Set initial conditions for your nodes, if necessary.
	for(auto nit=graph.node_begin() ; nit!=graph.node_end() ; ++nit)
		(*nit).value() = {1/graph.num_nodes(), Point(0,0,0)};
	

	for(auto eit=graph.edge_begin() ; eit!=graph.edge_end() ; ++eit)
		(*eit).value() = {100, norm((*eit).node1().position()-(*eit).node2().position())};
	

  // Print out the stats
  std::cout << graph.num_nodes() << " " << graph.num_edges() << std::endl;

  // Launch the Viewer
  CME212::SFML_Viewer viewer;
  auto node_map = viewer.empty_node_map(graph);

  viewer.add_nodes(graph.node_begin(), graph.node_end(), node_map);
  viewer.add_edges(graph.edge_begin(), graph.edge_end(), node_map);

  viewer.center_view();

  // We want viewer interaction and the simulation at the same time
  // Viewer is thread-safe, so launch the simulation in a child thread
  bool interrupt_sim_thread = false;
  auto sim_thread = std::thread([&](){

      // Begin the mass-spring simulation
      double dt = 0.001;
      double t_start = 0;
      double t_end = 5.0;

      for (double t = t_start; t < t_end && !interrupt_sim_thread; t += dt) {
        //std::cout << "t = " << t << std::endl;
        symp_euler_step(graph, t, dt, Problem1Force());

        // Update viewer with nodes' new positions
        viewer.add_nodes(graph.node_begin(), graph.node_end(), node_map);
        viewer.set_label(t);

        // These lines slow down the animation for small graphs, like grid0_*.
        // Feel free to remove them or tweak the constants.
        if (graph.size() < 100)
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }

    });  // simulation thread

  viewer.event_loop();

  // If we return from the event loop, we've killed the window.
  interrupt_sim_thread = true;
  sim_thread.join();

  return 0;
}
