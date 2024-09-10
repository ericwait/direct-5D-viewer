set(Mex_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(MexLib_SOURCES
    ${Mex_DIR}/MexAddPolygons.cpp
    ${Mex_DIR}/MexCaptureSpinMovie.cpp
    ${Mex_DIR}/MexCaptureWindow.cpp
    ${Mex_DIR}/MexClearAllTextures.cpp
    ${Mex_DIR}/MexClearTextureFrame.cpp
    ${Mex_DIR}/MexClose.cpp
    ${Mex_DIR}/MexCommand.cpp
    ${Mex_DIR}/MexDeleteAllPolygons.cpp
    ${Mex_DIR}/MexInit.cpp
    ${Mex_DIR}/MexInitVolume.cpp
    ${Mex_DIR}/MexLoadTexture.cpp
    ${Mex_DIR}/MexLoadTextureFrame.cpp
    ${Mex_DIR}/MexMoveCamera.cpp
    ${Mex_DIR}/MexPlay.cpp
    ${Mex_DIR}/MexPoll.cpp
    ${Mex_DIR}/MexPolygonLighting.cpp
    ${Mex_DIR}/MexReleaseControl.cpp
    ${Mex_DIR}/MexRemovePolygon.cpp
    ${Mex_DIR}/MexResetView.cpp
    ${Mex_DIR}/MexSetBackColor.cpp
    ${Mex_DIR}/MexSetBorderColor.cpp
    ${Mex_DIR}/MexSetBorderColor.cpp
    ${Mex_DIR}/MexSetCapturePath.cpp
    ${Mex_DIR}/MexSetCaptureSize.cpp
    ${Mex_DIR}/MexSetDpiScale.cpp
    ${Mex_DIR}/MexSetFrame.cpp
    ${Mex_DIR}/MexSetFrontClip.cpp
    ${Mex_DIR}/MexSetViewOrigin.cpp
    ${Mex_DIR}/MexSetViewRotation.cpp
    ${Mex_DIR}/MexSetWindowSize.cpp
    ${Mex_DIR}/MexSetWorldRotation.cpp
    ${Mex_DIR}/MexShowAllPolygons.cpp
    ${Mex_DIR}/MexShowFrameNumber.cpp
    ${Mex_DIR}/MexShowLabels.cpp
    ${Mex_DIR}/MexShowPolygonList.cpp
    ${Mex_DIR}/MexShowScaleBar.cpp
    ${Mex_DIR}/MexShowTexture.cpp
    ${Mex_DIR}/MexShowWidget.cpp
    ${Mex_DIR}/MexSpin.cpp
    ${Mex_DIR}/MexTakeControl.cpp
    ${Mex_DIR}/MexTextureAttenuation.cpp
    ${Mex_DIR}/MexTextureLighting.cpp
    ${Mex_DIR}/MexToggleWireframe.cpp
    ${Mex_DIR}/MexTransferFunction.cpp
    ${Mex_DIR}/MexUpdateRender.cpp
    ${Mex_DIR}/Threads.cpp
    ${Mex_DIR}/Viewer.cpp
    ${Mex_DIR}/Widget.cpp
)

add_library(D3dMexLib SHARED ${MexLib_SOURCES})

target_include_directories(D3dMexLib PUBLIC 
    ${Mex_DIR}
    ${D3d_DIR}
    ${Global_DIR}
    ${Messages_DIR}
    ${Matlab_INCLUDE_DIRS}
)

target_link_libraries(D3dMexLib PRIVATE D3dLib d3dcompiler d3d11 ${Matlab_LIBRARIES})

target_compile_definitions(D3dMexLib PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# Use CMAKE_CURRENT_SOURCE_DIR for a relative path from the current CMakeLists.txt file
add_custom_command(TARGET D3dMexLib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:D3dMexLib>
    ${CMAKE_CURRENT_SOURCE_DIR}/../../matlab/+D3d/@Viewer/Mex.mexw64
)

add_custom_command(TARGET D3dMexLib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${Shaders_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../matlab/+D3d/@Viewer/Shaders
)
