#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "Initialization.h"
#include "Globals.h"
#include "MessageQueue.h"
#include "LoadMessages.h"
#include "MessageHelpers.h"
#include "Threads.h"

namespace py = pybind11;

// Global Queue defined in other modules
extern MessageQueue gMsgQueueToDirectX;


// Wrappers

void cpp_init_viewer(int hwnd, std::string rootDir)
{
    // Initialize the viewer thread
    startThread(rootDir);
}

void cpp_load_texture(py::array_t<uint8_t> image, int frame, int bufferType)
{
    py::buffer_info buf = image.request();

    if (buf.ndim < 3)
        throw std::runtime_error("Image must be at least 3 dimensions (Y, X, Z)");

    // Data copy is required because we need a raw pointer. 
    // We will use forceWait=true to ensure the render thread reads it before we delete it.
    size_t numBytes = buf.size * sizeof(uint8_t);
    unsigned char* dataCopy = new unsigned char[numBytes];
    memcpy(dataCopy, buf.ptr, numBytes);

    // GraphicObjectTypes is defined in Renderer.h (via Globals.h)
    // Using simple enum access or scope depending on what compiler accepts.
    // The codebase seems to use GraphicObjectTypes::Group in some places if it's treated as scope-able,
    // or just the value. MexLoadTextureFrame.cpp uses GraphicObjectTypes::OriginalVolume.
    GraphicObjectTypes type = (bufferType == 2) ? GraphicObjectTypes::ProcessedVolume : GraphicObjectTypes::OriginalVolume;

    // Push message and WAIT for completion so we can safely delete the buffer.
    bool success = gMsgQueueToDirectX.pushMessage(new MessageLoadTextureFrame(type, frame, dataCopy), true);
    
    delete[] dataCopy;

    if (!success) {
        throw std::runtime_error("Failed to push message to render thread");
    }
}

void cpp_close_viewer()
{
    gMsgQueueToDirectX.pushMessage(new MessageClose());
}


PYBIND11_MODULE(d3d_viewer_ext, m) {
    m.doc() = "Direct 5D Viewer Python Extension";

    m.def("init_viewer", &cpp_init_viewer, "Initialize the viewer");
    m.def("load_texture", &cpp_load_texture, "Load a texture frame",
        py::arg("image"), py::arg("frame"), py::arg("buffer_type") = 1);
    m.def("close_viewer", &cpp_close_viewer, "Close the viewer");
}
