include_directories(${PROJECT_SOURCE_DIR}/tpl)
include_directories(${PROJECT_SOURCE_DIR}/tools/rippledp)
include_directories(${PROJECT_SOURCE_DIR}/tools/lef)
include_directories(${PROJECT_SOURCE_DIR}/tools/def)

#1.TplDB
add_executable(test_tpl_db $<TARGET_OBJECTS:db_obj> test_tpl_db.cpp)
target_link_libraries(test_tpl_db bookshelf ${Boost_LIBRARIES})

#2.TplStandardNetModel
set(TPL_STANDARD_NET_MODEL_SRC
		$<TARGET_OBJECTS:db_obj>
		$<TARGET_OBJECTS:net_model_obj>
		)
add_executable(test_tpl_net_model
		${TPL_STANDARD_NET_MODEL_SRC}
		test_tpl_standard_net_model.cpp
		)
target_link_libraries(test_tpl_net_model bookshelf ${Boost_LIBRARIES})

#3.TplStandardNetForceModel
set(TPL_STANDARD_NET_FORCE_MODEL_SRC
		${TPL_STANDARD_NET_MODEL_SRC}
		$<TARGET_OBJECTS:net_force_model_obj>
		)
add_executable(test_tpl_net_force_model
		${TPL_STANDARD_NET_FORCE_MODEL_SRC}
		test_tpl_standard_net_force_model.cpp
		)
target_link_libraries(test_tpl_net_force_model bookshelf ${Boost_LIBRARIES})

#4.TplStandardThermalForceModel
add_library(pthread STATIC IMPORTED)
SET_TARGET_PROPERTIES(pthread PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/tools/libpthread.a)
set(TPL_STANDARD_THERMAL_FORCE_MODEL_SRC
		$<TARGET_OBJECTS:db_obj>
		$<TARGET_OBJECTS:thermal_force_model_obj>
		)
add_executable(test_tpl_thermal_force_model
		${TPL_STANDARD_THERMAL_FORCE_MODEL_SRC}
		test_tpl_standard_thermal_force_model.cpp
		)
target_link_libraries(test_tpl_thermal_force_model
		bookshelf
		${Boost_LIBRARIES} ${STXXL_LIBRARIES}
		pthread
		)

#5.TplGyDetailPlacement
set(TPL_GY_DETAIL_PLACEMENT_SRC
		$<TARGET_OBJECTS:db_obj>
		$<TARGET_OBJECTS:detail_placement_obj>
		)
add_executable(test_tpl_detail_placement
		${TPL_GY_DETAIL_PLACEMENT_SRC}
		test_tpl_gy_detail_placement.cpp
		)
target_link_libraries(test_tpl_detail_placement bookshelf ${Boost_LIBRARIES}
		${PROJECT_SOURCE_DIR}/tools/rippledp/rippledp.a
		${PROJECT_SOURCE_DIR}/tools/rippledp/libdef.a
		${PROJECT_SOURCE_DIR}/tools/rippledp/liblef.a
		)

#6.TplStandardAlgorithm
set(TPL_STANDARD_ALGORITHM_SRC
		$<TARGET_OBJECTS:db_obj>
		$<TARGET_OBJECTS:net_model_obj>
		$<TARGET_OBJECTS:net_force_model_obj>
		$<TARGET_OBJECTS:thermal_force_model_obj>
		$<TARGET_OBJECTS:standard_algorithm_obj>
		)
add_executable(test_tpl_standard_algorithm
		${TPL_STANDARD_ALGORITHM_SRC}
		test_tpl_standard_algorithm.cpp
		)
target_link_libraries(test_tpl_standard_algorithm
		bookshelf
		${Boost_LIBRARIES} ${STXXL_LIBRARIES}
        ${PROJECT_SOURCE_DIR}/tools/rippledp/rippledp.a
        ${PROJECT_SOURCE_DIR}/tools/rippledp/libdef.a
        ${PROJECT_SOURCE_DIR}/tools/rippledp/liblef.a
		pthread
        )


#Checks
add_executable(check_free_nodes_order $<TARGET_OBJECTS:db_obj> check_free_nodes_order.cpp)
target_link_libraries(check_free_nodes_order bookshelf ${Boost_LIBRARIES})
