# System Architecture

## Overview
The **Direct 5D Viewer** is a high-performance visualization tool designed to inspect microscope data up to 5 dimensions (X, Y, Z, Time, Channel). It leverages a hybrid architecture combining a flexible **Matlab** frontend for user interaction and data handling with a high-performance **C++/DirectX 11** backend for real-time volumetric rendering.

## 5D Visualization Capabilities
The system explicitly handles 5 dimensions of data:
1.  **Spatial (X, Y, Z)**: Handled by the `VolumeInfo` and `Renderer`, representing the 3D volume.
2.  **Time (T)**: Managed via `numFrames` in `VolumeInfo` and dynamic texture updates. The system can play through time points seamlessly.
3.  **Channel (C)**: Support for multiple channels (e.g., different fluorescent markers) via `numChannels`, allowing blended visualization of different biological structures.

## Core Components & Real-Time Enablement

The system is divided into three main layers, each playing a critical role in enabling real-time performance:

### 1. Matlab Frontend (`src/matlab/+D3d`)
**Role:** User Interface & Data Management
-   **Functionality:** Provides the API for users to load images, control the view, and script interactions. It handles the high-level logic of what data to show.
-   **Key Files:**
    -   `@Viewer/` class: The main entry point for user scripts.
    -   `LoadImage.m`: Handles reading complex microscope data formats before sending to the backend.
-   **Connection to Real-Time:** Pre-processes data and sends commands asynchronously, ensuring the UI remains responsive while heavy rendering happens in the background.

### 2. Mex Interface (`src/c/Mex`)
**Role:** High-Performance Bridge
-   **Functionality:** Acts as the glue between Matlab's managed environment and the raw C++ memory. It converts Matlab data structures into C++ compatible formats and pushes commands to the backend.
-   **Key Files:**
    -   `MexLoadTextureFrame.cpp`: Efficiently transfers image data for a specific time point from Matlab memory to the rendering engine.
    -   `MexCommand.cpp`: Processes control commands (rotation, channel toggling) instantly.
-   **Connection to Real-Time:** Uses a **Message Queue** architecture (`gMsgQueueToDirectX`). Matlab functions return immediately after pushing a message, preventing the UI from hanging while the GPU processes the data.

### 3. C++ / Direct3D Backend (`src/c/D3dLib`)
**Role:** Real-Time Rendering Engine
-   **Functionality:** The core engine that holds the data on the GPU and performs the actual drawing. It runs on a dedicated thread to ensure smooth animation frames.
-   **Key Components:**
    -   **`Renderer`** (`D3d/Renderer.cpp`): Manages the DirectX 11 device, swap chains, and render loop. It creates a dedicated window for visualization.
    -   **`VolumeInfo`** (`D3d/VolumeInfo.cpp`): Stores the metadata for the 5D dataset (dimensions, physical size, number of frames/channels).
    -   **`MessageProcessor`** (`D3d/MessageProcessor.cpp`): The heart of the backend thread (`messageLoop`). It continuously checks for new messages (e.g., "Load Frame 5") and updates the scene without stopping the render loop.
    -   **Shaders** (`Shaders/ViewAlignedVolumePS.hlsl`): Custom High-Level Shading Language (HLSL) programs that execute on the GPU. They perform volume ray-casting or slicing to visualize the 3D data instantly as the camera moves.
    -   **Inspection Features**: The renderer supports **Front/Back Clipping** and **Depth Peeling** (passing clip planes to shaders), allowing users to "peel away" outer layers and inspect internal structures of the 5D volume.

## Data Flow for Real-Time Inspection

1.  **Loading**: The user loads a dataset in Matlab.
2.  **Transfer**: `MexLoadTextureFrame` pushes a pointer to the image data into the message queue.
3.  **Update**: The `MessageProcessor` on the render thread picks up the message and uploads the data to a **DirectX 3D Texture**.
4.  **Rendering**: The `Renderer` draws the scene. The pixel shader (`ViewAlignedVolumePS`) samples the 3D texture based on the current view, applying transfer functions (color mapping) for each channel.
5.  **Interaction**: When the user rotates the view or plays a movie:
    -   Rotation inputs are processed instantly by the render thread's loop.
    -   Time updates (`MexSetFrame`) trigger a switch to the pre-loaded 3D texture for the next time point, enabling smooth 5D playback.
