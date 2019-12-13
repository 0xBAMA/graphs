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

//--entity------------------

	typedef enum type_t
	{
		moon = 0,
		planet1 = 1,
		planet2 = 2,
		planet3 = 3,
		planet4 = 4,
		planet5 = 5,
		station = 6
	} type;

	class entity
	{
	public:
		type t;
		glm::vec3 location;
	};

//--edge------------------

	class route
	{
	public:
		int first;		//the star where it starts
		int second;		//the star where it ends

		float weight;	//distance between stars
	};

//--node------------------

	class star
	{
	public:
		int id;

		void add_random_entity();

		vector<route> linkages;
		vector<entity> entities;

		glm::vec3 location;
		float value;

	private:

	};

	void star::add_random_entity()
	{
		entity temp;

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> offset(-15.0,15.0);
		std::uniform_int_distribution<int> sel(0,6);



		temp.location = location + glm::vec3(offset(mt), offset(mt), offset(mt));
		temp.t = (type)sel(mt);

		cout << id << " is adding an entity of type " << temp.t << " at " << temp.location.x << " " << temp.location.y << " " << temp.location.z << endl;

		entities.push_back(temp);


	}


//--------------------

	class star_system
	{
	public:
		star_system(int num_stars, int num_entities);

		void voraldo_output();

	private:
		vector<star> stars;
	};


	star_system::star_system(int num_stars, int num_entities)
	{
		cout << "intializing with " << num_stars << " stars and " << num_entities << " other entities" << endl;

		stars.resize(num_stars);

		//random number generation
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<float> distx(25.0f,480.0f);
		std::uniform_real_distribution<float> distyz(25.0f,230.0f);


		//initialize all ids
		for(int i = 0; i < num_stars; i++)
		{
			stars[i].id = i;
			stars[i].location = glm::vec3(distx(mt), distyz(mt), distyz(mt));
		}

		std::uniform_int_distribution<int> sel(0,num_stars-1);  //nodes

		//populate list of random entities
		for(int i = 0; i < num_entities; i++)
		{
			stars[sel(mt)].add_random_entity();
		}


		//add routes to all other stars in the system
		for(int i = 0; i < num_stars; i++)
		{
			route temp;
			temp.first = i;	//all the routes for stars[i] will start at i

			for(int j = 0; j < num_stars; j++)
			{
				temp.second = j;	//maintaining i in first, j in second (j ranges across all)
				temp.weight = glm::distance(stars[i].location, stars[j].location);

				if(i!=j)	//don't want  a link to itself
					stars[i].linkages.push_back(temp);
			}
		}
	}


void star_system::voraldo_output()
{
	std::ofstream file;
	file.open (std::string("V.txt").c_str());

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> rad(1.4,3.0);
	std::uniform_real_distribution<float> rad2(1.1,2.3);

	//add links
	for(int i = 0; i < stars.size(); i++)
	{
		for(int j = 0; j < stars[i].linkages.size(); j++)
		{
			if(stars[i].linkages[j].second > stars[i].linkages[j].first)	//this is a good optimization (fix the naive duplicaitons)
			{
				file << "main_block->draw_cylinder(glm::vec3(" << stars[stars[i].linkages[j].first].location.x << ", " << stars[stars[i].linkages[j].first].location.y << ", " << stars[stars[i].linkages[j].first].location.z << "),   glm::vec3(" << stars[stars[i].linkages[j].second].location.x << ", " << stars[stars[i].linkages[j].second].location.y << ", " << stars[stars[i].linkages[j].second].location.z << "), " << /*x.weight/scale*/ 0.618f << ", route_material);" << endl;
			}
		}
	}

	//add stars
	for(int i = 0; i < stars.size(); i++)
	{
		file <<"main_block->draw_sphere(glm::vec3("<< stars[i].location.x << ", " << stars[i].location.y << ", " << stars[i].location.z << "), "<< rad(mt) <<", star_material);" << endl;
	}

	//add the other shit
	for(int i = 0; i < stars.size(); i++)
	{
		for(int j = 0; j < stars[i].entities.size(); j++)
		{
			switch(stars[i].entities[j].t)
			{
				case moon:
					file <<"main_block->draw_sphere(glm::vec3("<< stars[i].entities[j].location.x << ", " << stars[i].entities[j].location.y << ", " << stars[i].entities[j].location.z << "), "<< rad2(mt) <<", moon_material);" << endl;
					break;

				case planet1:
					file <<"main_block->draw_sphere(glm::vec3("<< stars[i].entities[j].location.x << ", " << stars[i].entities[j].location.y << ", " << stars[i].entities[j].location.z << "), "<< rad2(mt) <<", planet1_material);" << endl;
					break;

				case planet2:
					file <<"main_block->draw_sphere(glm::vec3("<< stars[i].entities[j].location.x << ", " << stars[i].entities[j].location.y << ", " << stars[i].entities[j].location.z << "), "<< rad2(mt) <<", planet2_material);" << endl;
					break;

				case planet3:
					file <<"main_block->draw_sphere(glm::vec3("<< stars[i].entities[j].location.x << ", " << stars[i].entities[j].location.y << ", " << stars[i].entities[j].location.z << "), "<< rad2(mt) <<", planet3_material);" << endl;
					break;

				case planet4:
					file <<"main_block->draw_sphere(glm::vec3("<< stars[i].entities[j].location.x << ", " << stars[i].entities[j].location.y << ", " << stars[i].entities[j].location.z << "), "<< rad2(mt) <<", planet4_material);" << endl;
					break;

				case planet5:
					file <<"main_block->draw_sphere(glm::vec3("<< stars[i].entities[j].location.x << ", " << stars[i].entities[j].location.y << ", " << stars[i].entities[j].location.z << "), "<< rad2(mt) <<", planet5_material);" << endl;
					break;

				case station:
					file <<"main_block->draw_sphere(glm::vec3("<< stars[i].entities[j].location.x << ", " << stars[i].entities[j].location.y << ", " << stars[i].entities[j].location.z << "), "<< rad2(mt) <<", station_material);" << endl;
					break;

			}
		}
	}
}
