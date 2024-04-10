add_library(vendor INTERFACE)

add_subdirectory(Eigen)

add_subdirectory(GLM)
add_subdirectory(GLFW)
add_subdirectory(GLAD)

target_link_libraries(vendor INTERFACE
    Eigen
    GLM
    GLFW
    GLAD
)