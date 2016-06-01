include_directories(${PROJECT_SOURCE_DIR}/tpl)

#1.TplDB
add_library(db_obj OBJECT tpl_db.cpp)

#2.TplConfig
add_library(config_obj OBJECT tpl_config.cpp)

#3.TplStandardNetModel
add_library(net_model_obj OBJECT tpl_standard_net_model.cpp)

#4.TplStandardNetForceModel
add_library(net_force_model_obj OBJECT tpl_standard_net_force_model.cpp)

#5.TplStandardThermalModel
 add_library(thermal_model_obj OBJECT tpl_standard_thermal_model.cpp)

#6.TplStandardDetailPlacement
add_library(detail_placement_obj OBJECT tpl_gy_detail_placement.cpp)

#7.TplStandardAlgorithm
add_library(standard_algorithm_obj OBJECT tpl_standard_algorithm.cpp)

#8.TplStandardThermalForceModel
add_library(thermal_force_model_obj OBJECT tpl_standard_thermal_force_model.cpp)
