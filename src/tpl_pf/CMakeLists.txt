#executables
#1.TplAlgorithm
set(TPL_ALGORITHM_TEST_SRC_PF tpl_algorithm_test_pf.cpp tpl_algorithm_pf.cpp)
add_executable(tpl_algorithm_test_pf ${TPL_ALGORITHM_TEST_SRC_PF})
target_link_libraries(tpl_algorithm_test_pf tpl bookshelf ${Boost_LIBRARIES})
