#executables
#1.TplAlgorithm
set(TPL_ALGORITHM_TEST_SRC_GY tpl_algorithm_test_gy.cpp tpl_algorithm_gy.cpp)
add_executable(tpl_algorithm_test_gy ${TPL_ALGORITHM_TEST_SRC_GY})
target_link_libraries(tpl_algorithm_test_gy tpl bookshelf ${Boost_LIBRARIES})
