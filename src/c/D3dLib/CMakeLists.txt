set(D3dLib_SOURCES
    ${D3dLib_DIR}/D3d/Camera.cpp
    ${D3dLib_DIR}/D3d/DepthTarget.cpp
    ${D3dLib_DIR}/D3d/EigenToFromDirectX.cpp
    ${D3dLib_DIR}/D3d/Material.cpp
    ${D3dLib_DIR}/D3d/MaterialParams.cpp
    ${D3dLib_DIR}/D3d/MeshPrimitive.cpp
    ${D3dLib_DIR}/D3d/Renderer.cpp
    ${D3dLib_DIR}/D3d/RenderInit.cpp
    ${D3dLib_DIR}/D3d/RenderTarget.cpp
    ${D3dLib_DIR}/D3d/SceneNode.cpp
    ${D3dLib_DIR}/D3d/TextRenderer.cpp
    ${D3dLib_DIR}/D3d/Texture.cpp
    ${D3dLib_DIR}/D3d/TextureLightingObj.cpp
    ${D3dLib_DIR}/D3d/VertexLayouts.cpp
    ${D3dLib_DIR}/D3d/VolumeInfo.cpp
)
set(Global_SOURCES
    ${D3dLib_DIR}/Global/WidgetData.cpp
)
set(Messages_SOURCES
    ${D3dLib_DIR}/Messages/AnimMessages.cpp
    ${D3dLib_DIR}/Messages/ErrorMsg.cpp
    ${D3dLib_DIR}/Messages/LoadData.cpp
    ${D3dLib_DIR}/Messages/LoadMessages.cpp
    ${D3dLib_DIR}/Messages/MessageHelpers.cpp
    ${D3dLib_DIR}/Messages/MessageQueue.cpp
    ${D3dLib_DIR}/Messages/QueuePolygon.cpp
    ${D3dLib_DIR}/Messages/RenderMessages.cpp
    ${D3dLib_DIR}/Messages/ReturnQueue.cpp
    ${D3dLib_DIR}/Messages/ViewMessages.cpp
    ${D3dLib_DIR}/Messages/ViewerGlobals.cpp
)

find_package(Eigen3 3.4 CONFIG REQUIRED)

# Library sources
add_library(D3dLib STATIC ${D3dLib_SOURCES} ${Global_SOURCES} ${Messages_SOURCES})

# Include directories
target_include_directories(D3dLib PUBLIC 
    ${D3dLib_DIR}
    ${Thirdparty_DIR}
)

# Link necessary libraries
target_link_libraries(D3dLib d3dcompiler d3d11 Eigen3::Eigen)

# Set compile options
target_compile_definitions(D3dLib PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)
