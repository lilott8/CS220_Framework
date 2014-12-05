
FLAGS=-std=c++11 #added
SRC=$(filter-out %main.cc, Source/*)
OBJ=claim.o edge.o grid.o netlist.o node.o path.o pathsegment.o problem_object.o segmentgroup.o leenode.o leeoriginal.o leebase.o ruben.o hadlock.o lee3bit.o map.o

vpath %.cc Source/

all: $(OBJ) main.cc
	g++ $(FLAGS) -o grid_router $^ Utilities/JSON_parser/json_parser.so 

test: all
	./grid_router Tests/test_sample.json
	
%.o: %.cc
	g++ $(FLAGS) -c $^

cleanup:
	rm -f *.o

clean: cleanup
	rm -f grid_router