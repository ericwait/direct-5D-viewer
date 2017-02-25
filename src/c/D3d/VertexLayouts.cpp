#include "VertexLayouts.h"

const std::string VertexLayout::semantic[Attributes::ATTR_END] = {"POSITION","NORMAL","TEXCOORD","COLOR","COLOR"};
const uint32_t VertexLayout::semanticIdx[Attributes::ATTR_END] ={0,0,0,0,1};

const DXGI_FORMAT VertexLayout::format[Attributes::ATTR_END] = 
		{DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT};

const uint32_t VertexLayout::elems[Attributes::ATTR_END] = {3,3,3,4,4};

const std::map<VertexLayout::Types, VertexLayout::Info> VertexLayout::layoutInfo =
		{{VertexLayout::Types::P, VertexLayout::Info(VertexLayout::Types::P,"P")},
		{VertexLayout::Types::PN, VertexLayout::Info(VertexLayout::Types::PN,"PN")},
		{VertexLayout::Types::PT, VertexLayout::Info(VertexLayout::Types::PT,"PT")},
		{VertexLayout::Types::PNT, VertexLayout::Info(VertexLayout::Types::PNT,"PNT")},
		{VertexLayout::Types::PNC, VertexLayout::Info(VertexLayout::Types::PNC,"PNC")},
		{VertexLayout::Types::PTC, VertexLayout::Info(VertexLayout::Types::PTC,"PTC")},
		{VertexLayout::Types::PTCC, VertexLayout::Info(VertexLayout::Types::PTCC,"PTCC")},
		{VertexLayout::Types::PNTC, VertexLayout::Info(VertexLayout::Types::PNTC,"PNTC")},
		{VertexLayout::Types::PNTCC, VertexLayout::Info(VertexLayout::Types::PNTCC, "PNTCC")}};

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

		D3D11_INPUT_ELEMENT_DESC newDesc;
		newDesc.SemanticName = semantic[i].c_str();
		newDesc.SemanticIndex = semanticIdx[i];
		newDesc.Format = format[i];
		newDesc.InputSlot = 0;
		newDesc.AlignedByteOffset = offsets[i];
		newDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		newDesc.InstanceDataStepRate = 0;

		inputLayout.push_back(newDesc);

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
	return new uint8_t[numVerts * getVertSize()];
}

void VertexLayout::sliceIntoLayout(void* vertMem, Attributes attr, size_t numVerts, const float* data, size_t outPitch)
{
	if ( outPitch == 0 )
		outPitch = getVertSize();

	size_t attrSize = elemSize*elems[attr];
	size_t attrOffset = layoutInfo.at(layoutType).offsets[attr];

	uint8_t* inLine = (uint8_t*)data;
	uint8_t* outLine = (uint8_t*)vertMem;
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
