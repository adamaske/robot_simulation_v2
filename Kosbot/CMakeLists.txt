cmake_minimum_required(VERSION 3.15)

project(Kosbot VERSION 1.0)

#add_subdirectory(Kostull)

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
file(GLOB INCS ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h)

add_executable(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include")

#target_link_libraries(${PROJECT_NAME} PRIVATE Kostull)