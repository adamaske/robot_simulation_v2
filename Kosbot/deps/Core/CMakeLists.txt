add_library(Core STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Core.cpp)

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

target_include_directories(Core PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)
