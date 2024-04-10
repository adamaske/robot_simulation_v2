
file(GLOB CORE_SOURCES ${CURRENT_CMAKE_SOURCE_DIR}src/*cpp)
file(GLOB CORE_INCLUDES ${CURRENT_CMAKE_SOURCE_DIR}include/*.h)

add_library(Core SHARED ${CORE_SOURCES} ${CORE_INCLUDES})
target_include_directories(Core INTERFACE ${CORE_INCLUDES})

target_link_libraries(Core PUBLIC kosbot_compiler_flags)

target_compile_definitions(Core PRIVATE "CORE_ACTIVE")


