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

	std::uniform_real_distribution<float> distx(0.3f,256.0f); //input values
	std::uniform_real_distribution<float> disty(0.3f,256.0f);
	std::uniform_real_distribution<float> distz(0.3f,512.0f);

	for(int i = 0; i < num_nodes; i++)
	{
		nodes[i].location = glm::vec3(distx(mt), disty(mt), distz(mt));
		verticies.push_back(nodes[i].location);
	}

	for(int i = 0; i < edge_log.size(); i++)
	{
		edge temp(edge_log[i].second, 0.003f*edge_log[i].weight*glm::distance(nodes[edge_log[i].first].location, nodes[edge_log[i].second].location));	//order is destination, weight
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

	for(auto x : all_edge_list)
		file << "glm::vec3(" << x.first.x << ", " << x.first.y << ", " << x.first.z << "),   glm::vec3(" << x.second.x << ", " << x.second.y << ", " << x.second.z << "), " << x.weight << endl;

	file << endl << endl;

	for(auto x : mst_edge_list)
		file <<"glm::vec3("<< x.first.x << ", " << x.first.y << ", " << x.first.z << ")  glm::vec3(" << x.second.x << ", " << x.second.y << ", " << x.second.z << "), " << x.weight << endl;

	file << endl << endl;

	for(auto x : verticies)
		file <<"glm::vec3("<< x.x << ", " << x.y << ", " << x.z << ")" << endl;
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
