// #include "graph.hpp"
#include "space.hpp"

int main(int argc, char const *argv[])
{

	// graph g(atoi(argv[1]), atoi(argv[2]));
	//
	// g.show_graph();
	//
	//
	// g.save_to_file(std::string("save.txt"));
	// g.load_from_file(std::string("save.txt"));
	//
	//
	// g.show_graph();
	// g.show_mst();
	//
	//
	// g.output_for_voraldo();


	star_system s(atoi(argv[1]), atoi(argv[2]));

	s.voraldo_output();


	return 0;

}
