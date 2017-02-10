#ifndef CME212_GRAPH_HPP
#define CME212_GRAPH_HPP

/** @file Graph.hpp
 * @brief An undirected graph type
 */

#include <algorithm>
#include <set>
#include <unordered_map>
#include <vector>
#include <cassert>

#include "CME212/Util.hpp"
#include "CME212/Point.hpp"


/** @class Graph
 * @brief A template for 3D undirected graphs.
 *
 * Users can add and retrieve nodes and edges. Edges are unique (there is at
 * most one edge between any pair of distinct nodes).
 */
template <typename V>
class Graph {
 private:

 public:

  //
  // PUBLIC TYPE DEFINITIONS
  //

  /** Type of this graph. */
  using graph_type = Graph;

  /** Predeclaration of Node type. */
  class Node;
  /** Synonym for Node (following STL conventions). */
  using node_type = Node;

  using node_value_type = V;

  /** Predeclaration of Edge type. */
  class Edge;
  /** Synonym for Edge (following STL conventions). */
  using edge_type = Edge;

  /** Type of node iterators, which iterate over all graph nodes. */
  class NodeIterator;
  /** Synonym for NodeIterator */
  using node_iterator = NodeIterator;

  /** Type of edge iterators, which iterate over all graph edges. */
  class EdgeIterator;
  /** Synonym for EdgeIterator */
  using edge_iterator = EdgeIterator;

  /** Type of incident iterators, which iterate incident edges to a node. */
  class IncidentIterator;
  /** Synonym for IncidentIterator */
  using incident_iterator = IncidentIterator;

  /** Type of indexes and sizes.
      Return type of Graph::Node::index(), Graph::num_nodes(),
      Graph::num_edges(), and argument type of Graph::node(size_type) */
  using size_type = unsigned;

  //
  // CONSTRUCTORS AND DESTRUCTOR
  //

  /** Construct an empty graph. */
  Graph() {
    this->num_edges_ = 0;
  }

  /** Default destructor */
  ~Graph() = default;

  //
  // NODES
  //

  /** @class Graph::Node
   * @brief Class representing the graph's nodes.
   *
   * Node objects are used to access information about the Graph's nodes.
   */
  class Node : private totally_ordered<Node> {
   public:
    /** Construct an invalid node.
     *
     * Valid nodes are obtained from the Graph class, but it
     * is occasionally useful to declare an @i invalid node, and assign a
     * valid node to it later. For example:
     *
     * @code
     * Graph::node_type x;
     * if (...should pick the first node...)
     *   x = graph.node(0);
     * else
     *   x = some other node using a complicated calculation
     * do_something(x);
     * @endcode
     */
    Node() {
      this->graph_ = NULL;
      this->uid_ = size_type(-1);
    }

    /** Return this node's position. */
    const Point& position() const {
      return graph_->p_nodes[this->uid_];
    }

    /** Return this node's index, a number in the range [0, graph_size). */
    size_type index() const {
      return this->uid_;
    }

    /* Return a reference to the node's value */ 
    node_value_type& value() {
      return graph_->v_nodes[this->uid_];  
    }

    /* Return a const reference to the node's value */
    const node_value_type& value() const {
      return graph_->v_nodes[this->uid_];
    }

    /* Return the number of edges adjacent to the node */
    size_type degree() const {
      auto search = this->graph_->edges.find(this->uid_);
      if (search == this->graph_->edges.end()) {
         return 0;
      } else { 
         return search->second.size();
      }
    }

    /* Return an IncidentIterator to the beginning of the list of edges adjacent
       to this node 
    */ 
    incident_iterator edge_begin() const {
      return IncidentIterator(this->graph_,this->uid_,0);
    }

    /* Return an IncidentIterator to the end of the list of edges adjacent to this
       node
    */
    incident_iterator edge_end() const {
      return IncidentIterator(this->graph_,this->uid_,degree());
    }

    /** Test whether this node and @a n are equal.
     *
     * Equal nodes have the same graph and the same index.
     */
    bool operator==(const Node& n) const {
      return ((this->graph_ == n.graph_) and (this->uid_ == n.uid_));
    }

    /** Test whether this node is less than @a n in a global order.
     *
     * This ordering function is useful for STL containers such as
     * std::map<>. It need not have any geometric meaning.
     *
     * The node ordering relation must obey trichotomy: For any two nodes x
     * and y, exactly one of x == y, x < y, and y < x is true.
     */
    bool operator<(const Node& n) const {
      if (this->graph_ == n.graph_) {
        return (this->uid_ < n.uid_);
      }  else {
        return (this->graph_ < n.graph_);
      }
    }

   private:
    // Allow Graph to access Node's private member data and functions.
    friend class Graph;
    
    /** Private Constructor **/
    Node(const Graph* gr, size_type id) : graph_(const_cast<Graph*>(gr)), 
        uid_(id) {
      assert(gr != NULL);
      assert(id < gr->size());
    }
      
    Graph* graph_;
    size_type uid_;
  };

  /** Return the number of nodes in the graph.
   *
   * Complexity: O(1).
   */
  size_type size() const {
    return this->v_nodes.size();
  }

  /** Synonym for size(). */
  size_type num_nodes() const {
    return size();
  }

  /** Add a node to the graph, returning the added node.
   * @param[in] position The new node's position
   * @param[in] val      The value of the node
   * @post new num_nodes() == old num_nodes() + 1
   * @post result_node.index() == old num_nodes()
   *
   * Complexity: O(1) amortized operations.
   */
  Node add_node(const Point& position, const node_value_type& val = 
      node_value_type()) {
    this->p_nodes.push_back(position);
    this->v_nodes.push_back(val);
    return Node(this,this->size()-1);
  }

  /** Determine if a Node belongs to this Graph
   * @return True if @a n is currently a Node of this Graph
   *
   * Complexity: O(1).
   */
  bool has_node(const Node& n) const {
    return (n.uid_ < this->size());
  }

  /** Return the node with index @a i.
   * @pre 0 <= @a i < num_nodes()
   * @post result_node.index() == i
   *
   * Complexity: O(1).
   */
  Node node(size_type i) const {
    return Node(this,i);
  }

  //
  // EDGES
  //

  /** @class Graph::Edge
   * @brief Class representing the graph's edges.
   *
   * Edges are order-insensitive pairs of nodes. Two Edges with the same nodes
   * are considered equal if they connect the same nodes, in either order.
   */
  class Edge : private totally_ordered<Edge> {
   public:
    /** Construct an invalid Edge. */
    Edge() {
      this->graph_ = NULL;
      this->node1_uid_ = size_type(-1);
      this->node2_uid_ = size_type(-1);
    }

    /** Return a node of this Edge */
    Node node1() const {
      return this->graph_->node(this->node1_uid_);
    }

    /** Return the other node of this Edge */
    Node node2() const {
      return this->graph_->node(this->node2_uid_);
    }

    /** Test whether this edge and @a e are equal.
     *
     * Equal edges represent the same undirected edge between two nodes.
     */
    bool operator==(const Edge& e) const {
      if (this->graph_ == e.graph_) {
        return ( ( (this->node1_uid_ == e.node1_uid_) and 
            (this->node2_uid_ == e.node2_uid_) ) or 
            ( (this->node1_uid_ == e.node2_uid_) and 
            (this->node2_uid_ == e.node1_uid_) ) );
      } else {
        return false;
      }
    }

    /** Test whether this edge is less than @a e in a global order.
     *
     * This ordering function is useful for STL containers such as
     * std::map<>. It need not have any interpretive meaning.
     */
    bool operator<(const Edge& e) const {
      if (this->graph_ == e.graph_) {
        size_type min_e = std::min(e.node1_uid_,e.node2_uid_);
        size_type min_t = std::min(this->node1_uid_,this->node2_uid_);
        if (min_t < min_e) {
          return true;
        } else if (min_t == min_e) {
          if ( std::max(this->node1_uid_,this->node2_uid_) <
              std::max(e.node1_uid_,e.node2_uid_) ) {
            return true;
          }
        }
        return false;
      } else {
        return (this->graph_ < e.graph_);
      }    
    }

   private:
    // Allow Graph to access Edge's private member data and functions.
    friend class Graph;
   
    /* Private Constructor */ 
    Edge(const Graph* gr, size_type n1_uid, size_type n2_uid) :
        graph_(const_cast<Graph*>(gr)), node1_uid_(n1_uid), node2_uid_(n2_uid) {}

    Graph* graph_;
    size_type node1_uid_;
    size_type node2_uid_;

  };

  /** Return the total number of edges in the graph.
   *
   * Complexity: No more than O(num_nodes() + num_edges()), hopefully less
   */
  size_type num_edges() const {
    return num_edges_;
  }

  /** Return the edge with index @a i.
   * @pre 0 <= @a i < num_edges()
   *
   * Complexity: No more than O(num_nodes() + num_edges()), hopefully less
   */
  Edge edge(size_type i) const {
    EdgeIterator ei = edge_begin();
    std::advance(ei,i-1);
    return *ei;
  }

  /** Test whether two nodes are connected by an edge.
   * @pre @a a and @a b are valid nodes of this graph
   * @return True if for some @a i, edge(@a i) connects @a a and @a b.
   *
   * Complexity: No more than O(num_nodes() + num_edges()), hopefully less
   */
  bool has_edge(const Node& a, const Node& b) const {
    auto search = this->edges.find(a.uid_);
    if (search != this->edges.end()) {
      if ((*search).second.find(b.uid_) != (*search).second.end()) {
        return true;
      }
    }
    return false;
  }

  /** Add an edge to the graph, or return the current edge if it already exists.
   * @pre @a a and @a b are distinct valid nodes of this graph
   * @return an Edge object e with e.node1() == @a a and e.node2() == @a b
   * @post has_edge(@a a, @a b) == true
   * @post If old has_edge(@a a, @a b), new num_edges() == old num_edges().
   *       Else,                        new num_edges() == old num_edges() + 1.
   *
   * Can invalidate edge indexes -- in other words, old edge(@a i) might not
   * equal new edge(@a i). Must not invalidate outstanding Edge objects.
   *
   * Complexity: No more than O(num_nodes() + num_edges()), hopefully less
   */
  Edge add_edge(const Node& a, const Node& b) {
    if (!has_edge(a,b)) {
      this->edges[a.uid_].insert(b.uid_);
      this->edges[b.uid_].insert(a.uid_);
      ++this->num_edges_; 
    }
    return Edge(this,a.uid_,b.uid_);
  }

  /** Remove all nodes and edges from this graph.
   * @post num_nodes() == 0 && num_edges() == 0
   *
   * Invalidates all outstanding Node and Edge objects.
   */
  void clear() {
    this->p_nodes.clear();
    this->v_nodes.clear();
    this->edges.clear();
    this->num_edges_ = 0;
  }

  //
  // Node Iterator
  //

  /** @class Graph::NodeIterator
   * @brief Iterator class for nodes. A forward iterator. */
  class NodeIterator : private totally_ordered<NodeIterator> {
   public:
    // These type definitions let us use STL's iterator_traits.
    using value_type        = Node;                     // Element type
    using pointer           = Node*;                    // Pointers to elements
    using reference         = Node&;                    // Reference to elements
    using difference_type   = std::ptrdiff_t;           // Signed difference
    using iterator_category = std::input_iterator_tag;  // Weak Category, Proxy

    /** Construct an invalid NodeIterator. */
    NodeIterator() {
      this->graph_ = NULL;
      this->curr_ = size_type(-1);
    }

    /* Returns the current Node */
    Node operator*() const {
      return Node(this->graph_,curr_); 
    }
   
    /* Increment the iterator by 1 */
    NodeIterator& operator++() {
      ++this->curr_;
      return *this;
    }

    /* Compare iterators */
    bool operator==(const NodeIterator& ni) const {
      return (this->graph_ == ni.graph_) and (this->curr_ == ni.curr_);
    }

   private:
    friend class Graph;
  
    /* Private Constructor to start iterator at any node */
    NodeIterator(const Graph* gr, size_type id) : graph_(const_cast<Graph*>(gr)),
        curr_(id) {
      assert(gr != NULL);
      assert(id <= gr->size());
    }

    Graph* graph_;
    size_type curr_;

  };

  /* Returns a NodeIterator to the first node */
  node_iterator node_begin() const {
    return NodeIterator(this,0);
  }    

  /* Returns a NodeIterator to just after the last node */
  node_iterator node_end() const {
    return NodeIterator(this,this->size());
  }

  //
  // Incident Iterator
  //

  /** @class Graph::IncidentIterator
   * @brief Iterator class for edges incident to a node. A forward iterator. */
  class IncidentIterator : private totally_ordered<IncidentIterator> {
   public:
    // These type definitions let us use STL's iterator_traits.
    using value_type        = Edge;                     // Element type
    using pointer           = Edge*;                    // Pointers to elements
    using reference         = Edge&;                    // Reference to elements
    using difference_type   = std::ptrdiff_t;           // Signed difference
    using iterator_category = std::input_iterator_tag;  // Weak Category, Proxy

    /** Construct an invalid IncidentIterator. */
    IncidentIterator() {
      this->graph_ = NULL;
    }

    /* Return the Current Edge */
    Edge operator*() const {
      return Edge(this->graph_,(*node_a_).first,*node_b_);
    }

    /* Increment the Iterator by 1 and return */
    IncidentIterator& operator++() {
      ++this->node_b_;
      return *this;
    }

    /* Compare IncidientIterators */
    bool operator==(const IncidentIterator& ii) const {
      return (this->graph_ == ii.graph_) and (this->node_a_ == ii.node_a_) and
          (this->node_b_ == ii.node_b_);
    }

   private:
    friend class Graph;
    
    /* Private Constructor for any starting edge or 1 beyond num_edges_*/
    IncidentIterator(const Graph* gr,size_type node1,size_type e) : 
        graph_(const_cast<Graph*>(gr)) {
      assert(gr != NULL);
      this->node_a_ = this->graph_->edges.find(node1);
      if (this->node_a_ != this->graph_->edges.end()) {
        assert(e <= this->graph_->edges[node1].size());
        this->node_b_ = this->node_a_->second.begin();
        for (size_type i=0; i < e; ++i) {
          ++this->node_b_;
        }
      }
    }

    Graph* graph_;
    std::unordered_map<size_type,std::set<size_type>>::iterator node_a_;
    std::set<size_type>::iterator node_b_;
  };

  //
  // Edge Iterator
  //

  /** @class Graph::EdgeIterator
   * @brief Iterator class for edges. A forward iterator. */
  class EdgeIterator : private totally_ordered<EdgeIterator> {
   public:
    // These type definitions let us use STL's iterator_traits.
    using value_type        = Edge;                     // Element type
    using pointer           = Edge*;                    // Pointers to elements
    using reference         = Edge&;                    // Reference to elements
    using difference_type   = std::ptrdiff_t;           // Signed difference
    using iterator_category = std::input_iterator_tag;  // Weak Category, Proxy

    /** Construct an invalid EdgeIterator. */
    EdgeIterator() {
      this->graph_ = NULL;
    }

    /* Return the current Edge */ 
    Edge operator*() const {
     return Edge(this->graph_,(*(this->node_a_)).first,*(this->node_b_)); 
    }
 
    /* Move to the Next Edge and Return the updated iterator */
    EdgeIterator& operator++() {
      size_type a = (*(this->node_a_)).first;
      size_type b = a;
      while ((this->node_a_ != this->graph_->edges.end()) and (b >= a) ) {
        if (++this->node_b_ == ((*(this->node_a_)).second).end()) {
          ++this->node_a_;
          if (this->node_a_ != this->graph_->edges.end()) {
            this->node_b_ = ((*(this->node_a_)).second).begin();
            a = (*this->node_a_).first;
            b = *this->node_b_;
          }
        } else {
          b = *this->node_b_;
        }
      }
      return *this;
    }

    /* Compare EdgeIterators */
    bool operator==(const EdgeIterator& ei) const {
      if ((this->graph_ == ei.graph_) and (this->node_a_ == ei.node_a_)) {
        if ((this->graph_ != NULL) and (this->node_a_ == this->graph_->edges.end())) {
          return true;
        } else {
          return (this->node_b_ == ei.node_b_);
        }
      }
      return false;
    }  
       
   private:
    friend class Graph;
    
     /* Private Constructor for starting at any edge 
      * @post *result = Edge(gr,n1,n2) if has_edge(Node(gr,n1),Node(gr,n2))
               result = edge_end() otherwise 
     */
     EdgeIterator(const Graph* gr, size_type n1, size_type n2) : 
        graph_(const_cast<Graph*>(gr)) {
      assert(gr != NULL);
      // If Edge exists, point to that edge (otherwise iterator is end)
      this->node_a_ = this->graph_->edges.find(n1);
      if (this->node_a_ != this->graph_->edges.end()) {
        this->node_b_ = (*this->node_a_).second.find(n2);
        if (this->node_b_ == (*this->node_a_).second.end()) {
          this->node_a_ = this->graph_->edges.end();
        }
      }
    } 

    Graph* graph_;
    std::unordered_map<size_type,std::set<size_type>>::iterator node_a_;
    std::set<size_type>::iterator node_b_;

  };

  /* Return an EdgeIterator to the first edge */
  edge_iterator edge_begin() const {
    size_type a = 0,b = 0;
    if (num_edges() > 0) {
      a = (*(this->edges.begin())).first;
      b = *(this->edges.at(a).begin());
    }
    return EdgeIterator(this,a,b);
  }

  /* Return an EdgeIterator to one past the last edge */
  edge_iterator edge_end() const {
    return EdgeIterator(this,num_nodes(),0);
  }

 private:

  std::vector<Point> p_nodes;
  std::vector<node_value_type> v_nodes;

  std::unordered_map<size_type,std::set<size_type>> edges;
  size_type num_edges_;

};

#endif // CME212_GRAPH_HPP
