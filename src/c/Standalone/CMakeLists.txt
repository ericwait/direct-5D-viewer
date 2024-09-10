# Create an executable from Standalone.cpp
add_executable(StandaloneApp Standalone.cpp)

# Include directories from the root CMakeLists.txt for headers
target_include_directories(StandaloneApp PRIVATE
    ${D3d_DIR}
    ${Global_DIR}
    ${Messages_DIR}
)

target_link_libraries(StandaloneApp PRIVATE D3dLib d3dcompiler d3d11)

set_target_properties(StandaloneApp PROPERTIES
    WIN32_EXECUTABLE TRUE
)

add_custom_command(TARGET StandaloneApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${Shaders_DIR}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Shaders
)
