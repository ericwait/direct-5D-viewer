set(D3dLib_SOURCES
    ${D3d_DIR}/Camera.cpp
    ${D3d_DIR}/DepthTarget.cpp
    ${D3d_DIR}/EigenToFromDirectX.cpp
    ${D3d_DIR}/Initialization.cpp
    ${D3d_DIR}/Material.cpp
    ${D3d_DIR}/MaterialParams.cpp
    ${D3d_DIR}/MeshPrimitive.cpp
    ${D3d_DIR}/MessageProcessor.cpp
    ${D3d_DIR}/Renderer.cpp
    ${D3d_DIR}/RenderTarget.cpp
    ${D3d_DIR}/SceneNode.cpp
    ${D3d_DIR}/TextRenderer.cpp
    ${D3d_DIR}/Texture.cpp
    ${D3d_DIR}/TextureLightingObj.cpp
    ${D3d_DIR}/VertexLayouts.cpp
    ${D3d_DIR}/VolumeInfo.cpp
)
set(Global_SOURCES
    ${Global_DIR}/ModuleInfo.cpp
    ${Global_DIR}/WidgetData.cpp
)
set(Messages_SOURCES
    ${Messages_DIR}/AnimMessages.cpp
    ${Messages_DIR}/ErrorMsg.cpp
    ${Messages_DIR}/LoadData.cpp
    ${Messages_DIR}/LoadMessages.cpp
    ${Messages_DIR}/MessageHelpers.cpp
    ${Messages_DIR}/MessageQueue.cpp
    ${Messages_DIR}/QueuePolygon.cpp
    ${Messages_DIR}/RenderMessages.cpp
    ${Messages_DIR}/ReturnQueue.cpp
    ${Messages_DIR}/ViewMessages.cpp
    ${Messages_DIR}/ViewerGlobals.cpp
)

find_package(Eigen3 3.4 CONFIG REQUIRED)

# Library sources
add_library(D3dLib STATIC ${D3dLib_SOURCES} ${Global_SOURCES} ${Messages_SOURCES})

# Include directories
target_include_directories(D3dLib PUBLIC 
    ${D3d_DIR}
    ${Global_DIR}
    ${Messages_DIR}
)

# Link necessary libraries
target_link_libraries(D3dLib d3dcompiler d3d11 Eigen3::Eigen)

# Set compile options
target_compile_definitions(D3dLib PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)
