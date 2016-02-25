#libraries
set(TPL_LIB_SRC tpl_circuit.cpp)
add_library(tpl SHARED ${TPL_LIB_SRC})
set_target_properties(tpl PROPERTIES VERSION 0.1 SOVERSION 1)

#executables
#1.TplCircuit
set(TPL_CIRCUIT_TEST_SRC tpl_circuit_test.cpp)
add_executable(tpl_circuit_test ${TPL_CIRCUIT_TEST_SRC})
target_link_libraries(tpl_circuit_test tpl bookshelf ${Boost_LIBRARIES})
