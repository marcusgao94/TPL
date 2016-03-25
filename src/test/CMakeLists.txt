add_executable(assert_free_nodes_order $<TARGET_OBJECTS:db_obj> assert_free_nodes_order.cpp)
target_link_libraries(assert_free_nodes_order bookshelf ${Boost_LIBRARIES})