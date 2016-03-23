include_directories(${PROJECT_SOURCE_DIR}/tpl)

#executables

#1.TplDB
add_library(db_obj OBJECT tpl_db.cpp)
add_executable(tpl_db_test $<TARGET_OBJECTS:db_obj> tpl_db_test.cpp)
target_link_libraries(tpl_db_test bookshelf ${Boost_LIBRARIES})

#2.TplConfig
add_library(config_obj OBJECT tpl_config.cpp)
add_executable(tpl_config_test $<TARGET_OBJECTS:config_obj> tpl_config_test.cpp)
target_link_libraries(tpl_config_test ${Boost_LIBRARIES})

#3.TplNetModel
add_library(net_model_obj OBJECT tpl_standard_net_model.cpp)
set(TPL_STANDARD_NET_MODEL_TEST_SRC
        $<TARGET_OBJECTS:db_obj>
        $<TARGET_OBJECTS:net_model_obj>
        tpl_standard_net_model_test.cpp)
add_executable(tpl_net_model_test ${TPL_STANDARD_NET_MODEL_TEST_SRC})
target_link_libraries(tpl_net_model_test bookshelf ${Boost_LIBRARIES})

#3.TplStandardAlgorithm
#set(TPL_STANDARD_ALGORITHM_TEST_SRC
#    tpl_config.cpp                 tpl_db.cpp
#    tpl_standard_net_model.cpp     tpl_standard_net_force_model.cpp
#    tpl_standard_thermal_model.cpp tpl_standard_thermal_force_model.cpp
#    tpl_standard_algorithm.cpp     tpl_standard_algorithm_test.cpp)
#
#add_executable(tpl_standard_algorithm_test ${TPL_STANDARD_ALGORITHM_TEST_SRC})
#target_link_libraries(tpl_standard_algorithm_test bookshelf ${Boost_LIBRARIES})
