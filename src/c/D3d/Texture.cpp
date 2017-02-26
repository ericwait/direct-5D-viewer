#include "Texture.h"


ConstTextureBase::ConstTextureBase(Renderer* rendererIn)
	: renderer(rendererIn), resourceView(NULL), samplerState(NULL)
{}

ConstTextureBase::~ConstTextureBase()
{
	SAFE_RELEASE(resourceView);
	SAFE_RELEASE(samplerState);
}



Const3DTexture::Const3DTexture(Renderer* rendererIn, Vec<size_t> dims, const unsigned char* texData)
	: ConstTextureBase(rendererIn), dims(dims)
{
	UINT iMipCount = 1;
	UINT BitSize = 0;

	D3D11_TEXTURE3D_DESC desc;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.Width = (unsigned int)dims.x;
	desc.Height = (unsigned int)dims.y;
	desc.Depth = (unsigned int)dims.z;
	desc.MipLevels = iMipCount;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.SysMemPitch = (unsigned int)dims.x;
	initData.SysMemSlicePitch = unsigned int(dims.x*dims.y);
	initData.pSysMem = (void*)texData;

	ID3D11Resource* texture3D = renderer->createTexture3D(&desc, &initData);

	resourceView = renderer->createShaderResourceView(texture3D);
	samplerState = rendererIn->createSamplerState(InterpTypes::Linear);

	SAFE_RELEASE(texture3D);
}



TextAtlasTexture::TextAtlasTexture(Renderer* rendererIn, HWND hwnd, const std::string& fontFace, int textHeight, const std::string& charList)
	: ConstTextureBase(rendererIn)
{
	// Use GDI to render the character list to a memory buffer.
	//uint8_t* atlas = NULL;
	size_t atlasSize = createTextAtlas(hwnd, fontFace, textHeight, charList, &atlas);
	if ( atlasSize == 0 )
		std::runtime_error("Unable to create atlas for font!");

	// Create 2D texture from memory buffer
	UINT iMipCount = 1;
	UINT BitSize = 0;

	D3D11_TEXTURE2D_DESC desc;
	desc.Format = DXGI_FORMAT_A8_UNORM;
	desc.Width = atlasDims.x;
	desc.Height = atlasDims.y;
	desc.MipLevels = iMipCount;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.SysMemPitch = atlasDims.x;
	initData.SysMemSlicePitch = atlasDims.y;
	initData.pSysMem = (void*)atlas;

	ID3D11Resource* texture2D = renderer->createTexture2D(&desc, &initData);

	resourceView = renderer->createShaderResourceView(texture2D);
	samplerState = rendererIn->createSamplerState(InterpTypes::Nearest);

	SAFE_RELEASE(texture2D);
	//delete[] atlas;
};

#define CEIL_DIV(n,d) (((n)+(d)-1)/(d))
#define ROUND_DIV(n,d) (((n)+(n)+(d))/((d)+(d)))

bool TextAtlasTexture::computeAtlasSize()
{
	const int minDim = 32;
	const int maxDim = 2048;

	// Figure out a square-ish texture size for the atlas
	int width = 0;
	for ( int w = minDim; w <= maxDim; w = w << 1 )
	{
		int boxes = (w / charBox.x) * (w / charBox.y);
		if ( boxes < numChars )
			continue;

		width = w;
		break;
	}

	if ( width == 0 )
		return false;

	int minRows = CEIL_DIV(numChars, (width / charBox.x));
	int minHeight = minRows * charBox.y;

	// Find the smallest power of 2 height that works
	int height = 0;
	for ( int h = minDim; h <= maxDim; h = h << 1 )
	{
		if ( h < minHeight )
			continue;

		height = h;
		break;
	}

	atlasDims = Vec<size_t>(width,height,1);
	atlasBoxes = atlasDims / charBox;

	return true;
}

size_t TextAtlasTexture::createTextAtlas(HWND hwnd, const std::string& fontFace, int textHeight, const std::string& charList, uint8_t** atlasOut)
{
	HDC windowDC = GetDC(hwnd);

	// Use a fixed-width font to that it is simple to layout the characters (also non-antialiased since it's going to be texture sampled)
	HFONT hFont = CreateFont(textHeight, 0,	0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, FIXED_PITCH|FF_DONTCARE, fontFace.c_str());

	HDC hdcMem = CreateCompatibleDC(windowDC);
	if ( !hdcMem )
		return 0;

	// Select font into DC to get preliminary text metrics so we can correctly size the texture
	SelectObject(hdcMem, (HGDIOBJ)hFont);

	TEXTMETRIC metrics;
	GetTextMetrics(hdcMem, &metrics);

	// Compute total atlas dimensions from the character box size
	numChars = charList.length();
	charBox = Vec<size_t>(metrics.tmMaxCharWidth, metrics.tmHeight, 1);
	if ( !computeAtlasSize() )
	{
		ReleaseDC(hwnd, windowDC);
		DeleteDC(hdcMem);
		DeleteObject(hFont);
		return 0;
	}

	// Set tight character dimensions (for quad coords and UVs)
	charSize = Vec<size_t>(metrics.tmAveCharWidth, metrics.tmHeight, 1);

	HBITMAP hBMP = CreateCompatibleBitmap(windowDC, atlasDims.x, atlasDims.y);
	SelectObject(hdcMem, hBMP);

	// Only needed the window device for creating a compatible bitmap
	ReleaseDC(hwnd, windowDC);


	// Clear background to black
	HBRUSH hbr = CreateSolidBrush(0x00000000);
	RECT rc ={0,0, atlasDims.x,atlasDims.y};
	FillRect(hdcMem, &rc, hbr);

	// Prepare to draw each character
	SetMapMode(hdcMem, MM_TEXT);
	SetTextColor(hdcMem, 0x00FFFFFF);
	SetBkMode(hdcMem, TRANSPARENT);
	SetTextAlign(hdcMem, TA_TOP | TA_LEFT);

	char charStr[2] = {'\0','\0'};

	// Render the characters individually to guarantee box-spacing
	SelectObject(hdcMem, (HGDIOBJ)hFont);
	for ( int i=0; i < charList.length(); ++i )
	{
		int idxX = i % atlasBoxes.x;
		int idxY = i / atlasBoxes.x;

		charStr[0] = charList[i];
		TextOut(hdcMem, charBox.x * idxX, charBox.y * idxY, charStr, 1);
	}

	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = atlasDims.x;
	bi.biHeight = -atlasDims.y;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	size_t pixelSize = bi.biBitCount / 8;

	size_t pixelRow = atlasDims.x * pixelSize;
	size_t rowPad = ((~(pixelRow & 0x03) + 1) & 0x03);
	size_t rowSize = ((pixelRow >> 2) + (rowPad > 0)) << 2;

	size_t bmpSize = rowSize * atlasDims.y;

	// Load device independent bitmap data into memory
	uint8_t* bmpData = new uint8_t[bmpSize];
	GetDIBits(hdcMem, hBMP, 0, (UINT)atlasDims.y, bmpData, (BITMAPINFO*) &bi, DIB_RGB_COLORS);

	// Can dump all the surface and graphic objects now
	DeleteDC(hdcMem);
	DeleteObject(hFont);
	DeleteObject(hBMP);
	DeleteObject(hbr);


	// Create a single channel version of the texture using averaging (in case we change to antialiased fonts)
	uint8_t* outData = new uint8_t[atlasDims.product()];
	memset(outData, 0, atlasDims.product());

	for ( int y = 0; y < atlasDims.y; ++y )
	{
		for ( int x=0; x < atlasDims.x; ++x )
		{
			size_t outIdx = x + y*atlasDims.x;
			size_t pixelStart = x*pixelSize + y*rowSize;

			int accum = 0;
			for ( int c=0; c < 3; ++c )
				accum += bmpData[pixelStart + c];

			outData[outIdx] = (uint8_t) ROUND_DIV(accum,3);
		}
	}

	delete[] bmpData;

	*atlasOut = outData;
	return atlasDims.product();
}
