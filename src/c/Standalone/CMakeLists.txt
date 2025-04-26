set(Window_SOURCES
    ${D3dWindow_DIR}/Initialization.cpp
    ${D3dWindow_DIR}/MessageProcessor.cpp
    ${D3dWindow_DIR}/ModuleInfo.cpp
)

# Create an executable from Standalone.cpp
add_executable(StandaloneApp Standalone.cpp ${Window_SOURCES})

# Include directories from the root CMakeLists.txt for headers
target_include_directories(StandaloneApp PRIVATE
    ${D3dLib_DIR}
    ${D3dWindow_DIR}
)

target_link_libraries(StandaloneApp PRIVATE D3dLib d3dcompiler d3d11)

set_target_properties(StandaloneApp PROPERTIES
    WIN32_EXECUTABLE TRUE
)

add_custom_command(TARGET StandaloneApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${D3dLib_DIR}/Shaders
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Shaders
)
