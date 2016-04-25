include_directories(${PROJECT_SOURCE_DIR}/tpl)
include_directories(${PROJECT_SOURCE_DIR}/tools/rippledp)
include_directories(${PROJECT_SOURCE_DIR}/tools/def)
include_directories(${PROJECT_SOURCE_DIR}/tools/lef)

#1.TplDB
add_executable(test_tpl_db $<TARGET_OBJECTS:db_obj> test_tpl_db.cpp)
target_link_libraries(test_tpl_db bookshelf ${Boost_LIBRARIES})

#2.TplConfig
add_executable(test_tpl_config $<TARGET_OBJECTS:config_obj> test_tpl_config.cpp)
#target_link_libraries(test_tpl_config ${Boost_LIBRARIES})

#3.TplStandardNetModel
set(TPL_STANDARD_NET_MODEL_SRC
		$<TARGET_OBJECTS:db_obj>
		$<TARGET_OBJECTS:net_model_obj>
		)
add_executable(test_tpl_net_model
		${TPL_STANDARD_NET_MODEL_SRC}
		test_tpl_standard_net_model.cpp
		)
target_link_libraries(test_tpl_net_model bookshelf ${Boost_LIBRARIES})

#4.TplStandardNetForceModel
set(TPL_STANDARD_NET_FORCE_MODEL_SRC
		${TPL_STANDARD_NET_MODEL_SRC}
		$<TARGET_OBJECTS:net_force_model_obj>
		)
add_executable(test_tpl_net_force_model
		${TPL_STANDARD_NET_FORCE_MODEL_SRC}
		test_tpl_standard_net_force_model.cpp
		)
target_link_libraries(test_tpl_net_force_model bookshelf ${Boost_LIBRARIES})

#5.TplStandardThermalModel
set(TPL_STANDARD_THERMAL_MODEL_SRC
		$<TARGET_OBJECTS:db_obj>
		$<TARGET_OBJECTS:config_obj>
		$<TARGET_OBJECTS:thermal_model_obj>
		)
add_executable(test_tpl_thermal_model
		${TPL_STANDARD_THERMAL_MODEL_SRC}
		test_tpl_standard_thermal_model.cpp
		)
target_link_libraries(test_tpl_thermal_model bookshelf ${Boost_LIBRARIES})

#6.TplStandardDetailPlacement
set(TPL_STANDARD_DETAIL_PLACEMENT_SRC
		$<TARGET_OBJECTS:db_obj>
		$<TARGET_OBJECTS:config_obj>
		$<TARGET_OBJECTS:detail_placement_obj>
		)
add_executable(test_tpl_detail_placement
		${TPL_STANDARD_DETAIL_PLACEMENT_SRC}
		test_tpl_standard_detail_placement.cpp
		)
target_link_libraries(test_tpl_detail_placement bookshelf ${Boost_LIBRARIES}
		${PROJECT_SOURCE_DIR}/tools/rippledp/rippledp.a
		${PROJECT_SOURCE_DIR}/tools/rippledp/libdef.a
		${PROJECT_SOURCE_DIR}/tools/rippledp/liblef.a
		)

#Checks
add_executable(check_free_nodes_order $<TARGET_OBJECTS:db_obj> check_free_nodes_order.cpp)
target_link_libraries(check_free_nodes_order bookshelf ${Boost_LIBRARIES})
