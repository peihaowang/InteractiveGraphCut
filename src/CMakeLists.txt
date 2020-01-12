
set(SOURCE_FILE_PATH
	Graph.cpp
	MaxFlow.cpp
	GraphCutImpl.cpp
	GraphCut.cpp
)

###############################################################################

add_library(GRAPH_CUT_SHARED
	SHARED
    ${SOURCE_FILE_PATH}
)

set_target_properties(GRAPH_CUT_SHARED PROPERTIES OUTPUT_NAME GraphCut)
set_target_properties(GRAPH_CUT_SHARED PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# The modules should placed in specified order because of the interdependencies
target_link_libraries(GRAPH_CUT_SHARED
	${3RDPARTY_LIBRARIES}
)

###############################################################################

add_library(GRAPH_CUT_STATIC
	STATIC
    ${SOURCE_FILE_PATH}
)

set_target_properties(GRAPH_CUT_STATIC PROPERTIES OUTPUT_NAME GraphCut)
set_target_properties(GRAPH_CUT_STATIC PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# The modules should placed in specified order because of the interdependencies
target_link_libraries(GRAPH_CUT_STATIC
	${3RDPARTY_LIBRARIES}
)

# Installation

install(TARGETS GRAPH_CUT_SHARED GRAPH_CUT_STATIC
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)

file(GLOB PATH_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES ${PATH_HEADER_FILES} DESTINATION include)
