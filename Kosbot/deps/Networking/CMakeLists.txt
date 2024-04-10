add_library(Networking STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Networking.cpp)

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

target_include_directories(Networking PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)
