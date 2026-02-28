from setuptools import setup, Extension
import os
import sys

# Assume the developer has built the extension via CMake and we just ship it?
# Or do we use setuptools to build?
# For now, let's keep it simple and assume a "source" build.

setup(
    name="d3d_viewer",
    version="0.1.0",
    description="Direct 5D Viewer Python Interface",
    packages=["d3d_viewer"],
    # Fix package_dir to point correctly
    package_dir={"d3d_viewer": "d3d_viewer"},
)
