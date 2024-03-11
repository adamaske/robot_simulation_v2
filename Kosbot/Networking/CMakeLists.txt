add_library(Networking entry.cpp)

target_include_directories(Networking 
                                INTERFACE 
                                ${CMAKE_CURRENT_SOURCE_DIR}

                            )


option(USE_NETWORKING "Use Custom Networking Library" ON)

if(USE_NETWORKING)

    target_compile_definitions(Networking PRIVATE "USE_NETWORKING")

    


endif()


#Link cpptemaplte compiler flags to this one
target_link_libraries(Networking PUBLIC cpptemplate_compiler_flags)



