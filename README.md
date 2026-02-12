# Direct 5D Viewer

## Overview

**Direct 5D Viewer** is a high-performance, real-time visualization tool designed explicitly for inspecting complex microscope data up to 5 dimensions: **X**, **Y**, **Z** (Spatial), **Time**, and **Channel** (Color).

By leveraging a powerful hybrid architecture of **Matlab** for data management and **DirectX 11 (C++)** for rendering, this viewer delivers interactive 3D volume rendering and smooth playback of time-series data without sacrificing performance.

## Key Features

-   **5D Data Support**: Seamlessly inspect datasets spanning across 3 spatial dimensions, time, and multiple fluorescent channels.
-   **Real-Time Rendering**: Utilizing DirectX 11, the viewer processes large volumetric data directly on the GPU, enabling smooth rotation, zooming, and playback.
-   **Interactive Analysis**: Users can visualize specific channels, adjust transfer functions, and inspect time dynamics interactively.
-   **Matlab Integration**: Directly load and manipulate data from your existing Matlab workflows.

## How It Works

The system achieves real-time performance through a decoupled architecture:

1.  **Matlab Interface**: Handles user commands and data loading asynchronously.
2.  **Mex Bridge**: Efficiently transfers image data and commands to the rendering engine via a high-speed message queue.
3.  **DirectX Backend**: A dedicated C++ rendering thread manages the GPU resources and executes advanced volume rendering shaders (HLSL) for instant visual feedback.

For a detailed technical breakdown of how each component contributes to the real-time 5D inspection capabilities, please see the [Architecture Documentation](docs/ARCHITECTURE.md).

## Getting Started

### Prerequisites
-   Windows OS (Required for DirectX 11)
-   Matlab
-   Visual Studio (for building the C++ backend)

### Installation
1.  Open the solution file in `src/c` with Visual Studio to build the backend libraries.
2.  Add the `src/matlab` directory to your Matlab path.
3.  Use the `D3d.Viewer` class in Matlab to load and visualize your data.

## License
Distributed under the BSD 3-Clause License. See [LICENSE](LICENSE) for more information.
