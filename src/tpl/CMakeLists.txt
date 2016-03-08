include_directories(${PROJECT_SOURCE_DIR}/tpl)

#executables

#1.TplDB
set(TPL_DB_TEST_SRC tpl_db.cpp tpl_db_test.cpp)
add_executable(tpl_db_test ${TPL_DB_TEST_SRC})
target_link_libraries(tpl_db_test bookshelf ${Boost_LIBRARIES})

#2.TplStandardAlgorithm
set(TPL_STANDARD_ALGORITHM_TEST_SRC tpl_db.cpp tpl_standard_algorithm.cpp tpl_standard_algorithm_test.cpp)
add_executable(tpl_standard_algorithm_test ${TPL_STANDARD_ALGORITHM_TEST_SRC})
target_link_libraries(tpl_standard_algorithm_test bookshelf ${Boost_LIBRARIES})

#3.TplGYAlgorithm
set(TPL_GY_ALGORITHM_TEST_SRC tpl_db.cpp tpl_standard_algorithm.cpp tpl_gy_algorithm.cpp tpl_gy_algorithm_test.cpp)
add_executable(tpl_gy_algorithm_test ${TPL_GY_ALGORITHM_TEST_SRC})
target_link_libraries(tpl_gy_algorithm_test       bookshelf ${Boost_LIBRARIES})
