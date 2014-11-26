
#include "../Headers/grid.h"
#include "../Headers/problem_object.h"
#include <map.h>
#include <claim.h>
#include <leebase.h>
#include <leeoriginal.h>

using std::cerr;
using std::cout;
using std::endl;

enum Algorithm {
    LEE2BIT, LEE3BIT, KORN, HADLOCK, RUBEN, LEE
};
Algorithm resolve_algorithm(string);


int main(int argc,char* argv[]) {

	// DO NOT CHANGE THIS SECTION OF CODE
	if(argc < 2) { 
		cout << "Usage: ./grid_router <test_file> [<algorithm> <bi-directional> <intersection> <korn modifier>]" << endl;
        cout << "Optional arugments: " << endl;
        cout << "<test_file> {string}" << endl;
        cout << "<algorithm> {lee | lee2bit | lee3bit | ruben | korn | hadlock}" << endl;
        cout << "<bi-directional> {1 | 0}" << endl;
        cout << "<intersection> {1 | 0}" << endl;
        cout << "<korn modifier> {float > 0}" << endl;
		exit(1);
	}
	Utilities::ProblemObject* first_problem = new Utilities::ProblemObject(std::string(argv[1]));
	// EDIT FROM HERE DOWN

    // Defaults for our variables
    // they get altered in the switch below
    double korn_modifier = 1.5;
    bool bi_directional = false;
    bool intersections = false;
    Algorithm a_type = LEE;
    string file = "../Tests/debug.json";
    LeeOriginal algorithm = LeeOriginal();

    switch (argc) {
        case 7:
            //claim("We have 7 args", kNote);
        case 6:
            //claim("We have 6 args", kNote);
            //claim("Setting korn modifier", kDebug);
            korn_modifier = atof(argv[5]);
        case 5:
            //claim("We have 5 args", kNote);
            //claim("Setting intersection capability", kDebug);
            intersections = atoi(argv[4]) == 1 ? true : false;
        case 4:
            //claim("We have 4 args", kNote);
            //claim("Setting bi-directional capability", kDebug);
            bi_directional = (atoi(argv[3])==1) ? true : false;
        case 3:
            //claim("We have 3 args", kNote);
            //claim("Setting algorithm type", kDebug);
            a_type = resolve_algorithm(argv[2]);
        case 2:
        default:
            //claim("We have 2 args", kNote);
            //claim("Setting the file", kDebug);
            file = argv[1];
            break;
    }
    if(intersections) {
        algorithm.enable_intersections();
        claim("Enabling intersections", kDebug);
    }

    claim("Working on problem: " + first_problem->get_name(), kNote);
    // Initializing a new map to project our solution on
    Map map = Map(first_problem->get_width(), first_problem->get_height());
    map.set_blockages(first_problem->get_blockers());
    map.set_sources_and_sinks(first_problem->get_connections());

    // Set the map of our algorithm
    algorithm.set_map(&map);

    while(map.get_size_of_routes() > 0) {
        // Get the next route
        Route work = map.get_next_route();
        claim("Working on a new connection: " + map.connection_to_string(work), kNote);
        // Solve the route
        algorithm.start(work);
        claim("=========================", kDebug);
        // Clear the map out
        map.zero_map();
    }
    claim("PathBack: " + algorithm.get_path_back(), kDebug);
    map.print_map();

	//Create your problem map object (in our example, we use a simple grid, you should create your own)
	//Utilities::Grid g(first_problem->get_width(), first_problem->get_height());
	//Utilities::Grid g(first_problem);

	/*
	Note: we do not take into account the connections or blockers that exist in the Project Object
	You should be accouting for these in your problem map objects (you should not be using Grid). You
	should be using both the Node and Edge classes as the background for whatever problem map object
	you create.
	*/

	/*
	Run your algorithm after creating your problem map object. You should be returning from your algorithm 
	either a Path or a Netlist

	Path: a series of straight line segments, with a single source and a single sink
	Netlist: a series of stright line segments, with a single source and more than one sink
	*/

	//Note, we create random paths just as an example of how to create paths, netlists are created similarly
	/*
	vector<Path*> paths;
	srand(time(NULL));
	int number_paths = first_problem->get_connections().size();
	cout << "Creating " << number_paths << " paths...";
	for (int i = 0;i < number_paths;i++) {
		Path* new_path = new Path();
		int x = rand() % first_problem->get_width();
		int y = rand() % first_problem->get_height();
		int path_length = 1+rand()%10;
		for (unsigned j = 0;j < path_length;j++) {
			bool direction = rand()%2;
			Point head(x,y);
			direction?x+=1:y+=1;
			Point tail(x,y);
			PathSegment* new_segment = new PathSegment(head,tail);
			new_path->add_segment(new_segment);
		}
		paths.push_back(new_path);
	}
	cout << "Completed." << endl;
	//vector<Path*> paths = g.test_algorithm();

	//Print the paths/netlists that you return from your algorithm
	for(unsigned i = 0; i < paths.size(); i++) {
		cout << "Path " << i << ": ";
		paths.at(i)->print();
		Path* temp = paths.at(i);
		delete temp;
	}

	paths.clear();
	*/

	delete first_problem;

	return 0;
}

Algorithm resolve_algorithm(string a) {
    if (a.compare("ruben") == 0) {
        //return RUBEN;
    }
    if (a.compare("korn") == 0) {
        //return KORN;
    }
    if (a.compare("hadlock") == 0) {
        //return HADLOCK;
    }
    if (a.compare("lee3bit") == 0) {
        //return LEE3BIT;
    }
    if (a.compare("lee2bit") == 0) {
        //return LEE2BIT;
    }
    if (a.compare("lee") == 0) {
        return LEE;
    }
    return LEE;
}
