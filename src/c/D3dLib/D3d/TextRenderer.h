#pragma once

#include "D3d/Renderer.h"
#include "D3d/Texture.h"
#include "D3d/Material.h"
#include "D3d/MeshPrimitive.h"
#include "D3d/Camera.h"

#include "Global/Color.h"

#include <map>
#include <string>

// The renderer has one of these to deal with string overlays
class TextRenderer
{
	friend class Renderer;
public:
	TextRenderer(Renderer* rendererIn, const std::string& fontFace, int textHeight);

	void drawString(const std::string text, Vec<int> baselinePos, Color fg = Color(1.0f,1.0f,1.0f,1.0f), Color bg = Color(0.0f,0.0f,0.0f,0.0f));
	void drawRect(Vec<int> bottomLeft, Vec<int> size, Color color = Color(1.0f, 1.0f, 1.0f, 1.0f));

	int getFontHeight();
	int getFontWidth();

	void render();

	~TextRenderer();

private:
	TextRenderer(){}

	static std::map<char, int> buildIdxMap();

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
