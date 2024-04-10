add_library(vendor INTERFACE)

add_subdirectory(Eigen)

target_link_libraries(vendor INTERFACE
    Eigen
)