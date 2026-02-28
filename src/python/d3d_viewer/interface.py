import numpy as np
import threading
import time

# Try to import the extension
# In a real install, this would be properly located.
# For development, we might need to add the build path.
try:
    import d3d_viewer_ext
except ImportError:
    # Attempt to find it in build directories for dev convenience
    import sys
    import os
    # Adjust this path based on where CMake puts the file
    build_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../build/src/c/Python/Release"))
    if os.path.exists(build_path):
        sys.path.append(build_path)
        import d3d_viewer_ext
    else:
        raise ImportError("Could not find d3d_viewer_ext. Please build the C++ extension first.")

class D3dViewer:
    def __init__(self):
        self.is_open = False

    def init(self):
        """Initialize the viewer window."""
        if self.is_open:
            return
        
        # We need to pass the root directory where shaders are found.
        # This is tricky in Python. Let's assume a default or let user override.
        # For now, pass a dummy path or try to find the source root.
        root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../.."))
        
        # The C++ init launches a thread and returns.
        # We pass 0 for HWND to let it create its own window.
        d3d_viewer_ext.init_viewer(0, root_dir)
        self.is_open = True
        
        # Give it a moment to start
        time.sleep(0.5)

    def load_image(self, image, frame=0, buffer_type=1):
        """
        Load a 3D volume into the viewer.
        
        :param image: Numpy array (Z, Y, X) or (C, Z, Y, X) - TODO: Clarify dimensions
                      The C++ expects flat pointer, so we need to match what Mex does.
        :param frame: Time point index.
        :param buffer_type: 1 for original, 2 for processed.
        """
        if not self.is_open:
            self.init()
            
        # Ensure contiguous uint8
        if image.dtype != np.uint8:
            image = image.astype(np.uint8)
        
        if not image.flags['C_CONTIGUOUS']:
            image = np.ascontiguousarray(image)

        d3d_viewer_ext.load_texture(image, frame, buffer_type)

    def close(self):
        if self.is_open:
            d3d_viewer_ext.close_viewer()
            self.is_open = False

    def __enter__(self):
        self.init()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
