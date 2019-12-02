#include <stdio.h>
#include <vector>
#include <random>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>


using std::vector;
using std::cout;
using std::endl;
using std::cin;


// #define GLM_MESSAGES
// #define GLM_SWIZZLE
#define GLM_FORCE_SWIZZLE
#define GLM_SWIZZLE_XYZW
#include "resources/glm/glm.hpp" //general vector types
#include "resources/glm/gtc/matrix_transform.hpp" // for glm::ortho
#include "resources/glm/gtc/type_ptr.hpp" //to send matricies gpu-side
#include "resources/glm/gtx/transform.hpp"


#include "resources/perlin.h"







//--------------------

class edge
{
public:
	edge(int dest, float wei) { destination = dest; weight = wei; }

	int destination;	//id of target node
	float weight;

private:

};





class edge_record
{
public:
	edge_record(int fir, int sec, float wei) {first = fir; second = sec; weight = wei;}

	int first;
	int second;

	float weight;

private:

};


class Vedge
{
public:
	Vedge(glm::vec3 fir, glm::vec3 sec, float wei) {first = fir; second = sec; weight = wei;}

	glm::vec3 first;
	glm::vec3 second;

	float weight;

private:

};




//--------------------

class node
{
public:
	int id;

	void add_link(edge link) { linkages.push_back(link); }
	bool alone() { return(linkages.empty());  }
	int size() { return(linkages.size()); }

	edge get_link(int num) { return linkages[num]; }

	vector<edge> linkages;

	glm::vec3 location;
private:

	float value;

	//other information?

};













//--------------------

class graph
{
public:
	graph(int num, int connections);

	void create_random_edge_log(int how_many);
	void add_edge_to_log(int first, int second, float weight);

	void reconstruct(); //clears nodes,

	void show_graph();
	void show_edge_log();
	void show_mst();

	void save_to_file(std::string filename);
	void load_from_file(std::string filename);

	void output_for_voraldo();

private:
	vector<node> nodes;
	vector<edge_record> edge_log;
	int num_nodes, num_connections;


	vector<Vedge> all_edge_list;
	vector<Vedge> mst_edge_list;
	vector<glm::vec3> verticies;
};










//--------------------

graph::graph(int num, int connections)
{
	nodes.clear();
	edge_log.clear();

	nodes.resize(num);

	num_nodes = num;
	num_connections = connections;

	create_random_edge_log(connections);
	reconstruct();
}









//--------------------

void graph::add_edge_to_log(int first, int second, float weight)
{


	edge_log.push_back(edge_record(first,second,weight));
}








//--------------------

void graph::create_random_edge_log(int how_many)
{

	cout << "now making " << how_many << " random weighted connections" << endl;

	edge_log.clear();

	//random number generation
  std::random_device rd;
  std::mt19937 mt(rd());

	if(nodes.size() != 0)
	{








		std::uniform_real_distribution<float> dist1(0.3f,9.8f); //input values
		std::uniform_int_distribution<int> dist2(0,nodes.size()-1);  //nodes

		for(int i = 0; i < how_many; i++)
		{

			int first = dist2(mt);
			int second = dist2(mt);

			float weight = dist1(mt);

			// cout << std::setw(4) <<  i << " : making connection from " <<  std::setw(4) << first << " to " <<  std::setw(4) << second << " and back with weight " << weight << endl;

			add_edge_to_log(first, second, weight);

		}

		cout << endl << endl;
	}
}












//--------------------

void graph::show_graph(){

    if(!nodes.empty())
		{
        for(int i = 0; i < nodes.size(); i++)
        {
            cout << "node " << std::setw(4) << i << " has links to: ";
            if(!nodes[i].alone())
            {
                for(int j = 0; j < nodes[i].size(); j++)
                {//j indexes the list of nodes, no other significance, unless you ordered them ahead of time
									cout << std::fixed << std::setprecision(2) << std::setw(4); //only show two decimal placese, i.e. 90.04
									cout << nodes[i].get_link(j).destination << "(" << nodes[i].get_link(j).weight << ") ";

									if(j > 4)
									{
										cout << "...";
										break;
									}
                }
                cout << endl;
            }
						else
            {
                cout<< " no other nodes" << endl;
            }

        }
    }
		else
		{

        std::cout<<std::endl<<"Graph is empty!"<<std::endl<<std::endl;
    }
		cout << endl;
}









//--------------------

void graph::show_edge_log()
{
	for(int i = 0; i < edge_log.size(); i++)
	{
		cout << std::setw(4) <<  i << " : making connection from " <<  std::setw(4) << edge_log[i].first << " to " <<  std::setw(4) << edge_log[i].second << " and back with weight " << edge_log[i].weight << endl;
	}
}



//--------------------

bool comparison(const edge &a, const edge &b)
{//  this is used to satisfy the heap functions parameter format
  return (a.weight > b.weight);
}


//--------------------

void graph::show_mst()
{//implementing prims algorithm

	mst_edge_list.clear();

	vector<edge> min_heap;	//used to sort by weights

	int src = 0; // Taking vertex 0 as source

  // Create a vector for keys and initialize  keys as infinite (99)
  vector<int> key(num_nodes, 9999);

  // To store parent array which in turn store MST
  vector<int> parent(num_nodes, -1);

  // To keep track of vertices included in MST
  vector<bool> inMST(num_nodes, false);

  // Insert source itself in priority queue and initialize its key as 0.
  min_heap.push_back(edge(src, 0));
  key[src] = 0;



	/* Looping till priority queue becomes empty */
	while (!min_heap.empty())
	{
		//create the heap
		std::make_heap(min_heap.begin(), min_heap.end(), comparison);// all new entries get sorted with make_heap()

	    // The first vertex in pair is the minimum weight
	    // vertex, extract it from the heap.
	    // vertex label is stored in destination (it
	    // is done this way to keep the vertices
	    // sorted by weight (weight is key)

  	int u = min_heap[0].destination;
		min_heap.erase(min_heap.begin()); // we erase the first element (pop)

    inMST[u] = true; // Include vertex in MST

    // Traverse all edges adjacent to u
    for (auto x : nodes[u].linkages)
    {
      // Get vertex label and weight of current adjacent of u.
      int v = x.destination;
      int weight = x.weight;

      // If v is not in MST and weight of (u,v) is smaller than current key of v
      if (inMST[v] == false && key[v] > weight)
      {
        // Updating key of v
        key[v] = weight;
        min_heap.push_back(edge(v, key[v]));
        parent[v] = u;
      }
    }
	}

	// Print edges of MST using parent array
	for (int i = 1; i < num_nodes; ++i)
	{
		mst_edge_list.push_back(Vedge(nodes[i].location, nodes[parent[i]].location, 0.005*glm::distance(nodes[i].location, nodes[parent[i]].location)));
		printf("%d - %d\n", parent[i], i);			//<----- these are the edges to highlight - but we need the rest, too
	}

}







//--------------------

void graph::reconstruct()
{

	nodes.clear();
	verticies.clear();
	all_edge_list.clear();

	nodes.resize(num_nodes);

	//random number generation
  std::random_device rd;
  std::mt19937 mt(rd());

	// std::uniform_real_distribution<float> distx(0.0f,511.0f);
	// std::uniform_real_distribution<float> disty(0.0f,255.0f);
	// std::uniform_real_distribution<float> distz(0.0f,255.0f); //input values














			// // function template generate_canonical
			//   template<class RealType, size_t bits, class URBG>
			//   RealType generate_canonical(URBG& g);
			//   // class template uniform_int_distribution
			//   template<class IntType = int>
			//   class uniform_int_distribution;
			//   // class template uniform_real_distribution
			//   template<class RealType = double>
			//   class uniform_real_distribution;
			//   // class bernoulli_distribution
			//   class bernoulli_distribution;
			//   // class template binomial_distribution
			//   template<class IntType = int>
			//   class binomial_distribution;
			//   // class template geometric_distribution
			//   template<class IntType = int>
			//   class geometric_distribution;
			//   // class template negative_binomial_distribution
			//   template<class IntType = int>
			//   class negative_binomial_distribution;
			//   // class template poisson_distribution
			//   template<class IntType = int>
			//   class poisson_distribution;
			//   // class template exponential_distribution
			//   template<class RealType = double>
			//   class exponential_distribution;
			//   // class template gamma_distribution
			//   template<class RealType = double>
			//   class gamma_distribution;
			//   // class template weibull_distribution
			//   template<class RealType = double>
			//   class weibull_distribution;
			//   // class template extreme_value_distribution
			//   template<class RealType = double>
			//   class extreme_value_distribution;
			//   // class template normal_distribution
			//   template<class RealType = double>
			//   class normal_distribution;
			//   // class template lognormal_distribution
			//   template<class RealType = double>
			//   class lognormal_distribution;
			//   // class template chi_squared_distribution
			//   template<class RealType = double>
			//   class chi_squared_distribution;
			//   // class template cauchy_distribution
			//   template<class RealType = double>
			//   class cauchy_distribution;
			//   // class template fisher_f_distribution
			//   template<class RealType = double>
			//   class fisher_f_distribution;
			//   // class template student_t_distribution
			//   template<class RealType = double>
			//   class student_t_distribution;
			//   // class template discrete_distribution
			//   template<class IntType = int>
			//   class discrete_distribution;
			//   // class template piecewise_constant_distribution
			//   template<class RealType = double>
			//   class piecewise_constant_distribution;
			//   // class template piecewise_linear_distribution
			//   template<class RealType = double>
			//   class piecewise_linear_distribution;















	std::uniform_real_distribution<float> dist(-1.3f,1.3f);
	std::uniform_real_distribution<float> scale(0.3f,2.0f);





	PerlinNoise p;

	float s = 0.4;


	for(int i = 0; i < num_nodes; i++)
	{

		glm::vec3 test = 50.0f*scale(mt)*glm::normalize(glm::vec3(dist(mt), dist(mt), dist(mt))) + glm::vec3(255,127,127);

		while(p.noise(s*test.x, s*test.y, s*test.z) * p.noise(s*test.x/2.0, s*test.y/2.0, s*test.z/2.0) * p.noise(s*test.x/4.0, s*test.y/4.0, s*test.z/4.0) < 0.3)
		{
			test = 50.0f*glm::normalize(glm::vec3(dist(mt), dist(mt), dist(mt))) + glm::vec3(255,127,127);
		}

		nodes[i].location = test;
		nodes[i].location.x = 1.4f * (nodes[i].location.x - 255) + 255;

		// cout << nodes[i].location.x << " " << nodes[i].location.y << " " << nodes[i].location.z << endl;

		verticies.push_back(nodes[i].location);

		cout << "\ri" << std::flush;
	}







	for(int i = 0; i < edge_log.size(); i++)
	{
		// edge temp(edge_log[i].second, 0.003f*edge_log[i].weight*glm::distance(nodes[edge_log[i].first].location, nodes[edge_log[i].second].location));	//order is destination, weight
		edge temp(edge_log[i].second, 0.01f*glm::distance(nodes[edge_log[i].first].location, nodes[edge_log[i].second].location));	//order is destination, weight


		nodes[edge_log[i].first].add_link(temp);

		temp.destination = edge_log[i].first;
		nodes[edge_log[i].second].add_link(temp);


		all_edge_list.push_back(Vedge(nodes[edge_log[i].first].location, nodes[edge_log[i].second].location, temp.weight));
	}

	cout << "reconstructed " << edge_log.size() << " edges" << endl << endl;

}





void graph::output_for_voraldo()
{
	std::ofstream file;
	file.open (std::string("V.txt").c_str());

	float scale = 3.0;

	int total = all_edge_list.size()+mst_edge_list.size()+verticies.size();

	file << "cout << \"writing " << total << " voraldo commands\" << endl;" << endl;

	int count = 0;

	for(auto x : all_edge_list)
	{
			file << "main_block->draw_cylinder(glm::vec3(" << x.first.x << ", " << x.first.y << ", " << x.first.z << "),   glm::vec3(" << x.second.x << ", " << x.second.y << ", " << x.second.z << "), " << /*x.weight/scale*/ 0.618f << ", all_edge_material);" << endl;

			file << "cout << \"\\r" << count << " of " << total << " voraldo commands\";" << endl;

			count++;
	}

	file << endl << endl;

	for(auto x : mst_edge_list)
	{
			file << "main_block->draw_cylinder(glm::vec3(" << x.first.x << ", " << x.first.y << ", " << x.first.z << "),   glm::vec3(" << x.second.x << ", " << x.second.y << ", " << x.second.z << "), " << /*x.weight/scale*/ 1.0f << ", mst_edge_material);" << endl;

			file << "cout << \"\\r" << count << " of " << total << " voraldo commands\";" << endl;

			count++;
	}

	file << endl << endl;

	for(auto x : verticies)
	{
		file <<"main_block->draw_sphere(glm::vec3("<< x.x << ", " << x.y << ", " << x.z << "), 2.0, vertex_material);" << endl;

		file << "cout << \"\\r" << count << " of " << total << " voraldo commands\";" << endl;

		count++;
	}
}










//--------------------

	//expected file format is as follows:
			//	<num_nodes> <num_connections>
			//	<first> <second> <weight>
			//	<first> <second> <weight>
			//	<first> <second> <weight>
			//		...  //num_connections many of these

//--------------------

void graph::save_to_file(std::string filename)
{
	//put the edge log into a file
	std::ofstream file;
	file.open (filename.c_str());

	cout << "writing " << edge_log.size() << " edges to a file, between " << num_nodes << " nodes" << endl;
	file << std::setw(4) << num_nodes << "  " << std::setw(4) << num_connections << endl;

	for (int i = 0; i < edge_log.size(); i++)
	{
		file << std::setw(4) << edge_log[i].first << " " << std::setw(4) << edge_log[i].second << " " << std::setw(4) << edge_log[i].weight << endl;
	}

	file.close();
}



//--------------------

void graph::load_from_file(std::string filename)
{
	//get the edge log from a file
	edge_log.clear();


	std::ifstream file (filename.c_str());

 	if (file.is_open())
	{
		//read in num_nodes, num_connections
		file >> num_nodes >> num_connections;

		cout << "loading " << num_nodes << " nodes, " << num_connections << " connections" << endl;

		int first, second;
		float weight;

		for(int i = 0; i < num_connections; i++)
		{
			file >> first >> second >> weight;
			add_edge_to_log(first, second, weight);
		}

	}

	// reconstruct the graph, based on this edge_log
	reconstruct();

}
