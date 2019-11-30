#include <stdio.h>
#include <vector>
#include <random>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;
using std::cin;



//--------------------

class edge
{
public:
	edge(int destination, float weight) { destination = destination; weight = weight; }

	int destination;	//id of target node
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

private:
	vector<edge> linkages;

	float value;

	//other information?

};

//--------------------

class graph
{
public:
	graph(int num, int connections) { nodes.clear(); nodes.resize(num); make_connections(connections); }

	void make_connections(int how_many);
	void add_edge(int first, int second, float weight);

	void show();

private:
	vector<node> nodes;

};

//--------------------
void graph::add_edge(int first, int second, float weight)
{

	//I'm kind of thinking about making a random chance for one or the other to not get added

	edge temp(second, weight);	//order is destination, weight
	nodes[first].add_link(temp);

	temp.destination = first;
	nodes[second].add_link(temp);
}

//--------------------

void graph::make_connections(int how_many)
{
	//random number generation
  std::random_device rd;
  std::mt19937 mt(rd());

	if(nodes.size() != 0)
	{

		std::uniform_real_distribution<float> dist1(0.3,9.8); //input values
		std::uniform_int_distribution<int> dist2(0,nodes.size()-1);  //nodes

		for(int i = 0; i < how_many; i++)
		{

			add_edge(dist2(mt), dist2(mt), dist1(mt));

		}
	}


}


// edge get_link(int num) { return linkages[num]; }


void graph::show(){

    if(!nodes.empty())
		{
        for(int i = 0; i < nodes.size(); i++)
        {
            cout << "node " << i << " has links to: ";
            if(!nodes[i].alone())
            {
                for(int j = 0; j < nodes[i].size(); j++)
                {
										cout << nodes[i].get_link(j).destination << "(" << nodes[i].get_link(j).weight << ") ";
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
}
