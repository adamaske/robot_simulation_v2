add_library(deps INTERFACE)

add_subdirectory(Core)
add_subdirectory(Networking)

target_link_libraries(deps INTERFACE
    Core
    Networking
)
