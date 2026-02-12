# Implementation Plan - Python Interface

This plan details the creation of a Python interface for the Direct 5D Viewer, replicating the functionality of the existing Matlab (`Mex`) interface. The goal is to allow Python users (e.g., using Napari, Jupyter, or raw scripts) to visualize 5D data using the high-performance DirectX backend.

## Architecture

We will use **PyBind11** to create a native C++ extension module (`d3d_viewer_ext`) that links against the core `D3dLib`. A high-level Python package (`d3d_viewer`) will wrap this extension to provide a user-friendly API similar to the Matlab `+D3d` package.

### Directory Structure
```
src/
  c/
    Python/             [NEW] - C++ PyBind11 code
      CMakeLists.txt
      PyBind.cpp
      ...
  python/               [NEW] - Python package
    setup.py
    d3d_viewer/
      __init__.py
      utils.py
```

## User Review Required

> [!IMPORTANT]
> **dependency**: This plan requires `pybind11`. The project currently uses `vcpkg`. I will assume `pybind11` can be found via `find_package(pybind11)` or added to the vcpkg manifest if one exists (not visible in file list, but `CMakeLists.txt` references vcpkg).

## Proposed Changes

### C++ Backend (`src/c/Python`)

#### [NEW] [PyBind.cpp](file:///e:/programming/direct-5D-viewer/src/c/Python/PyBind.cpp)
The main entry point for the Python extension. It will expose:
-   `init(hwnd: int, root_dir: str)`: Initializes the global renderer thread.
-   `load_texture(image: np.array, frame: int)`: Accepts a numpy array (via buffer protocol), extracts the pointer, and pushes a `MessageLoadTextureFrame`.
-   `close()`: Cleans up.
-   `update_render()`: Triggers a frame update.
-   Other commands (rotate, set_transfer_function, etc.) as needed.

#### [NEW] [CMakeLists.txt](file:///e:/programming/direct-5D-viewer/src/c/Python/CMakeLists.txt)
-   Finds `pybind11`.
-   Adds a library target that links `D3dLib` and output a Python extension module.

#### [MODIFY] [CMakeLists.txt](file:///e:/programming/direct-5D-viewer/CMakeLists.txt)
-   Add `add_subdirectory(src/c/Python)` conditional on Python + PyBind11 availability.

### Python Frontend (`src/python`)

#### [NEW] [setup.py](file:///e:/programming/direct-5D-viewer/src/python/setup.py)
-   Standard setuptools setup to build the extension.

#### [NEW] [interface.py](file:///e:/programming/direct-5D-viewer/src/python/d3d_viewer/interface.py)
-   Pythonic wrapper around the `_d3d_viewer_ext` module.
-   Handles validaton of input arrays (dtype, dimensionality).
-   Provides context managers (e.g., `with D3dViewer() as viewer:`).

## Verification Plan

### Automated Tests
1.  **Build Verification**: Run `cmake` and `build` to ensure the extension compiles.
2.  **Import Test**: Run a simple Python script to `import d3d_viewer` and check for symbol resolution errors.

### Manual Verification
1.  Since we cannot run a GUI window in the agent environment easily, verification will largely depend on successful compilation and linking.

## Phase 2: Universal UI (Future Work)

The user has expressed interest in a UI similar to the existing Matlab `+D3d/+UI` controls. Rather than rewriting the UI twice (once for Matlab, once for Python/Qt), we propose a **Universal C++ UI** using **Dear ImGui**.

### Rationale
-   **Write Once, Run Everywhere**: The UI is rendered directly by the DirectX backend, meaning it works identical whether the host is Matlab, Python, or a standalone C++ exe.
-   **Performance**: ImGui is designed for real-time 3D applications and has negligible overhead.
-   **Simplicity**: usage becomes `D3d.Open(im)` (Python/Matlab) and the customized UI appears inside the window immediately.

### Proposed Changes for Phase 2

#### [MODIFY] [CMakeLists.txt](file:///e:/programming/direct-5D-viewer/src/c/D3dLib/CMakeLists.txt)
-   Add `imgui` dependency (via vcpkg).

#### [MODIFY] [Renderer.cpp](file:///e:/programming/direct-5D-viewer/src/c/D3dLib/D3d/Renderer.cpp)
-   Initialize ImGui context on device creation.
-   Add `ImGui_ImplDX11_NewFrame()` and `ImGui::Render()` to the render loop.
-   Implement a `renderUI()` method to draw sliders for:
    -   Contrast/Brightness (Transfer Function).
    -   Time navigation.
    -   Channel toggling.
    -   Clipping planes.
