#libraries
#set(TPL_LIB_SRC tpl_circuit.cpp)
#add_library(tpl SHARED ${TPL_LIB_SRC})
#set_target_properties(tpl PROPERTIES VERSION 0.1 SOVERSION 1)

include_directories(${PROJECT_SOURCE_DIR}/src/tpl)
#executables
#1.TplCircuit
set(TPL_CIRCUIT_TEST_SRC tpl_circuit.cpp tpl_circuit_test.cpp)
add_executable(tpl_circuit_test ${TPL_CIRCUIT_TEST_SRC})
target_link_libraries(tpl_circuit_test      bookshelf ${Boost_LIBRARIES})
#2.TplAlgorithmPF
set(TPL_ALGORITHM_PF_TEST_SRC tpl_circuit.cpp tpl_algorithm_pf.cpp tpl_algorithm_pf_test.cpp)
add_executable(tpl_algorithm_pf_test ${TPL_ALGORITHM_PF_TEST_SRC})
target_link_libraries(tpl_algorithm_pf_test bookshelf ${Boost_LIBRARIES})
#3.TplAlgorithmGY
set(TPL_ALGORITHM_GY_TEST_SRC tpl_circuit.cpp tpl_algorithm_gy.cpp tpl_algorithm_gy_test.cpp)
add_executable(tpl_algorithm_gy_test ${TPL_ALGORITHM_GY_TEST_SRC})
target_link_libraries(tpl_algorithm_gy_test bookshelf ${Boost_LIBRARIES})
