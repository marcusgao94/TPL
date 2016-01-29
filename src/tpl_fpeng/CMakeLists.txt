#library
include_directories(${PROJECT_SOURCE_DIR}/src/bookshelf)

set(TPL_LIB_SRC tpl_circuit.cpp tpl_algorithm.cpp)
add_library(tpl_fpeng SHARED ${TPL_LIB_SRC})
set_target_properties(tpl_fpeng PROPERTIES VERSION 0.1 SOVERSION 1)

#executables
include_directories(${PROJECT_SOURCE_DIR}/src/tpl_fpeng)
#link_directories(${PROJECT_BINARY_DIR}/lib)
#targets
set(TPL_CIRCUIT_TEST_SRC tpl_circuit_test.cpp)
add_executable(tpl_circuit_test ${TPL_CIRCUIT_TEST_SRC})
target_link_libraries(tpl_circuit_test tpl_fpeng bookshelf boost_filesystem)
