#include "TextRenderer.h"
#include "SceneNode.h"

#include "Global/Globals.h"

const int TextRenderer::maxQuads = 512;
const std::string TextRenderer::printable = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
const std::map<char, int> TextRenderer::asciiIdxMap = buildIdxMap();

std::map<char,int> TextRenderer::buildIdxMap()
{
	std::map<char,int> idxMap;
	for ( int i=0; i < TextRenderer::printable.length(); ++i )
		idxMap.insert(std::make_pair(printable[i],i));

	return idxMap;
}


TextRenderer::TextRenderer(Renderer* rendererIn, HWND hwnd, const std::string& fontFace, int textHeight)
	: renderer(rendererIn), fontFace(fontFace), textHeight(textHeight), textNode(NULL)
{
	textAtlas = std::make_shared<TextAtlasTexture>(renderer, hwnd, fontFace, textHeight, printable);

	textMesh = std::make_shared<TextQuads>(renderer, maxQuads);

	textMaterial = std::make_shared<TextMaterial>(renderer);
	textMaterial->attachTexture(0, textAtlas);

	// Note: This doesn't get attached to the scene
	textNode = new GraphicObjectNode(0, GraphicObjectTypes::Text, textMesh, textMaterial);
}

TextRenderer::~TextRenderer()
{
	SAFE_DELETE(textNode);
}

void TextRenderer::drawString(const std::string text, Vec<int> screenPos, Color fg, Color bg)
{
	const int mult = 1;

	int xSize = mult*(textAtlas->charSize.x);
	int ySize = mult*(textAtlas->charSize.y);

	Vec<float> uvQuad[4];
	Vec<float> posQuad[4];
	Vec<float> posOffset[4] = {Vec<float>(0.0f,0.0f,0.0f),
								Vec<float>(0.0f,ySize,0.0f),
								Vec<float>(xSize,ySize,0.0f),
								Vec<float>(xSize,0.0f,0.0f)};

	int writeCount = 0;

	Vec<float> cursor(screenPos.x, screenPos.y, 0.0f);

	for ( int i=0; i < text.length(); ++i )
	{
		// Skip spaces
		if ( text[i] == ' ' )
		{
			cursor += posOffset[3] + Vec<float>(1.0f, 0.0f, 0.0f);
			continue;
		}

		// Skip anything that doesn't map to texture
		bool foundMap = computeUV(text[i], uvQuad);
		if ( !foundMap )
		{
			cursor += posOffset[3] + Vec<float>(1.0f, 0.0f, 0.0f);
			continue;
		}

		computePos(cursor, posOffset, posQuad);

		// If we've run out of quads we have to render what's already there
		bool added = textMesh->addQuad(posQuad, uvQuad, fg, bg);
		if ( !added )
		{
			render();
			added = textMesh->addQuad(posQuad, uvQuad, fg, bg);
		}

		cursor += posOffset[3] + Vec<float>(0.0f,0.0f,0.0f);
	}

}

void TextRenderer::render()
{
	textMesh->updateResources();
	renderer->renderNode(gCameraText, textNode);

	textMesh->clearQuads();
}

void TextRenderer::computePos(const Vec<float> start, const Vec<float> offsets[4], Vec<float> posQuad[4])
{
	for ( int i=0; i < 4; ++i )
		posQuad[i] = start + offsets[i];
}

bool TextRenderer::computeUV(const char code, Vec<float> uvQuad[4])
{
	if ( asciiIdxMap.count(code) == 0 )
		return false;

	int mapIdx = asciiIdxMap.at(code);
	int boxX = mapIdx % textAtlas->atlasBoxes.x;
	int boxY = mapIdx / textAtlas->atlasBoxes.x;

	int sizeX = textAtlas->charSize.x;
	int sizeY = textAtlas->charSize.y;

	Vec<float> pixStart = Vec<size_t>(boxX,boxY,1) * textAtlas->charBox;
	uvQuad[0] = pixStart / textAtlas->atlasDims;
	uvQuad[1] = (pixStart + Vec<float>(0.0f,sizeY,0.0f)) / textAtlas->atlasDims;
	uvQuad[2] = (pixStart + Vec<float>(sizeX, sizeY, 0.0f)) / textAtlas->atlasDims;
	uvQuad[3] = (pixStart + Vec<float>(sizeX, 0.0f, 0.0f)) / textAtlas->atlasDims;

	return true;
}
