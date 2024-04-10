add_library(Eigen INTERFACE)#header only

target_include_directories(Eigen INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
