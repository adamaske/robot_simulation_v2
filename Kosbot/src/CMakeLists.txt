add_executable(${PROJECT_NAME} main.cpp glad.c)

target_link_libraries(${PROJECT_NAME} PRIVATE deps vendor)