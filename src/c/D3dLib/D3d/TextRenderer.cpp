#include "D3d/TextRenderer.h"
#include "D3d/SceneNode.h"

#include "Global/Globals.h"

const int TextRenderer::maxQuads = 512;

TextRenderer::TextRenderer(Renderer* rendererIn, const std::string& fontFace, int textHeight)
	: renderer(rendererIn), fontFace(fontFace), textHeight(textHeight), textNode(NULL)
{
	textAtlas = std::make_shared<TextAtlasTexture>(renderer, fontFace, textHeight, 33, (127-33));
	
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

void TextRenderer::drawString(const std::string text, Vec<int> baselinePos, Color fg, Color bg)
{
	const int mult = 1;

	Vec<float> uvQuad[4];
	Vec<float> posQuad[4];

	int writeCount = 0;

	Vec<float> cursor(baselinePos.x, baselinePos.y, 0.0f);

	for ( int i=0; i < text.length(); ++i )
	{
		if (textAtlas->getWhitespaceOffset(text[i], baselinePos.x, cursor))
			continue;

		if (!textAtlas->getCharQuad(text[i], cursor, posQuad, uvQuad))
			continue;

		// If we've run out of quads we have to render what's already there
		bool added = textMesh->addQuad(posQuad, uvQuad, fg, bg);
		if ( !added )
		{
			render();
			added = textMesh->addQuad(posQuad, uvQuad, fg, bg);
		}
	}

}

void TextRenderer::drawRect(Vec<int> bottomLeft, Vec<int> size, Color color)
{
	Vec<float> uvQuad[4] = {Vec<float>(0.0f),Vec<float>(0.0f),
							Vec<float>(0.0f),Vec<float>(0.0f)};

	Vec<float> posQuad[4];

	Color bgTrnspt(0.0f, 0.0f, 0.0f, 0.0f);

	posQuad[0] = Vec<float>(bottomLeft.x, bottomLeft.y-size.y, 0.0f);
	posQuad[1] = Vec<float>(bottomLeft.x, bottomLeft.y, 0.0f);
	posQuad[2] = Vec<float>(bottomLeft.x+size.x, bottomLeft.y, 0.0f);
	posQuad[3] = Vec<float>(bottomLeft.x+size.x, bottomLeft.y-size.y, 0.0f);

	// Flip the fg/bg colors, lets us use the text shader for rendering color quads
	bool added = textMesh->addQuad(posQuad, uvQuad, bgTrnspt, color);
	if ( !added )
	{
		render();
		added = textMesh->addQuad(posQuad, uvQuad, bgTrnspt, color);
	}
}

int TextRenderer::getFontHeight()
{
	return textAtlas->maxCharBox.y;
}

int TextRenderer::getFontWidth()
{
	return textAtlas->maxCharBox.x;
}

void TextRenderer::render()
{
	textMesh->updateResources();
	renderer->renderNode(gCameraText, textNode);

	textMesh->clearQuads();
}
