#include_directories(
#    ${PROJECT_SOURCE_DIR}/src/bookshelf
#    ${PROJECT_SOURCE_DIR}/src/tpl
#    ${Boost_INCLUDE_DIRS}
#    )

#libraries
set(TPL_LIB_SRC tpl_circuit.cpp tpl_algorithm.cpp)
add_library(tpl SHARED ${TPL_LIB_SRC})
set_target_properties(tpl PROPERTIES VERSION 0.1 SOVERSION 1)

#executables
#1.TplCircuit
set(TPL_CIRCUIT_TEST_SRC tpl_circuit_test.cpp)
add_executable(tpl_circuit_test ${TPL_CIRCUIT_TEST_SRC})
target_link_libraries(tpl_circuit_test   tpl bookshelf ${Boost_LIBRARIES})
#2.TplAlgorithm
set(TPL_ALGORITHM_TEST_SRC tpl_algorithm_test.cpp)
add_executable(tpl_algorithm_test ${TPL_ALGORITHM_TEST_SRC})
target_link_libraries(tpl_algorithm_test tpl bookshelf ${Boost_LIBRARIES})
