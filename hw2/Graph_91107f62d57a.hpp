#ifndef CME212_GRAPH_HPP
#define CME212_GRAPH_HPP

/** @file Graph.hpp
 * @brief An undirected graph type
 */

#include <algorithm>
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
template<typename V, typename E>
class Graph {
 private:

  // HW0: YOUR CODE HERE
  // Use this space for declarations of important internal types you need
  // later in the Graph's definition.
  // (As with all the "YOUR CODE HERE" markings, you may not actually NEED
  // code here. Just use the space if you need it.)

 
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

  /** Synonym for Node value time **/
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

  using edge_value_type = E;
  //
  // CONSTRUCTORS AND DESTRUCTOR
  //

  /** Construct an empty graph. */
  Graph() {
    // HW0: YOUR CODE HERE
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
  class Node: private totally_ordered<Node> {
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
     // HW0: YOUR CODE HERE
    }

   /** Return this node's position. */
    const Point& position() const {
     // HW0: YOUR CODE HERE
      return graph->nodes_[node_index].first;
    }

    /** Return this node's potision to be modified. */ 
    Point& position() {
      return graph->nodes_[node_index].first;
    }
     
    /** Return this node's index, a number in the range [0, graph_size). */
    size_type index() const {
      // HW0: YOUR CODE HERE
      return node_index;
    }

    // HW1: YOUR CODE HERE
    // Supply definitions AND SPECIFICATIONS for:
    node_value_type& value(){
        return graph->nodes_[node_index].second;	
    };
    const node_value_type& value() const{
    	return graph->nodes_[node_index].second;
    };
	
    size_type degree() const{
    	return graph->adj[node_index].size();  
    }
   
    incident_iterator edge_begin() const{
	incident_iterator bedge_iter(graph,node_index,0);   
	return bedge_iter;
    };
    incident_iterator edge_end() const{
   	incident_iterator eedge_iter(graph,node_index,degree());
        return eedge_iter;
    };
    /** Test whether this node and @a n are equal.
     *
     * Equal nodes have the same graph and the same index.
     */
    bool operator==(const Node& n) const {
      // HW0: YOUR CODE HERE 
      return n.graph == graph && node_index == n.index() ? true:false ;
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
      // HW0: YOUR CODE HERE
      return node_index < n.index() ? true:false;
    }

   private:
    // Allow Graph to access Node's private member data and functions.
    friend class Graph;
    // HW0: YOUR CODE HERE
    // Use this space to declare private data members and methods for Node
    // that will not be visible to users, but may be useful within Graph.
    // i.e. Graph needs a way to construct valid Node objects
    Graph* graph;
    Node(const Graph* graph_ ,const size_type node_index_) 
      :graph(const_cast<Graph*>(graph_)),node_index(node_index_){
    }
    size_type node_index;
   };

  /** Return the number of nodes in the graph.
   *
   * Complexity: O(1).
   */
  size_type size() const {
    // HW0: YOUR CODE HERE
    return nodes_.size();
  }

  /** Synonym for size(). */
  size_type num_nodes() const {
    // HW0: YOUR CODE HERE
    return nodes_.size();
  }

  /** Add a node to the graph, returning the added node.
   * @param[in] position The new node's position
   * @post new num_nodes() == old num_nodes() + 1
   * @post result_node.index() == old num_nodes()
   *
   * Complexity: O(1) amortized operations.
   */
  Node add_node(const Point& position, const node_value_type& value = node_value_type()){
     nodes_.push_back(std::make_pair(position,value));
     adj.push_back({});
     edges_.push_back({});
     return Node(this, size() - 1);
  } 
  /** Determine if a Node belongs to this Graph
   * @return True if @a n is currently a Node of this Graph
   *
   * Complexity: O(1).
   */
  bool has_node(const Node& n) const {
    // HW0: YOUR CODE HERE
    return (n.graph == this) && (n.index() < size()) ? true:false;
  }

  /** Remove a node to the graph, returning 1 if node exits 0 otherwise.
   * @param[in] n The node to be removed.
   * @return 1 if has_edge(@a n) 0 otherwise
   * @post has_node(@a n) == false
   * @post If old has_edge(@a n), new num_edges() == old num_edges() - 1.
   *       Else,                  new num_edges() == old num_edges()
   *   
   * Complexity: no more than O(num_nodes())
   */ 
  size_type remove_node(const Node& n){
    if(has_node(n)){
      size_type n_id = n.index();
     while(adj[n_id].size()){
         remove_edge(n,node(adj[n_id].back()));
     }      
     nodes_.erase(nodes_.begin() + n_id);
     adj.erase(adj.begin() + n_id);
     edges_.erase(edges_.begin() + n_id);
      for(size_type i = 0 ; i < adj.size(); ++i){
       for(size_type j = 0 ; j < adj[i].size(); ++j){
         if(adj[i][j] > n_id) adj[i][j]--;
       }
     }
     return 1;
   }
  else return 0;
 }
 
  /** Remove a node to the graph, returning 1 if node exits 0 otherwise.
   * @param[in] n_it The n_iter pointing to the node to be removed.
   * @return   Iterator pointing to the element next to the removed element otherwise if the
   *         node existing otherwise the origin iterator
   * @post has_node(@a *n_it) == false
   * @post If old has_node(@a *n_it), new num_edges() == old num_edges() - 1.
   *       Else,                       new num_edges() == old num_edges()
   *   
   * Complexity: no more than O(num_nodes())
   */ 
  node_iterator remove_node(node_iterator n_it){
       size_type node_id = (*n_it).index();
       if(remove_node(*n_it)){
       auto it = node_begin();
       for(size_type i = 0 ; i < node_id; ++i) it++;
       return it;
       }
       else{
         return n_it;
       }
  }

  /** Return the node with index @a i.
   * @pre 0 <= @a i < num_nodes()
   * @post result_node.index() == i
   *
   * Complexity: O(1).
   */
  Node node(size_type i) const {
    // HW0: YOUR CODE HERE
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
  class Edge: private totally_ordered<Edge> {
    public:
      /** Construct an invalid Edge. */
      Edge() {
      }

      /** Return a node of this Edge */
      Node node1() const {
        // HW0: YOUR CODE HERE   
        return Node(graph,index_1);
      }

      /** Return the other node of this Edge */
      Node node2() const {
        // HW0: YOUR CODE HERE
        return Node(graph,fetch_node2_id());   
      }

      double length() const {
        return norm(node1().position() - node2().position());
      }

      edge_value_type& value(){
         size_type node_min = std::min(node1().index(),node2().index());                
         size_type node_max = std::max(node1().index(),node2().index());  
         for(size_type i = 0 ; i < graph->adj[node_min].size(); ++i){ 
            if(graph->adj[node_min][i] == node_max)
              return graph->edge_value[graph->edges_[node_min][i]];
         }
      }

      const edge_value_type& value() const{
         size_type node_min = std::min(node1().index(),node2().index());         
         size_type node_max = std::max(node1().index(),node2().index());   
         for(size_type i = 0 ; i < graph->adj[node_min].size(); ++i){ 
            if(graph->adj[node_min][i] == node_max)
              return graph->edge_value[graph->edges_[node_min][i]];
         }
      }

      /** Test whether this edge and @a e are equal.
      *
      * Equal edges represent the same undirected edge between two nodes.
      */
      bool operator==(const Edge& e) const {
      // HW0: YOUR CODE HERE
        return e.graph == graph && ( (index_1 == e.index_1 && fetch_node2_id() == e.fetch_node2_id()) || (fetch_node2_id() == e.index_1 && index_1 == e.fetch_node2_id()) ) ;
      }

      /** Test whether this edge is less than @a e in a global order.
       *
       * This ordering function is useful for STL containers such as
       * std::map<>. It need not have any interpretive meaning.
       */
      bool operator<(const Edge& e) const {
        // HW0: YOUR CODE HERE
        if(e.graph == graph){
         std::cout << "comparing" << std::endl;
         size_type auxmax = std::max(index_1,fetch_node2_id());
         size_type auxmin = std::max(index_1,fetch_node2_id());
         size_type newmax = std::max(e.index_1,e.fetch_node2_id());
         size_type newmin = std::min(e.index_1,e.fetch_node2_id());
         if( auxmin < newmin  || (auxmin == newmin && auxmax < newmax) ){
             return true;
         }
         else{
             return false;
          }
         }
         else{
             return graph < e.graph;
         } 
      }


      private:
      // Allow Graph to access Edge's private member data and functions.
      friend class Graph;
      // HW0: YOUR CODE HERE
      // Use this space to declare private data members and methods for Edge
      // that will not be visible to users, but may be useful within Graph.
      // i.e. Graph needs a way to construct valid Edge objects
      Graph* graph; 
      size_type index_1;
      size_type aux_index_2;

      size_type fetch_node2_id() const{
          return graph->adj[index_1][aux_index_2];
      }
      Edge(const Graph* graph_ ,size_type index_1, size_type aux_index_2) 
      :graph(const_cast<Graph*>(graph_)), index_1(index_1), aux_index_2(aux_index_2){
      }
    };

    /** Return the total number of edges in the graph.
     *
     * Complexity: No more than O(num_nodes() + num_edges()), hopefully less
     */
    size_type num_edges() const {
      // HW0: YOUR CODE HERE
      //return std::distance(edge_begin(),edge_end());
      return edge_value.size();
    }

    /** Return the edge with index @a i.
     * @pre 0 <= @a i < num_edges()
     *
     * Complexity: No more than O(num_nodes() + num_edges()), hopefully less
     */
    Edge edge(size_type i) const {
      // HW0: YOUR CODE HERE 
      return *std::next(edge_begin(),i);
    }

    /** Test whether two nodes are connected by an edge.
     * @pre @a a and @a b are valid nodes of this graph
     * @return True if for some @a i, edge(@a i) connects @a a and @a b.
     *
     * Complexity: No more than O(num_nodes() + num_edges()), hopefully less
     */
     bool has_edge(const Node& a, const Node& b) const {
       // HW0: YOUR CODE HERE 
        for(size_type i = 0 ; i < adj[a.index()].size() ; ++i){
           if(adj[a.index()][i] == b.index())  return true;
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
     Edge add_edge(const Node& a, const Node& b,const edge_value_type& value_ = edge_value_type()) {	
       // HW0: YOUR CODE HERE
       for(size_type i = 0 ; i < adj[a.index()].size() ; ++i){
         if(adj[a.index()][i] == b.index()) 
           return Edge(this,a.index(),i);
       }
       adj[a.index()].push_back(b.index());
       adj[b.index()].push_back(a.index());
       size_type index_min = std::min(a.index(),b.index());
       edge_value.push_back(value_);
       edges_[a.index()].push_back(edge_value.size() - 1);
       edges_[b.index()].push_back(edge_value.size() - 1);
       return Edge(this,a.index(),adj[a.index()].size() - 1 );
     }


      /** Remove an edge from the graph, or return 1 if existing 0 otherwise.
      * param[in] Node a, Node b
      * @return 1 if has_edge(@a, @b) 0 otherwise
      *
      * @pre @a a and @a b are distinct valid nodes of this graph
      * @post has_edge(@a a, @a b) == false
      * @post If old has_edge(@a a, @a b), new num_edges() == old num_edges() - 1.
      *       Else,                        new num_edges() == old num_edges()
      *
      * Complexity: No more than O(num_nodes() + num_edges())
      */ 
    size_type remove_edge(const Node& a, const Node& b){
      if(has_edge(a,b)){
        size_type a_id = a.index();
        size_type b_id = b.index();
        size_type edge_id;
        for(size_type i = 0 ; i < adj[a_id].size(); ++i){
            if( adj[a_id][i] == b_id ){
                adj[a_id].erase(adj[a_id].begin() + i);
                edge_id = edges_[a_id][i];
                edges_[a_id].erase(edges_[a_id].begin() + i);
                break; 
            }
        } 
        for(size_type i = 0 ; i < adj[b_id].size(); ++i){
            if( adj[b_id][i] == a_id ){
                adj[b_id].erase(adj[b_id].begin() + i);
                edges_[b_id].erase(edges_[b_id].begin() + i);
                break; 
            }
        }
        edge_value.erase(edge_value.begin() + edge_id);
        for(size_type i = 0 ; i < edges_.size(); ++i){
            for(size_type j = 0 ; j < edges_[i].size(); ++j){
               if(edges_[i][j] > edge_id) edges_[i][j]--; 
            }
        }
        return 1;
      }
      else{
        return 0;
      }           
   }



  /** Remove an edge from the graph, or return 1 if existing 0 otherwise.
   * param[in] e Edge to be removed
   * @return 1 if has_edge(@a e) 0 otherwise
   * @post has_edge(@a e) == false
   * @post If old has_edge(@a e), new num_edges() == old num_edges() - 1.
   *       Else,                        new num_edges() == old num_edges()
   *
   * Complexity: No more than O(num_nodes() + num_edges())
   */ 
   size_type remove_edge(const Edge& e){
       return remove_edge(e.node1(),e.node2());
   }

   /** Remove an edge from the graph, or return 1 if existing 0 otherwise.
    * param[in] e_it edge_iterator pointing to the edge to be removed.
    * @return 1 if has_edge(*(@a e_it)) e_it otherwise
    *
    * @pre @a a and @a b are distinct valid nodes of this graph
    * @post has_edge((*(@a e_it))) == false
    * @post If old has_edge((*(@a e_it))), new num_edges() == old num_edges() - 1.
    *       Else,                        new num_edges() == old num_edges()
    *
    * Complexity: No more than O(num_nodes() + num_edges())
   */  
   edge_iterator remove_edge(edge_iterator e_it){       
       if(remove_edge(*e_it)){
          edge_iterator next = ++e_it; 
          return next;
       }else{
          return e_it;
       }
   };

     /** Remove all nodes and edges from this graph.
      * @post num_nodes() == 0 && num_edges() == 0
      *
      * Invalidates all outstanding Node and Edge objects.
      */
     void clear() {
        nodes_.clear();
        adj.clear();
        edges_.clear();
        edge_value.clear();
     }
      
      //
      // Node Iterator
      //

  /** @class Graph::NodeIterator
   * @brief Iterator class for nodes. A forward iterator. */
  class NodeIterator:private totally_ordered<NodeIterator>  {
   public:
    // These type definitions let us use STL's iterator_traits.
    using value_type        = Node;                     // Element type
    using pointer           = Node*;                    // Pointers to elements
    using reference         = Node&;                    // Reference to elements
    using difference_type   = std::ptrdiff_t;           // Signed difference
    using iterator_category = std::input_iterator_tag;  // Weak Category, Proxy

    /** Construct an invalid NodeIterator. */
    NodeIterator() {
    };

    // HW1 #2: YOUR CODE HERE
    // Supply definitions AND SPECIFICATIONS for:
    Node operator*() const{
	return Node(graph,cur); 
    };

    NodeIterator& operator++(){
        ++cur;
	return *this;
    };
    bool operator==(const NodeIterator& n) const{
    	return (graph == n.graph) && (cur == n.cur )? true:false;
    };

    size_type node_id(){
       return cur;
     }
   private:
     friend class Graph;
      // HW1 #2: YOUR CODE HERE
     Graph* graph; 
     NodeIterator(const Graph* graph_ ,size_type n_iter) 
      :graph(const_cast<Graph*>(graph_)),cur(n_iter){
      }
    size_type cur;
 };

  // HW1 #2: YOUR CODE HERE
  // Supply definitions AND SPECIFICATIONS for:
   NodeIterator node_begin() const{
      NodeIterator biter(this,0);
      return biter;
   };
   NodeIterator node_end() const{ 
      NodeIterator eiter(this,size());
      return eiter;
   }

  //
  // Incident Iterator
  //

  /** @class Graph::IncidentIterator
   * @brief Iterator class for edges incident to a node. A forward iterator. */
  class IncidentIterator:private totally_ordered<IncidentIterator>  {
   public:
    // These type definitions let us use STL's iterator_traits.
    using value_type        = Edge;                     // Element type
    using pointer           = Edge*;                    // Pointers to elements
    using reference         = Edge&;                    // Reference to elements
    using difference_type   = std::ptrdiff_t;           // Signed difference
    using iterator_category = std::input_iterator_tag;  // Weak Category, Proxy

    /** Construct an invalid IncidentIterator. */
    IncidentIterator() {
    }

    // HW1 #3: YOUR CODE HERE
    // Supply definitions AND SPECIFICATIONS for:
    Edge operator*() const{
      return Edge(graph,node1_id,aux_node2_id);
    };
    
    IncidentIterator& operator++(){
	++aux_node2_id;
        return *this;
    };
    
    bool operator==(const IncidentIterator& n) const{
	return (graph == n.graph) && (node1_id == n.node1_id) && (aux_node2_id == n.aux_node2_id)? true:false;
    };
 
   private:
    friend class Graph;
    // HW1 #3: YOUR CODE HERE
    Graph* graph; 
    IncidentIterator(const Graph* graph_,size_type n_id, size_type i_id) 
      :graph(const_cast<Graph*>(graph_)),node1_id(n_id),aux_node2_id(i_id){
    }
    size_type node1_id;
    size_type aux_node2_id;
  };

  //
  // Edge Iterator
  //

  /** @class Graph::EdgeIterator
   * @brief Iterator class for edges. A forward iterator. */
  class EdgeIterator:private totally_ordered<EdgeIterator> {
   public:
    // These type definitions let us use STL's iterator_traits.
    using value_type        = Edge;                     // Element type
    using pointer           = Edge*;                    // Pointers to elements
    using reference         = Edge&;                    // Reference to elements
    using difference_type   = std::ptrdiff_t;           // Signed difference
    using iterator_category = std::input_iterator_tag;  // Weak Category, Proxy

    /** Construct an invalid EdgeIterator. */
    EdgeIterator() {
    }

    // HW1 #5: YOUR CODE HERE
    // Supply definitions AND SPECIFICATIONS for:
    Edge operator*() const{
        return Edge(graph,node1_id,aux_node2_id);
    };

    EdgeIterator& operator++(){
        ++aux_node2_id;
        makeIteratorGreaterAgain();
        return *this;
    };


    bool operator==(const EdgeIterator& n) const{
       return (graph == n.graph) && (node1_id == n.node1_id) && (aux_node2_id == n.aux_node2_id);
    };

     
   private:
     friend class Graph;
      // W1 #5: YOUR CODE HERE
    Graph* graph; 
    EdgeIterator(const Graph* graph_,size_type n1, size_type n2) 
      :graph(const_cast<Graph*>(graph_)),node1_id(n1),aux_node2_id(n2){
    }
    size_type node1_id;
    size_type aux_node2_id;

    void makeIteratorGreaterAgain(){
         while(true){ 
            if(node1_id == graph->num_nodes()){
                 break;
            }
            else if(aux_node2_id == graph->adj[node1_id].size()){
                aux_node2_id = 0;
                node1_id++;
            }
	    else if( node1_id > graph->adj[node1_id][aux_node2_id]){   
               aux_node2_id++;
            }
            else{
               break;
            }
         }
    }
   };

    // HW1 #5: YOUR CODE HERE
    //Supply definitions AND SPECIFICATIONS for:
   edge_iterator edge_begin() const{
     EdgeIterator biter(this,0,0);
     return biter;
    }
   edge_iterator edge_end() const{
     EdgeIterator eiter(this,num_nodes(),0);
      return eiter;
   }
  
  private:
  // HW0: YOUR CODE HERE
  // Use this space for your Graph class's internals:
  // helper functions, data members, and so forth.
    std::vector<std::pair<Point, node_value_type> > nodes_;
    std::vector<std::vector<size_type> > adj;
    std::vector<std::vector<size_type> > edges_;
    std::vector<edge_value_type> edge_value;
};
#endif // CME212_GRAPH_HPP
