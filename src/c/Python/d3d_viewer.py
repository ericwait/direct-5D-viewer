
try:
    import d3d_viewer_ext
except ImportError:
    # Allow import without extension for testing/linting purposes, 
    # but actual usage will fail if extension is missing.
    d3d_viewer_ext = None

class D3dViewer:
    def __init__(self):
        if d3d_viewer_ext is None:
            raise ImportError("d3d_viewer_ext module not found. Make sure the C++ extension is built and in PYTHONPATH.")
        
    def init(self, root_dir: str):
        """
        Initialize the viewer.
        :param root_dir: Path to the directory containing data/shaders.
        """
        # HWND is currently ignored by the implementation, passing 0
        d3d_viewer_ext.init_viewer(0, root_dir)

    def load_texture(self, image, frame, buffer_type=1):
        """
        Load a texture frame into the viewer.
        :param image: Numpy array (3D: Y, X, Z) of uint8.
        :param frame: Frame index.
        :param buffer_type: 1 for OriginalVolume, 2 for ProcessedVolume.
        """
        d3d_viewer_ext.load_texture(image, frame, buffer_type)

    def close(self):
        """
        Close the viewer thread.
        """
        d3d_viewer_ext.close_viewer()
