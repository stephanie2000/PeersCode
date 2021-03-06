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
struct EdgeData {
  double length;       //initial edge length
  double K;  //spring constant
};
// Define the Graph type
using GraphType = Graph<NodeData,EdgeData>;
using Node = typename GraphType::node_type;
using Edge = typename GraphType::edge_type;

struct PlaneConstraint{
   template<typename NODE>
   void operator()(NODE n){
      if (dot(n.position(),Point(0,0,1))<-.75){
         n.value().vel=Point(0,0,0);
         n.position()=Point(n.position().x,n.position().y,-.75);
      }
   }
};

struct SphereConstraint{
   Point c=Point(.5,.5,-.5);
   template<typename NODE>
   void operator()(NODE n){
      if( norm_2(n.position()-c)<.15){
         Point Ri=(n.position()-c)/norm_2(n.position()-c);
         n.value().vel=n.value().vel-dot(n.value().vel,Ri)*Ri;
      }
   }
};

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

    // Update the position of the node according to its velocity
    // x^{n+1} = x^{n} + v^{n} * dt
    if ((n.position()==Point(0,0,0)) || (n.position()==Point(1,0,0))){
    }else{
       n.position() += n.value().vel * dt;
    }
  }

  // Compute the t+dt velocity
  for (auto it = g.node_begin(); it != g.node_end(); ++it) {
    auto n = *it;

    // v^{n+1} = v^{n} + F(x^{n+1},t) * dt / m
    n.value().vel += force(n, t) * (dt / n.value().mass);
  }

  return t + dt;
    // HW2 #1: YOUR CODE HERE
}
struct DampingForce{
template <typename NODE>
   Point operator()(NODE n){
      return n.value().vel*-1/n.graph_->size();
   }
};
struct GravityForce{
template <typename NODE>
   Point operator()(NODE n){
      return -1*grav*Point(0,0,1)*n.value().mass;
   }
};
struct SpringForce{
template<typename NODE>
   Point operator()(NODE n){
    // HW2 #1: YOUR CODE HERE
    Point force;
    auto begin=n.edge_begin();
    auto end=n.edge_end();
    for ( ; begin!=end ; ++begin){
       if (n==(*begin).node1()){
           force+=-1*(*begin).value().K*(n.position()-(*begin).node2().position())*((*begin).length()-(*begin).value().length)/((*begin).length());
       }else{
           force+=-1*(*begin).value().K*(n.position()-(*begin).node1().position())*((*begin).length()-(*begin).value().length)/((*begin).length());
       }
    }
    return force;
  }
};

struct ZeroForce{
   template<typename NODE>
   Point operator()(NODE n){
      (void) n;
      return Point(0,0,0);
   }
};


template<typename F1, typename F2, typename F3=ZeroForce>
struct make_combined_force{
   F1 f1;
   F2 f2;
   F3 f3;
   template<typename NODE>
   Point operator()(NODE n,double t){
      (void) t;
      return f1(n)+f2(n)+f3(n);
   }
};

/** Force function object for HW2 #1. */
struct Problem1Force {
  /** Return the force applying to @a n at time @a t.
   *
   * For HW2 #1, this is a combination of mass-spring force and gravity,
   * except that points at (0, 0, 0) and (1, 0, 0) never move. We can
   * model that by returning a zero-valued force. */
  template <typename NODE>
  Point operator()(NODE n, double t) {
    // HW2 #1: YOUR CODE HERE
    (void) t;    // silence compiler warnings
    Point force;
    auto begin=n.edge_begin();
    auto end=n.edge_end();
    for ( ; begin!=end ; ++begin){
       if (n==(*begin).node1()){
           force+=-1*(*begin).value().K*(n.position()-(*begin).node2().position())*((*begin).length()-(*begin).value().length)/((*begin).length());
       }else{
           force+=-1*(*begin).value().K*(n.position()-(*begin).node1().position())*((*begin).length()-(*begin).value().length)/((*begin).length());
       }
    }
    force+=grav*Point(0,0,-1)*n.value().mass;
    if (n.position() == Point(0,0,0) || n.position() == Point(1,0,0))
       return Point(0,0,0);
    return force;
  }
};

template<typename constraint1,typename constraint2=constraint1>
struct Constraint{
   constraint1 c1;
   constraint2 c2;
   void operator()(GraphType& graph){
      for (auto n=graph.node_begin();n!=graph.node_end();++n){
          c1(*n);
          c2(*n);
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
    // Diagonal edges: include as of HW2 #2
    graph.add_edge(nodes[t[0]], nodes[t[3]]);
    graph.add_edge(nodes[t[1]], nodes[t[2]]);
    graph.add_edge(nodes[t[1]], nodes[t[3]]);
    graph.add_edge(nodes[t[2]], nodes[t[3]]);
  }

  // HW2 #1 YOUR CODE HERE
  // Set initial conditions for your nodes, if necessary.
  // Iterate through nodes to set IC
    for (auto begin=graph.node_begin(); begin!=graph.node_end(); ++begin){
        (*begin).value().vel=Point(0,0,0);
        (*begin).value().mass=1.0/graph.num_nodes();
     
    }
    for (auto begin=graph.edge_begin(); begin!=graph.edge_end(); ++begin){
        (*begin).value().K=100;
        (*begin).value().length=(*begin).length();
    } 

  //Forces
  make_combined_force<GravityForce, SpringForce> force;
  //Problem1Forces force
   
  //Constraints
  Constraint<PlaneConstraint,SphereConstraint> constraint;
  //Constraint<PlaneConstraint> constraint;


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
      double dt = 0.0005;
      double t_start = 0;
      double t_end = 5.0;

      for (double t = t_start; t < t_end && !interrupt_sim_thread; t += dt) {
        //std::cout << "t = " << t << std::endl;
        symp_euler_step(graph, t, dt, force);
        constraint(graph);
        
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
