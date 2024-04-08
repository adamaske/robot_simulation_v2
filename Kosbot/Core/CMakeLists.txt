add_library(Core entry.cpp)

# for anybody to use Core, they need to include 
target_include_directories(Core INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(Core PUBLIC cpp_compiler_flags)



