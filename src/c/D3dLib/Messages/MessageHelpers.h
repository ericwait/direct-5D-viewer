#pragma once

#include "D3d/Renderer.h"

// A few helper functions that Messages and MessageProcessor use
void setObjectTypeVisibility(GraphicObjectTypes type, bool visible);
void setObjectTypeColor(GraphicObjectTypes type, Vec<float> color, float alpha);
void setObjectWireframe(GraphicObjectTypes type, bool wireframe);
void setObjectLighting(GraphicObjectTypes type, bool lightingOn);