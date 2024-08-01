ADD_EXECUTABLE(baseline baseline_test.cpp)
TARGET_LINK_LIBRARIES(baseline graph_mining_without_thread_local)

# ADD_EXECUTABLE(temporal_test temporal_test.cpp)
# TARGET_LINK_LIBRARIES(temporal_test graph_mining)

ADD_EXECUTABLE(unit_test unitTest.cpp)
TARGET_LINK_LIBRARIES(unit_test graph_mining_without_thread_local)

ADD_EXECUTABLE(test test.cpp)
TARGET_LINK_LIBRARIES(test graph_mining_without_thread_local)

ADD_EXECUTABLE(memory tcs_memory.cpp)
TARGET_LINK_LIBRARIES(memory graph_mining_without_thread_local)