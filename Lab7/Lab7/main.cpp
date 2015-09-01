// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************


#include <iostream>
#include <ctime>
#include "XTime.h"
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"
#include "VS_Grid.csh"
#include "PS_Grid.csh"
#include "VS_Star.csh"
#include "windows.h"
#include "..\Assets\Assets\Models\Cube.h"
#include "..\Assets\Textures\Cowboy.h"
#include "Grid.h"
#include "..\Assets\Textures\numbers_test.h"

using namespace std;
// BEGIN PART 1
// TODO: PART 1 STEP 1a
#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")

// TODO: PART 1 STEP 1b
//#include <DirectXMath.h>
#include "mathlib.h"
//using namespace DirectX;
// TODO: PART 2 STEP 6

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

// this needs to match 
//The input-layout description
//D3D11_INPUT_ELEMENT_DESC layout[] =
//{
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//};

POINT cursor_previous;
POINT cursor_current;
IDXGISwapChain *chain = nullptr;

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2

	DXGI_SWAP_CHAIN_DESC *desk = nullptr;
	ID3D11Texture2D* BackBuffer = nullptr;
	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *deviceContext = nullptr;
	ID3D11RenderTargetView *renderTargetView = nullptr;
	ID3D11InputLayout* pInputLayout = nullptr;
	D3D11_VIEWPORT viewPort;
	DXGI_MODE_DESC bufferDesctoFill;
	
	ID3D11Buffer * VertBuffer;
	ID3D11Buffer * VertBuffer_triangle;
	ID3D11Buffer * VertBufferStar;
	ID3D11Buffer * VertBufferCube; // << unload this
	ID3D11Buffer * VertBufferGrid;
	ID3D11Buffer * IndexBufferStar;
	ID3D11Buffer * IndexBufferCube;  //<<<<  make this safe
	ID3D11Buffer * IndexBufferGrid;
	ID3D11Buffer * VS_Buffer = nullptr;
	ID3D11Buffer * PS_Buffer = nullptr;
	ID3D11Buffer * constantBuffer;
	ID3D11Buffer * constantBuffer_offset; //<< clear later
	ID3D11Buffer * constantBuffer_Camera;

	ID3D11Resource *chainBuffer = nullptr;
	ID3D11ShaderResourceView* CubesTexture = nullptr;
	ID3D11SamplerState* CubesTexSamplerState = nullptr;

	ID3D11PixelShader* PS = nullptr;
	ID3D11PixelShader* PS_Grid_pointer = nullptr;
	ID3D11VertexShader* VS = nullptr;
	ID3D11VertexShader* VSStar = nullptr;
	ID3D11VertexShader* VSGrid_p = nullptr;

	ID3D11DepthStencilView* pDSV;

	ID3D11Texture2D* pDepthStencil = NULL;

	// All Shader Variables (Always Pre-fixed by “SV_”)
	MY_MATRIX_4X4 SV_WorldMatrix;
	MY_MATRIX_4X4 SV_ViewMatrix;
	MY_MATRIX_4X4 SV_Perspective;



	MY_MATRIX_4X4 Rotate = IdentityMatrix();
	MY_MATRIX_4X4 TranslateMatrix = IdentityMatrix();

	ID3D11BlendState * blendState;
	ID3D11BlendState* Transparency;
	
	ID3D11RasterizerState* rasterizerState;
	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;



	// grid stuff
//	ID3D11PixelShader* PS_Grid = nullptr;
//	ID3D11VertexShader* VS_Grid = nullptr;

	double deltatime = 0.0f;
	float moveZ = -1.0f;
	float moveX = 0.0f;
	float moveY = 0.0f;
	float Aspect = 1.0f;
	bool antializing = false;
	float offset;
	// BEGIN PART 3
	// TODO: PART 3 STEP 1
	XTime xTime;
	// TODO: PART 3 STEP 2b

	struct SEND_TO_VRAM
	{
		float constantColor[4];
		float constantOffset[2];
		float padding[2];
	};
	// TODO: PART 3 STEP 4a
	SEND_TO_VRAM toShader;

public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void ResizeWindow();
};




//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;	// this is where the window will be stretched, make changes here for last class					
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW /*& ~(WS_THICKFRAME | WS_MAXIMIZEBOX)*/,
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a

	ZeroMemory(&bufferDesctoFill, sizeof(DXGI_MODE_DESC));
	bufferDesctoFill.Height = BACKBUFFER_WIDTH;
	bufferDesctoFill.Width = BACKBUFFER_HEIGHT;
	bufferDesctoFill.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesctoFill.RefreshRate.Denominator = 1;

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };

	ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	DXGI_SAMPLE_DESC sample_descriptor;
	ZeroMemory(&sample_descriptor, sizeof(DXGI_SAMPLE_DESC));
	sample_descriptor.Count = 1;
	sample_descriptor.Quality = 0; // standard multisample patter

	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc = bufferDesctoFill;
	swap_chain_desc.SampleDesc = sample_descriptor;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // set these flags and add the others as ouput
	swap_chain_desc.OutputWindow = window;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//swap_chain_desc.SampleDesc.Count = 1;
	//swap_chain_desc.SampleDesc.Quality = 0;
	D3D_FEATURE_LEVEL pFeatures[6] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	D3D_FEATURE_LEVEL pFeatureLevel;

#if _DEBUG																																																	// this will happen later on 
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,						    // this can be used to chose the default adapter
		D3D_DRIVER_TYPE_HARDWARE,		// this where we can pug in your own special rasterizer, not used in the modern day.
		NULL,							// umake this 0, 
		D3D11_CREATE_DEVICE_DEBUG,	    // you put in interesting flags. can also use the// set flag in release 
		pFeatures,						// this will work in the enum for backwards compataility, pass null in here.
		6,								// the amount levels above you uses
		D3D11_SDK_VERSION,				// outpts
		&swap_chain_desc,				// 
		&chain,
		&device,
		&pFeatureLevel,					// this tells you wat verrsion 
		&deviceContext);
#else
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, GetModuleHandle(L"d3d11.dll"), 0, nullptr, 6, D3D11_SDK_VERSION, &swap_chain_desc, &chain, &device, nullptr, nullptr);
	// deviceContext issues comads directly to the hardware
#endif

	
	// TODO: PART 1 STEP 3b
	
	
	// fill the back buffer
	hr = chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&chainBuffer);

	//Create the Render Target
	hr = device->CreateRenderTargetView(chainBuffer, NULL, &renderTargetView);

//	texture->Release();

	// TODO: PART 1 STEP 5

	// this a window in the backbuffer 
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = BACKBUFFER_WIDTH;
	viewPort.Height = BACKBUFFER_HEIGHT;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	// TODO: PART 2 STEP 3a
	

	/////////////////////////////////////////////////////////////// end lab 7




D3D11_BUFFER_DESC verteciesBufferDesc_cube;
ZeroMemory(&verteciesBufferDesc_cube, sizeof(verteciesBufferDesc_cube));

verteciesBufferDesc_cube.Usage = D3D11_USAGE_IMMUTABLE;
verteciesBufferDesc_cube.BindFlags = D3D11_BIND_VERTEX_BUFFER;
verteciesBufferDesc_cube.ByteWidth = sizeof(Cube_data);
verteciesBufferDesc_cube.MiscFlags = 0;
verteciesBufferDesc_cube.CPUAccessFlags = 0;
verteciesBufferDesc_cube.StructureByteStride = 0;

D3D11_SUBRESOURCE_DATA vertexBufferData_cube;
ZeroMemory(&vertexBufferData_cube, sizeof(vertexBufferData_cube));

vertexBufferData_cube.pSysMem = Cube_data;
vertexBufferData_cube.SysMemPitch = 0;
vertexBufferData_cube.SysMemSlicePitch = 0;

hr = device->CreateBuffer(&verteciesBufferDesc_cube, &vertexBufferData_cube, &VertBufferCube);


//////////////////////////////////////////////////

D3D11_BUFFER_DESC indexBufferData_cube = { 0 };
indexBufferData_cube.Usage = D3D11_USAGE_IMMUTABLE;
indexBufferData_cube.BindFlags = D3D11_BIND_INDEX_BUFFER;
indexBufferData_cube.ByteWidth = sizeof(Cube_indicies);
indexBufferData_cube.MiscFlags = 0;
indexBufferData_cube.CPUAccessFlags = 0;
indexBufferData_cube.StructureByteStride = 0;

D3D11_SUBRESOURCE_DATA indexBufferDataSR_cube = { 0 };
indexBufferDataSR_cube.pSysMem = Cube_indicies;
indexBufferDataSR_cube.SysMemPitch = 0;
indexBufferDataSR_cube.SysMemSlicePitch = 0;

hr = device->CreateBuffer(&indexBufferData_cube, &indexBufferDataSR_cube, &IndexBufferCube);

device->CreateVertexShader(VS_Star, sizeof(VS_Star), nullptr, &VSStar);
device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &PS); // first create the shaders 

D3D11_INPUT_ELEMENT_DESC vLayout[] =
{
	{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UVM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NRM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////// drawing star END

// GRID Stuff /////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// TODO: PART 3 STEP 3
	D3D11_BUFFER_DESC constBufferDesc = { 0 };
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//constBufferDesc.ByteWidth = sizeof(SEND_TO_VRAM);
	constBufferDesc.ByteWidth = sizeof(SV_WorldMatrix);
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA constBuffer_resource = { 0 };
	device->CreateBuffer(&constBufferDesc, nullptr, &constantBuffer);

	// 
	D3D11_BUFFER_DESC constBufferDesc_offset = { 0 };
	constBufferDesc_offset.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc_offset.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc_offset.ByteWidth = sizeof(float) * 4;
	constBufferDesc_offset.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA constBuffer_resource_offset = { 0 };
	device->CreateBuffer(&constBufferDesc_offset, nullptr, &constantBuffer_offset);


	// TODO: PART 3 STEP 4b
	//toShader = { 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	D3D11_BUFFER_DESC constBufferDesc_camera = { 0 };
	constBufferDesc_camera.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc_camera.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//constBufferDesc.ByteWidth = sizeof(SEND_TO_VRAM);
	constBufferDesc_camera.ByteWidth = sizeof(MY_MATRIX_4X4) * 2;
	constBufferDesc_camera.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&constBufferDesc_camera, nullptr, &constantBuffer_Camera);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	// Create the depth stencil view
	
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	
	hr = device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);


	hr = device->CreateDepthStencilView(pDepthStencil, // Depth stencil texture
		&descDSV,										// Depth stencil desc
		&pDSV);											// [out] Depth stencil view

	// TEXTURE DESCRIPTION 




	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd[numbers_test_numlevels];
	ZeroMemory(tbsd, sizeof(tbsd));
	for (unsigned int i = 0; i < numbers_test_numlevels; i++)
	{
		tbsd[i].pSysMem = &numbers_test_pixels[numbers_test_leveloffsets[i]] ;
		tbsd[i].SysMemPitch = sizeof(UINT) * (numbers_test_width >> i);
	}


	tdesc.Width = numbers_test_width ;
	tdesc.Height = numbers_test_height;
	tdesc.MipLevels = numbers_test_numlevels;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tdesc.CPUAccessFlags = 0;
	tdesc.MiscFlags = 0;

	device->CreateTexture2D(&tdesc, tbsd, &tex);
	hr = device->CreateSamplerState(&samplerDesc, &CubesTexSamplerState);
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceView_desc;
	shaderResourceView_desc.Format = tdesc.Format;
	shaderResourceView_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceView_desc.Texture2D.MostDetailedMip = 0;
	shaderResourceView_desc.Texture2D.MipLevels = numbers_test_numlevels;
	device->CreateShaderResourceView(tex, NULL, &CubesTexture);
	
	SAFE_RELEASE(tex);

	// BUILDING THE GRID 

	float x = -1.0f;
	float z = 1.0f;

	for (UINT i = 0; i < 21; i++)
	{
		// back row
		grid_data_new[i].pos[0] = x;// x
		grid_data_new[i].pos[1] = 0.0f;// y
		grid_data_new[i].pos[2] = 1.0f;// z

		// front row
		grid_data_new[i + 21].pos[0] = x;// x
		grid_data_new[i + 21].pos[1] = 0.0f;// y
		grid_data_new[i + 21].pos[2] = -1.0f;// z
		x += 0.1;

		// left row 
		grid_data_new[i + 42].pos[0] = -1.0f;// x
		grid_data_new[i + 42].pos[1] = 0.0f;// y
		grid_data_new[i + 42].pos[2] = z;// z

		// right row
		grid_data_new[i + 63].pos[0] = 1.0f;// x
		grid_data_new[i + 63].pos[1] = 0.0f;// y
		grid_data_new[i + 63].pos[2] = z;// z
		z -= 0.1f;
	};

	D3D11_BUFFER_DESC verteciesBufferDesc_grid;
	ZeroMemory(&verteciesBufferDesc_grid, sizeof(verteciesBufferDesc_grid));

	verteciesBufferDesc_grid.Usage = D3D11_USAGE_IMMUTABLE;
	verteciesBufferDesc_grid.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	verteciesBufferDesc_grid.ByteWidth = sizeof(grid_data_new);
	verteciesBufferDesc_grid.MiscFlags = 0;
	verteciesBufferDesc_grid.CPUAccessFlags = 0;
	verteciesBufferDesc_grid.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData_grid;
	ZeroMemory(&vertexBufferData_grid, sizeof(vertexBufferData_grid));

	vertexBufferData_grid.pSysMem = grid_data_new;
	vertexBufferData_grid.SysMemPitch = 0;
	vertexBufferData_grid.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&verteciesBufferDesc_grid, &vertexBufferData_grid, &VertBufferGrid);


	D3D11_BUFFER_DESC indexBufferData_grid = { 0 };
	indexBufferData_grid.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferData_grid.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferData_grid.ByteWidth = sizeof(grid_indicies_new);
	indexBufferData_grid.MiscFlags = 0;
	indexBufferData_grid.CPUAccessFlags = 0;
	indexBufferData_grid.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferDataSR_grid = { 0 };
	indexBufferDataSR_grid.pSysMem = grid_indicies_new;
	indexBufferDataSR_grid.SysMemPitch = 0;
	indexBufferDataSR_grid.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&indexBufferData_grid, &indexBufferDataSR_grid, &IndexBufferGrid);


	device->CreatePixelShader(PS_Grid, sizeof(PS_Grid), nullptr, &PS_Grid_pointer);
	device->CreateVertexShader(VS_Grid, sizeof(VS_Grid), nullptr, &VSGrid_p);
	device->CreateInputLayout(vLayout, 3, VS_Grid, sizeof(VS_Grid), &pInputLayout);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ANTIALISING 

	D3D11_RASTERIZER_DESC rasterizer_Description;
	ZeroMemory(&rasterizer_Description, sizeof(rasterizer_Description));
	rasterizer_Description.AntialiasedLineEnable = antializing;
	rasterizer_Description.FillMode = D3D11_FILL_SOLID;
	rasterizer_Description.CullMode = D3D11_CULL_NONE;

	hr = device->CreateRasterizerState(&rasterizer_Description, &rasterizerState);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// BLENDSTATE

	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.RenderTarget[0] = rtbd;

	device->CreateBlendState(&blendStateDesc, &Transparency);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// rasterizer description 

	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = device->CreateRasterizerState(&cmdesc, &CWcullMode);

	//*******************************************************
	// Member function for window resize 

	ResizeWindow();



}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{

	xTime.Signal();
	deltatime += xTime.Delta();

	// TODO: PART 4 STEP 2	

	// TODO: PART 4 STEP 3

	// TODO: PART 4 STEP 5

	// END PART 4

	// TODO: PART 1 STEP 7a

	// building the view matrix 


	SV_Perspective = Perspective_Projection_Matrix(90.0f, 100.0f, 0.1f, Aspect); // <<<<<< Projection 
	SV_WorldMatrix = IdentityMatrix();
	SV_ViewMatrix = IdentityMatrix();

//	MY_MATRIX_4X4 Rotate = IdentityMatrix();
	MY_MATRIX_4X4 TranslateMatrix = IdentityMatrix();

	// MMOVEMENT STUFF
	if (GetAsyncKeyState('S'))
	{
		moveZ -= MOVE_SPEED;

	}
	else
		if (GetAsyncKeyState('W'))
		{
			moveZ += MOVE_SPEED;

			if (moveZ >= -1.00f)
			{
				moveZ = -1.0f;
			}
		}
		else
			if (GetAsyncKeyState('A'))
			{
				moveX -= MOVE_SPEED;
			}
			else
				if (GetAsyncKeyState('D'))
				{
					moveX += MOVE_SPEED;
				}
				else
					if (GetAsyncKeyState('R'))
					{
						moveY += MOVE_SPEED;
					}
					else
						if (GetAsyncKeyState('F'))
						{
							moveY -= MOVE_SPEED;
						}
						else
							if (moveZ > 1.0f)
							{
								moveX = -1.0f;
							}
							else if (GetAsyncKeyState('4'))
							{
								antializing = !antializing;
								D3D11_RASTERIZER_DESC rasterizer_Description;
								ZeroMemory(&rasterizer_Description, sizeof(rasterizer_Description));
								rasterizer_Description.AntialiasedLineEnable = antializing;
								rasterizer_Description.FillMode = D3D11_FILL_SOLID;
								rasterizer_Description.CullMode = D3D11_CULL_NONE;
								device->CreateRasterizerState(&rasterizer_Description, &rasterizerState);
							}

	MY_MATRIX_4X4 TranslateMatrix_Save = Translate(TranslateMatrix, moveX, moveY, moveZ);

	TranslateMatrix = Translate(IdentityMatrix(), moveX, moveY, moveZ);


	float dy = 0.0f;
	float dx = 0.0f;
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		GetCursorPos(&cursor_current);

		dx = cursor_previous.x - cursor_current.x;
		dy = cursor_previous.y - cursor_current.y;

		if (dx < 0.0f)
		{
			Rotate = ViewMatrix_RoateY(Rotate, DegToRad( dx ));

		}
		else if (dx > 0.0f)
		{
			Rotate = ViewMatrix_RoateY(Rotate, DegToRad(dx ));


		} else 

		if (dy < 0.0f)
		{
			Rotate = ViewMatrix_RoateX(Rotate, DegToRad(dy ));

		}
		else
		if (dy > 0.0f)
		{
			Rotate = ViewMatrix_RoateX(Rotate, DegToRad( dy ));
		}
		cursor_previous = cursor_current;

	}

	//Rotate = ViewMatrix_RoateX(Rotate, DegToRad(0.0f));            // rotate
	TranslateMatrix = Translate(TranslateMatrix_Save, moveX, moveY, moveZ);

	//	TranslateMatrix = Translate(TranslateMatrix, moveX, moveY, moveZ); // translation 
	SV_ViewMatrix = Matrix_Matrix_Multiply(TranslateMatrix, Rotate);
	SV_ViewMatrix = Inverse(SV_ViewMatrix);                          //   <<<<<<< VIEW
	
	

	deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL); // this is thwre the Z bugger will go 
	// TODO: PART 1 STEP 7b
	//deviceContext->RSSetViewports(1, &viewPort);
	// TODO: PART 1 STEP 7c
	float color_array[4] = { 0, 0, 0.3f, 0.4f };
	deviceContext->ClearRenderTargetView(renderTargetView, color_array);

	// Create the depth stencil view
	deviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	
	
	//********************************************************************************************//
	// setting up blending

	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };

	// sets up the default blendstate for opaique objects 
	deviceContext->OMSetBlendState(0, 0, 0xffffffff);
	//Set the blend state for transparent objects
	deviceContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);
	
 

	// Drawing Grid 
	const UINT stride = sizeof(OBJ_VERT);
	UINT offest = 0;
	deviceContext->IASetVertexBuffers(0, 1, &VertBufferGrid, &stride, &offest);
	deviceContext->IASetIndexBuffer(IndexBufferGrid, DXGI_FORMAT_R32_UINT, offest);
	// TODO: PART 2 STEP 9b
	deviceContext->VSSetShader(VSGrid_p, nullptr, NULL);
	deviceContext->PSSetShader(PS, nullptr, NULL);
	// TODO: PART 2 STEP 9c
	deviceContext->IASetInputLayout(pInputLayout);
	// TODO: PART 2 STEP 9d

	// TEXTURE

	SV_WorldMatrix = IdentityMatrix();
	float offset_for0;
	D3D11_MAPPED_SUBRESOURCE mapped_resource = { 0 };
	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &SV_WorldMatrix, sizeof(SV_WorldMatrix));
	deviceContext->Unmap(constantBuffer, 0);
	// TODO: PART 3 STEP 6
	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // the 0 is slot index 0, 1 is the num buffers 

	//deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);

	deviceContext->PSSetShaderResources(0, 1, &CubesTexture);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);


	deviceContext->PSSetShader(PS_Grid_pointer, nullptr, NULL);
	deviceContext->RSSetState(rasterizerState);

	// old code
	//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP); // for triangles
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->DrawIndexed(ARRAYSIZE(grid_indicies_new), 0, 0);



	
	float offset;
	mapped_resource = { 0 };
	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &SV_WorldMatrix, sizeof(SV_WorldMatrix));
	deviceContext->Unmap(constantBuffer, 0);
	// TODO: PART 3 STEP 6
	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // the 0 is slot index 0, 1 is the num buffers 
	
	


	offset = 0.0f;
	if (deltatime > 2 )
	{
		offset = 0.25f;
	} 
	if (deltatime > 4)
	{
		offset = 0.5f;
	}
	if (deltatime > 6)
	{
		offset = 0.75f;
	}
	if (deltatime > 8)
	{
		deltatime = 0.0f;
	}


	D3D11_MAPPED_SUBRESOURCE mapped_resource_offest = { 0 };
	deviceContext->Map(constantBuffer_offset, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource_offest);
	memcpy(mapped_resource_offest.pData, &offset, sizeof(float));
	deviceContext->Unmap(constantBuffer_offset, 0);
	// TODO: PART 3 STEP 6
	deviceContext->PSSetConstantBuffers(0, 1, &constantBuffer_offset); // the 0 is slot index 0, 1 is the num buffers 




	//mapped_resource = { 0 };
	MY_MATRIX_4X4 scene[2] = { SV_ViewMatrix, SV_Perspective };
	deviceContext->Map(constantBuffer_Camera, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, scene, sizeof(scene));
	deviceContext->Unmap(constantBuffer_Camera, 0);
	// TODO: PART 3 STEP 6
	deviceContext->VSSetConstantBuffers(1, 1, &constantBuffer_Camera); // 




	// rotates the cupe matrix
	SV_WorldMatrix = RotateOnY(SV_WorldMatrix, DegToRad(deltatime*20.0f));
	
	offset_for0 = 0.0f;
	 mapped_resource = { 0 };
	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &SV_WorldMatrix, sizeof(SV_WorldMatrix));
	deviceContext->Unmap(constantBuffer, 0);
	// TODO: PART 3 STEP 6
	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // the 0 is slot index 0, 1 is the num buffers 




	// Bind the depth stencil view
	deviceContext->OMSetRenderTargets(1,          // One rendertarget view
		&renderTargetView,						 // Render target view, created earlier
		pDSV);									// Depth stencil view for the render target

	// TODO: PART 2 STEP 9a
	//VertBuffer

	// // old code 
	//const UINT stride = sizeof(SIMPLE_VERTEX);

	// OLD LAB 7
	//	deviceContext->IASetVertexBuffers(0, 1, &VertBuffer, &stride, &offest);
	//	deviceContext->IASetVertexBuffers(0, 1, &VertBuffer_triangle, &stride, &offest); // new code !   <<<<<<<<<<<<<<<<<<<<<<<
	deviceContext->IASetVertexBuffers(0, 1, &VertBufferCube, &stride, &offest);
	deviceContext->IASetIndexBuffer(IndexBufferCube, DXGI_FORMAT_R32_UINT, offest);
	// TODO: PART 2 STEP 9b
	deviceContext->VSSetShader(VSStar, nullptr, NULL);
	deviceContext->PSSetShader(PS, nullptr, NULL);
	// TODO: PART 2 STEP 9c
	deviceContext->IASetInputLayout(pInputLayout);
	// TODO: PART 2 STEP 9d
	// TEXTURE
	//deviceContext->PSSetShaderResources(0, 1, &CubesTexture);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);
	deviceContext->PSSetShaderResources(0, 1, &CubesTexture);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Culling mode
	//Counter clockwise, backside first then front.... culling mode
	//**************************************************************************
	// DRAW Cube clockwise 
	deviceContext->RSSetState(CCWcullMode); // counter clockwise 
	deviceContext->DrawIndexed(ARRAYSIZE(Cube_indicies), 0, 0);

	deviceContext->RSSetState(CWcullMode);  // clockwise 
	deviceContext->DrawIndexed(ARRAYSIZE(Cube_indicies), 0, 0);
	

	// TODO: PART 2 STEP 10

	// END PART 2

	// TODO: PART 1 STEP 8

	// END OF PART 1
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6

	deviceContext->ClearState();
	
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(chain);
	SAFE_RELEASE(deviceContext);
	SAFE_RELEASE(pInputLayout);
	SAFE_RELEASE(PS);
	SAFE_RELEASE(VS);
	SAFE_RELEASE(device);
	SAFE_RELEASE(BackBuffer);
	SAFE_RELEASE(constantBuffer);
	SAFE_RELEASE(VSStar);
	SAFE_RELEASE(VertBufferCube); // << unload this
	SAFE_RELEASE(IndexBufferCube);  //<<<<  make this safe
	SAFE_RELEASE(CubesTexture);
	SAFE_RELEASE(CubesTexSamplerState);
	SAFE_RELEASE(constantBuffer_Camera);
	SAFE_RELEASE(pDSV);
	SAFE_RELEASE(pDepthStencil);
	SAFE_RELEASE(VertBufferGrid);
	SAFE_RELEASE(VSGrid_p);
	SAFE_RELEASE(rasterizerState);
	SAFE_RELEASE(PS_Grid_pointer);
	SAFE_RELEASE(Transparency);
	SAFE_RELEASE(CCWcullMode);
	SAFE_RELEASE(CWcullMode);
	SAFE_RELEASE(constantBuffer_offset); 
	SAFE_RELEASE(constantBuffer_Camera);
	

	SAFE_RELEASE(IndexBufferGrid);
	SAFE_RELEASE(VS_Buffer);
	SAFE_RELEASE(PS_Buffer);
	SAFE_RELEASE(chainBuffer);
	SAFE_RELEASE(CubesTexSamplerState);
	SAFE_RELEASE(PS_Grid_pointer);
	SAFE_RELEASE(Transparency);
	SAFE_RELEASE(CWcullMode);


	
	UnregisterClass(L"DirectXApplication", application);

	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

DEMO_APP * theApp = NULL;
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	theApp = &myApp;
	// initialize the input stuff
//	if (!InitDirectInput(hInstance))
//	{
//		MessageBox(0, L"Direct Input Initialization - Failed",
//			L"Error", MB_OK);
//		return 0;
//	}


	while (msg.message != WM_QUIT && myApp.Run())
	{
		// this is where the messages being handled.
		// input handling
		// move things
		// update stuff here
		// clear backbuffer 
		// draw
		// press buttons
		chain->Present(0, 0);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); // process the message, is the message needs to be dispatched. 
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
						break;
	case (WM_SIZE) :	 {
							if (theApp)
							{
								theApp->ResizeWindow();
								
							}
						 } 
						break;

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//


void DEMO_APP::ResizeWindow()
{
	if (chain)
	{
	

		SAFE_RELEASE(pDepthStencil);
		SAFE_RELEASE(pDSV);
		SAFE_RELEASE(chainBuffer);
		SAFE_RELEASE(renderTargetView);
		
		deviceContext->OMSetRenderTargets(0, 0, 0);
		deviceContext->ClearState();
		
		HRESULT hr;
		hr = chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D * pBuffer;
		chain->GetBuffer(0, __uuidof (ID3D11Texture2D), (void**)&pBuffer);

		device->CreateRenderTargetView(pBuffer, NULL, &renderTargetView);
	
		
		//	ID3D11RenderTargetView * tempView = nullptr;
		//deviceContext->OMSetRenderTargets(1, &tempView, nullptr);
		//	deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);




		D3D11_TEXTURE2D_DESC width_height;
		D3D11_VIEWPORT vp;
		pBuffer->GetDesc(&width_height);
		SAFE_RELEASE(pBuffer);

		
		// rsize depth stencil 
		D3D11_TEXTURE2D_DESC descDepth = {0};
		descDepth.Width = width_height.Width;
		descDepth.Height = width_height.Height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		hr = device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);

		hr = device->CreateDepthStencilView(pDepthStencil, // Depth stencil texture
			0,									           // Depth stencil desc
			&pDSV);									       // [out] Depth stencil view

		deviceContext->OMSetRenderTargets(1, &renderTargetView, pDSV);

		vp.Height = width_height.Height;
		vp.Width = width_height.Width;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		
		deviceContext->RSSetViewports(1, &vp);

		Aspect = vp.Height / vp.Width;
	}

}