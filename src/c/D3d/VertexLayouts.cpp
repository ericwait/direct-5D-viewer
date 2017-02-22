#include "VertexLayouts.h"

const std::string VertexLayout::semantic[Attributes::ATTR_END] = {"POSITION","NORMAL","TEXCOORD","COLOR"};
const DXGI_FORMAT VertexLayout::format[Attributes::ATTR_END] = {DXGI_FORMAT_R32G32B32_FLOAT,DXGI_FORMAT_R32G32B32_FLOAT,DXGI_FORMAT_R32G32B32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT};
const int VertexLayout::elems[Attributes::ATTR_END] = {3,3,3,4};

const std::map<VertexLayout::Types, VertexLayout::Info> VertexLayout::layoutInfo =
		{{VertexLayout::Types::P, VertexLayout::Info(VertexLayout::Types::P,"P")},
		{VertexLayout::Types::PN, VertexLayout::Info(VertexLayout::Types::PN,"PN")},
		{VertexLayout::Types::PT, VertexLayout::Info(VertexLayout::Types::PT,"PT")},
		{VertexLayout::Types::PNT, VertexLayout::Info(VertexLayout::Types::PNT,"PNT")},
		{VertexLayout::Types::PNC, VertexLayout::Info(VertexLayout::Types::PNC,"PNC")},
		{VertexLayout::Types::PTC, VertexLayout::Info(VertexLayout::Types::PTC,"PTC")},
		{VertexLayout::Types::PNTC, VertexLayout::Info(VertexLayout::Types::PNTC,"PNTC")}};

VertexLayout::Info::Info(VertexLayout::Types layout, const std::string& name)
	: name(name), size(0)
{
	int accumOffset = 0;
	for ( int i=0; i < Attributes::ATTR_END; ++i )
	{
		offsets[i] = -1;
		if ( !(LAYOUT_ELEM(i) & layout) )
			continue;

		offsets[i] = accumOffset;
		inputLayout.push_back({semantic[i].c_str(),0,format[i],0,(UINT)offsets[i],D3D11_INPUT_PER_VERTEX_DATA,0});

		size += elemSize * elems[i];
		accumOffset += elemSize * elems[i];
	}
}

VertexLayout::VertexLayout(VertexLayout::Types layoutType)
	: layoutType(layoutType)
{}

size_t VertexLayout::getVertSize()
{
	return layoutInfo.at(layoutType).size;
}

void* VertexLayout::allocLayout(size_t numVerts)
{
	return new unsigned char[numVerts * getVertSize()];
}

void VertexLayout::sliceIntoLayout(void* vertMem, Attributes attr, size_t numVerts, const float* data, size_t outPitch)
{
	if ( outPitch == 0 )
		outPitch = getVertSize();

	size_t attrSize = elemSize*elems[attr];
	size_t attrOffset = layoutInfo.at(layoutType).offsets[attr];

	unsigned char* inLine = (unsigned char*)data;
	unsigned char* outLine = (unsigned char*)vertMem;
	for ( int i=0; i < numVerts; ++i )
	{
		memcpy(outLine + attrOffset, inLine, attrSize);

		outLine += outPitch;
		inLine += attrSize;
	}
}

const std::string& VertexLayout::getName()
{
	return layoutInfo.at(layoutType).name;
}

const std::vector<D3D11_INPUT_ELEMENT_DESC>& VertexLayout::getLayoutDesc()
{
	return layoutInfo.at(layoutType).inputLayout;
}
