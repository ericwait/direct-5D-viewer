#pragma once

#include "Renderer.h"
#include "Texture.h"
#include "Material.h"
#include "MeshPrimitive.h"
#include "Camera.h"

#include "Color.h"

#include <map>
#include <string>

// The renderer has one of these to deal with string overlays
class TextRenderer
{
	friend class Renderer;
public:
	TextRenderer(Renderer* rendererIn, HWND hwnd, const std::string& fontFace, int textHeight);

	void drawString(const std::string text, Vec<int> screenPos, Color fg = Color(1.0f,1.0f,1.0f,1.0f), Color bg = Color(0.0f,0.0f,0.0f,0.0f));
	void drawRect(Vec<int> topLeft, Vec<int> size, Color color = Color(1.0f, 1.0f, 1.0f, 1.0f));
	void render();

	~TextRenderer();

private:
	TextRenderer(){}

	static std::map<char, int> buildIdxMap();

	void computePos(const Vec<float> start, const Vec<float> offsets[4], Vec<float> posQuad[4]);
	bool computeUV(const char code, Vec<float> uvQuad[4]);

	static const int maxQuads;
	static const std::string printable;
	static const std::map<char,int> asciiIdxMap;

	Renderer* renderer;

	std::string fontFace;
	int textHeight;

	TextCamera* textCamera;
	GraphicObjectNode* textNode;

	std::shared_ptr<TextQuads> textMesh;
	std::shared_ptr<TextMaterial> textMaterial;

	std::shared_ptr<TextAtlasTexture> textAtlas;
};
