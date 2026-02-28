# Python Interface for Direct 5D Viewer

This directory contains the source code for the Python extension of the Direct 5D Viewer, built using [PyBind11](https://github.com/pybind/pybind11).

## Prerequisites

To build and run this extension, you need:

1.  **Python 3.8+**
2.  **CMake 3.22+**
3.  **C++ Compiler** (MSVC on Windows)
4.  **PyBind11** Python package:

```bash
pip install pybind11
```

## Building

The Python extension is built as part of the main CMake project.

1.  **Configure**:
    Run CMake from the project root:
    ```bash
    cmake -B build -S .
    ```
    *Make sure `pybind11` is found. If CMake cannot find it, ensure you have installed it in your active Python environment.*

2.  **Build**:
    Compile the project (Release mode recommended for performance):
    ```bash
    cmake --build build --config Release
    ```

3.  **Output**:
    The compiled extension module (e.g., `d3d_viewer_ext.cp311-win_amd64.pyd`) will be located in:
    `build/src/c/Python/Release/`

## Usage & Testing

A Python wrapper `d3d_viewer.py` is provided in this directory to give a proper Object-Oriented API.

### Running the Test

To run the provided test script (`tests/test_basic.py`), you must ensure Python can find the compiled extension file (`.pyd`) and the wrapper (`d3d_viewer.py`).

1.  **Set PYTHONPATH**:
    Add the build output directory and the source directory to your `PYTHONPATH`.

    **PowerShell**:
    ```powershell
    $env:PYTHONPATH = "$PWD/build/src/c/Python/Release;$PWD/src/c/Python"
    ```

    **Command Prompt**:
    ```cmd
    set PYTHONPATH=%cd%\build\src\c\Python\Release;%cd%\src\c\Python
    ```

2.  **Run the Test**:
    ```bash
    python tests/test_basic.py
    ```

### Example Usage

```python
from d3d_viewer import D3dViewer
import numpy as np

# Initialize viewer
viewer = D3dViewer()
viewer.init("path/to/resources")

# Create a 3D volume (Y, X, Z)
data = np.random.randint(0, 255, (512, 512, 100), dtype=np.uint8)

# Load texture (frame 0)
viewer.load_texture(data, 0)

# ...

# Close viewer
viewer.close()
```
