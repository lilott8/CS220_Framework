set(LIB_SOURCE
claim.cc
edge.cc
grid.cc
netlist.cc
node.cc
path.cc
pathsegment.cc
segmentgroup.cc
problem_object.cc
map.cc
leenode.cc
leebase.cc
leeoriginal.cc
lee3bit.cc
ruben.cc
hadlock.cc
)

add_library(SOURCE_LIB ${LIB_SOURCE})

set(MAIN_SRC
    main.cc
)

add_executable(grid_test ${MAIN_SRC})
target_link_libraries(grid_test SOURCE_LIB)
target_link_libraries(grid_test JSON_LIB)