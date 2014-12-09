
#include "../Headers/grid.h"
#include "../Headers/map.h"
#include "../Headers/claim.h"
#include "../Headers/leebase.h"
#include "../Headers/leeoriginal.h"
#include "../Headers/lee3bit.h"
#include "../Headers/ruben.h"
#include "../Headers/hadlock.h"
#include "../Headers/lee2bit.h"

#include <memory>

using std::cerr;
using std::cout;
using std::endl;

enum Algorithm {
    LEE2BIT, LEE3BIT, KORN, HADLOCK, RUBEN, LEE
};
Algorithm resolve_algorithm(string);
unique_ptr<LeeBase> child_factory(Algorithm);

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
    double korn_modifier = 1;
    bool bi_directional = false;
    bool intersections = false;
    Algorithm a_type = LEE;
    string file = "../Tests/debug.json";

    switch (argc) {
        case 7:
            //claim("We have 7 args", kNote);
        case 6:
            //claim("We have 6 args", kNote);
            //claim("Setting korn modifier", kDebug);
            korn_modifier = atof(argv[5]);
            if(korn_modifier < 1.0) {
                korn_modifier = 1.0;
            }
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
    unique_ptr<LeeBase> algorithm = child_factory(a_type);

    if(intersections) {
        algorithm->enable_intersections();
        claim("Enabling intersections", kDebug);
    }

    if (bi_directional) {
        algorithm->enable_bi_direction();
        claim("Enabling bi-directional search", kDebug);
    }

    if(a_type == RUBEN || a_type == KORN) {
        algorithm->set_korn(korn_modifier);
        claim("Enabling a Korn modifier of: " + to_string(korn_modifier), kDebug);
    }

    claim("Working on problem: " + first_problem->get_name(), kNote);
    // Initializing a new map to project our solution on
    Map map = Map(first_problem->get_width(), first_problem->get_height());
    map.set_blockages(first_problem->get_blockers());
    map.set_sources_and_sinks(first_problem->get_connections());

    // Set the map of our algorithm
    algorithm->set_map(&map);
    int x = 0;
    //map.print_map();

    while(map.get_size_of_routes() > 0) {
        // Get the next route
        Route work = map.get_next_route();
        claim("Working on a new connection: " + map.connection_to_string(work), kNote);
        // Solve the route
        algorithm->start(work);
        claim("=========================", kDebug);
        //map.print_map();
        // Clear the map out
        map.zero_map();
        algorithm->get_path_back(x)->print();
        map.print_map();
        x++;
    }

	delete first_problem;

	return 0;
}

unique_ptr<LeeBase> child_factory(Algorithm a) {
    switch(a) {
        default:
        case LEE:
            return unique_ptr<LeeOriginal>(new LeeOriginal());
        case LEE2BIT: return unique_ptr<Lee2Bit> (new Lee2Bit());
        case LEE3BIT: return unique_ptr<Lee3Bit> (new Lee3Bit());
        case RUBEN: return unique_ptr<Ruben> (new Ruben());
        case HADLOCK:
            return unique_ptr<Hadlock>(new Hadlock());
    }
}

Algorithm resolve_algorithm(string a) {
    if (a.compare("ruben") == 0) {
        claim("Using ruben's/korn's algorithm", kDebug);
        return RUBEN;
    }
    if (a.compare("korn") == 0) {
        claim("Using ruben's/korn's algorithm", kDebug);
        return RUBEN;
    }
    if (a.compare("hadlock") == 0) {
        claim("Using hadlock's algorithm", kDebug);
        return HADLOCK;
    }
    if (a.compare("lee3bit") == 0) {
        claim("Using Lee3bit algorithm", kDebug);
        return LEE3BIT;
    }
    if (a.compare("lee2bit") == 0) {
        claim("Using Lee2bit algorithm", kDebug);
        return LEE2BIT;
    }
    if (a.compare("lee") == 0) {
        claim("Using Lee's algorithm", kDebug);
        return LEE;
    }
    return LEE;
}
