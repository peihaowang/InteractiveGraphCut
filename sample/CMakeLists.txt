add_executable(GRAPH_CUT_EXE
    main.cpp
)

set_target_properties(GRAPH_CUT_EXE PROPERTIES OUTPUT_NAME GraphCutter)
set_target_properties(GRAPH_CUT_EXE PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# The modules should placed in specified order because of the interdependencies
target_link_libraries(GRAPH_CUT_EXE
	GRAPH_CUT_STATIC
	${3RDPARTY_LIBRARIES}
)

# Installation
install(TARGETS GRAPH_CUT_EXE
    RUNTIME DESTINATION bin
)
