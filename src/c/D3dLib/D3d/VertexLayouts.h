#pragma once

#include "Vec.h"

#include <d3d11.h>

#include <vector>
#include <string>
#include <map>

#define LAYOUT_ELEM(type) (1<<(type))

class VertexLayout
{
	friend class Renderer;
public:
	// Supported vertex attributes
	enum Attributes
	{
		Position = 0,
		Normal = 1,
		TextureUV = 2,
		Color = 3,
		ColorBack = 4,

		ATTR_END
	};

	// Supported vertex layouts
	enum Types
	{
		P	= LAYOUT_ELEM(Attributes::Position),
		PN	= LAYOUT_ELEM(Attributes::Position)	| LAYOUT_ELEM(Attributes::Normal),
		PT	= LAYOUT_ELEM(Attributes::Position)	| LAYOUT_ELEM(Attributes::TextureUV),
		PNT	= LAYOUT_ELEM(Attributes::Position)	| LAYOUT_ELEM(Attributes::Normal)	| LAYOUT_ELEM(Attributes::TextureUV),
		PNC	= LAYOUT_ELEM(Attributes::Position)	| LAYOUT_ELEM(Attributes::Normal)	| LAYOUT_ELEM(Attributes::Color),
		PTC	= LAYOUT_ELEM(Attributes::Position)	| LAYOUT_ELEM(Attributes::TextureUV)| LAYOUT_ELEM(Attributes::Color),
		PTCC = LAYOUT_ELEM(Attributes::Position)| LAYOUT_ELEM(Attributes::TextureUV)| LAYOUT_ELEM(Attributes::Color)	| LAYOUT_ELEM(Attributes::ColorBack),
		PNTC = LAYOUT_ELEM(Attributes::Position) | LAYOUT_ELEM(Attributes::Normal)	| LAYOUT_ELEM(Attributes::TextureUV)| LAYOUT_ELEM(Attributes::Color),
		PNTCC = LAYOUT_ELEM(Attributes::Position) | LAYOUT_ELEM(Attributes::Normal)	| LAYOUT_ELEM(Attributes::TextureUV)| LAYOUT_ELEM(Attributes::Color) | LAYOUT_ELEM(Attributes::ColorBack)
	};

public:
	VertexLayout(Types layoutType = Types::P);

	size_t getVertSize();
	bool validAttribute(Attributes attr){return ((LAYOUT_ELEM(attr) & layoutType) != 0);}
	void* allocLayout(size_t numVerts);
	void sliceIntoLayout(void* vertMem, Attributes attr, size_t numVerts, const float* data, size_t outPitch = 0);

	Types getType(){return layoutType;}

	const std::string& getName();
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& getLayoutDesc();

private:
	// Static helpers for different layout types
	struct Info
	{
		std::string name;
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout;

		int size;
		int offsets[Attributes::ATTR_END];

		Info(Types layout, const std::string& name);
	};

	static const std::string semantic[Attributes::ATTR_END];
	static const uint32_t semanticIdx[Attributes::ATTR_END];

	static const DXGI_FORMAT format[Attributes::ATTR_END];

	static const uint32_t elemSize = sizeof(float);
	static const uint32_t elems[Attributes::ATTR_END];

	static const std::map<Types, Info> layoutInfo;

	// Non-static info
	Types layoutType;
};