// D3D Graphics 2 project
// Author: Dmitrii Zyrianov


//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d11.lib")


#include <d3d11.h>
#include "mathlib.h"
#include <iostream>
#include <ctime>
#include "XTime.h"
//#include "Trivial_VS.csh"
//#include "Trivial_PS.csh"
#include "Trivial_PS_PointL.csh"
//#include "PS_Diffuse.csh"
//#include "VS_Grid.csh"
//#include "PS_Grid.csh"
#include "VS_Star.csh"
#include "SkyBox_PS.csh"
#include "SkyBox_VS.csh"
#include "PS_Point.csh"
#include "VS_Point.csh"
#include "windows.h"
#include "..\Assets\Assets\Models\Cube.h"
#include "..\Assets\Textures\Cowboy.h"
#include "Grid.h"
#include "..\Assets\Textures\numbers_test.h"
#include <DirectXMath.h>
#include "DDSTextureLoader.h"
#include <dinput.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using namespace DirectX;

#define BACKBUFFER_WIDTH	1280
#define BACKBUFFER_HEIGHT	768
#define MOSE_SPEED			0.007f

//************************************************************
//************ GLOBAL DEFINES     ****************************
//************************************************************

POINT cursor_previous;
POINT cursor_current;
IDXGISwapChain *chain = nullptr;

XTime xTime;
double deltatime = 0.0f;

/////////// DIRECT INPUT
IDirectInputDevice8 * DIKeyboard;
IDirectInputDevice8 * DIMouse;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rot = 0.0f;
float riseY = 0.0f;
float rotx = 0.0f;
float rotz = 0.0f;
float scaleX = 1.0f;
float scaleY = 1.0f;

XMMATRIX Rotationx;
XMMATRIX Rotationz;
XMMATRIX Rotationy;
/////////// DERECT INPUT END

////////////// CAMERA //////////////////////
XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
// new defines
XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
XMVECTOR camPosition = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);;

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
float cam_View_Angle = 90.0f;
///////////// CAMERA END //////////////////

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************



class DEMO_APP
{

	// personal defines

	float moveZ = -1.0f;
	float moveX = 0.0f;
	float moveY = 0.0f;
	float Aspect = 1.0f;
	bool antializing = true;
	float offset;
	float movemet_speed = 0.001f;
	bool first_person = false;
	bool blend = false;
	bool reverse_light = false;
	UINT model_indicies_Size = 0;

	float shiftLight = 0.0f;
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2

//	DXGI_SWAP_CHAIN_DESC  * desk = nullptr;
	ID3D11Texture2D       * BackBuffer = nullptr;
	ID3D11Device		  * device = nullptr;
	ID3D11DeviceContext   * deviceContext = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11InputLayout	  * pInputLayout = nullptr;
	D3D11_VIEWPORT			viewPort;
	DXGI_MODE_DESC			bufferDesctoFill;

	ID3D11Buffer * VertBufferCube = nullptr; // << unload this
	ID3D11Buffer * VertBufferGround = 0;
	ID3D11Buffer * VertBufferModel = 0;
	ID3D11Buffer * VertBufferSky = 0;
	ID3D11Buffer * IndexBufferCube = nullptr;
	ID3D11Buffer * IndexBufferGround = 0;
	ID3D11Buffer * IndexBufferSky = 0;
	ID3D11Buffer * IndexBufferModel = 0;


	ID3D11Buffer * VS_Buffer = nullptr;
	ID3D11Buffer * PS_Buffer = nullptr;
	ID3D11Buffer * constantBuffer = nullptr;
	ID3D11Buffer * constantBuffer_lightPosition = nullptr;
	ID3D11Buffer * constantBuffer_offset = nullptr; //<< clear later
	ID3D11Buffer * constantBuffer_Camera = nullptr;

	ID3D11Resource				* chainBuffer = nullptr;
	ID3D11ShaderResourceView	* CubesTexture = nullptr;
	ID3D11ShaderResourceView	* GroundTexture = nullptr;
	ID3D11ShaderResourceView	* SkyTexture = nullptr;
	ID3D11SamplerState			* CubesTexSamplerState = nullptr;

	ID3D11PixelShader* PS = nullptr;
	ID3D11VertexShader* VS = nullptr;
	ID3D11VertexShader* VSStar = nullptr;

	ID3D11DepthStencilView* pDSV = nullptr;
	ID3D11Buffer* cbPerFrameBuffer = nullptr;

	ID3D11Texture2D* pDepthStencil = NULL;

	// All Shader Variables (Always Pre-fixed by �SV_�)
	XMMATRIX SV_WorldMatrix = XMMatrixIdentity();
	XMMATRIX SV_CubeMatrix = XMMatrixIdentity();
	XMMATRIX SV_ViewMatrix = XMMatrixIdentity();
	
	XMMATRIX SV_Perspective = XMMatrixIdentity();  
	XMMATRIX RotationY = XMMatrixIdentity();
	XMMATRIX RotationX = XMMatrixIdentity();
	XMMATRIX RotationZ = XMMatrixIdentity();
	XMMATRIX Translation = XMMatrixIdentity();
	
	ID3D11BlendState * blendState;
	ID3D11BlendState* Transparency;

	ID3D11RasterizerState* rasterizerState;
	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

	// skybox 
	ID3D11Buffer* sphereIndexBuffer;
	ID3D11Buffer* sphereVertBuffer;
	ID3D11Buffer * constantBufferSphere = nullptr;
	ID3D11Buffer * constantBufferSky = nullptr;
	ID3D11Buffer * constantBufferPointLight = nullptr;

	ID3D11VertexShader* SKYMAP_VS;
	ID3D11PixelShader* SKYMAP_PS;
	ID3D11VertexShader* POINT_VS;
	ID3D11PixelShader* POINT_PS;

	ID3D10Blob* SKYMAP_VS_Buffer;
	ID3D10Blob* SKYMAP_PS_Buffer;

	ID3D11ShaderResourceView* smrv;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	int NumSphereVertices;
	int NumSphereFaces;

	XMMATRIX SV_Skybox_World;
	// end skybox


	/// with DAN !

	struct OBJECT
	{
		XMMATRIX world;
	};
	struct SCENE
	{
		XMMATRIX view;
		XMMATRIX projection;
	};



	struct Light
	{
		Light()
		{
			ZeroMemory(this, sizeof(Light));
		}
		XMFLOAT3 dir;
		float pad1;
		
		XMFLOAT3 pos;
		float range;
		XMFLOAT3 att;
		float pad2;
	
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
	};

	// Setting Up the light 
	Light light;


	struct cbPerFrame
	{
		Light light;
	};

	cbPerFrame constbuffPerFrame;

	struct cbPerObject
	{
		XMMATRIX  WVP;
		XMMATRIX  World;
	};



	struct SEND_TO_VRAM
	{
		float constantColor[4];
		float constantOffset[2];
		float padding[2];
	};

	struct WorldBuffer 
	{
		XMFLOAT4X4 worldMatrix;
	};

	struct Scene {
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projMatrix;

	};
	
	SEND_TO_VRAM toShader;

public:


	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void ResizeWindow(); // prototype the resize 
	void UpdateCamera(double deltaTime); // prototype the camera function
	bool InitializeDirectInput(HINSTANCE hInstance); // initialize the direct input
	void DetectInput(double deltaTime);
	void MakeCube();
	void MakeSky(int LatLines, int LongLines);
	void MakeGround(float scale = 2.0f);
	void MakeGrid(float depth, float width);
	XMMATRIX PerspectiveProjectionMatrix(float FOV, float zFar, float zNear, float aspect);
	void PointLight1(Light * _light);
	bool LoadObj(char * filename,
		std::vector < XMVECTOR > & out_vertices,
		std::vector < XMVECTOR > & out_uvs,
		std::vector < XMVECTOR > & out_normals
		);
	//void Update();
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

	window = CreateWindow(L"DirectXApplication", L"Graphics2 Demo", WS_OVERLAPPEDWINDOW /*& ~(WS_THICKFRAME | WS_MAXIMIZEBOX)*/,
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//

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
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // set these flags and add the others as output
	swap_chain_desc.OutputWindow = window;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
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
		NULL,							// you make this 0, 
		D3D11_CREATE_DEVICE_DEBUG,	    // you put in interesting flags. can also use the// set flag in release 
		pFeatures,						// this will work in the enum for backwards compatibility, pass null in here.
		6,								// the amount levels above you uses
		D3D11_SDK_VERSION,				// gives you the version
		&swap_chain_desc,				// 
		&chain,
		&device,
		&pFeatureLevel,					
		&deviceContext);
#else
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, GetModuleHandle(L"d3d11.dll"), 0, nullptr, 6, D3D11_SDK_VERSION, &swap_chain_desc, &chain, &device, nullptr, nullptr);
	// deviceContext issues commands directly to the hardware
#endif

	
	// fill the back buffer
	hr = chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&chainBuffer);

	//Create the Render Target
	hr = device->CreateRenderTargetView(chainBuffer, NULL, &renderTargetView);

	// this a window in the back-buffer 
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = BACKBUFFER_WIDTH;
	viewPort.Height = BACKBUFFER_HEIGHT;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;


D3D11_INPUT_ELEMENT_DESC vLayout[] =
{
	{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UVM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NRM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

	D3D11_BUFFER_DESC constBufferDesc = { 0 };
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.ByteWidth = sizeof(SV_WorldMatrix) * 2;
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA constBuffer_resource = { 0 };
	device->CreateBuffer(&constBufferDesc, nullptr, &constantBuffer);

	D3D11_BUFFER_DESC constBufferDesc_offset = { 0 };
	constBufferDesc_offset.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc_offset.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc_offset.ByteWidth = sizeof(float) * 4;
	constBufferDesc_offset.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA constBuffer_resource_offset = { 0 };
	device->CreateBuffer(&constBufferDesc_offset, nullptr, &constantBuffer_offset);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;



	///////////////////////////////////////////////////////////
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


	hr = device->CreateDepthStencilView(pDepthStencil,  // Depth stencil texture
		&descDSV,										// Depth stencil desc
		&pDSV);											// [out] Depth stencil view


														// TEXTURE DESCRIPTION 

	

// cube texturing the manual way code
/*
/	ID3D11Texture2D * tex;
/	D3D11_TEXTURE2D_DESC tdesc;
/	D3D11_SUBRESOURCE_DATA tbsd[numbers_test_numlevels];
/	ZeroMemory(tbsd, sizeof(tbsd));
/	for (unsigned int i = 0; i < numbers_test_numlevels; i++)
/	{
/		tbsd[i].pSysMem = &numbers_test_pixels[numbers_test_leveloffsets[i]] ;
/		tbsd[i].SysMemPitch = sizeof(UINT) * (numbers_test_width >> i);
/	}
/
/
/	tdesc.Width = numbers_test_width ;
/	tdesc.Height = numbers_test_height;
/	tdesc.MipLevels = numbers_test_numlevels;
/	tdesc.ArraySize = 1;
/
/	tdesc.SampleDesc.Count = 1;
/	tdesc.SampleDesc.Quality = 0;
/	tdesc.Usage = D3D11_USAGE_DEFAULT;
/	tdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
/	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
/
/	tdesc.CPUAccessFlags = 0;
/	tdesc.MiscFlags = 0;
/
/	device->CreateTexture2D(&tdesc, tbsd, &tex);
/	hr = device->CreateSamplerState(&samplerDesc, &CubesTexSamplerState);
/	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceView_desc;
/	shaderResourceView_desc.Format = tdesc.Format;
/	shaderResourceView_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
/	shaderResourceView_desc.Texture2D.MostDetailedMip = 0;
/	shaderResourceView_desc.Texture2D.MipLevels = numbers_test_numlevels;
/	
/	////// get TEXTURE from file 
/	SAFE_RELEASE(tex);// !!!
*/




	
	/////////////////////////////////////////////////////////////////////////////////
	// Textures 

	hr = CreateDDSTextureFromFile(device, L"metallock.dds", NULL, &CubesTexture);
	hr = CreateDDSTextureFromFile(device, L"fieldDDs.dds", NULL, &GroundTexture);
	device->CreateInputLayout(vLayout, 3, VS_Star, sizeof(VS_Star), &pInputLayout);
	
	////////////////////////////////////////////////////////////////////////////////
	// shaders
	device->CreateVertexShader(VS_Star, sizeof(VS_Star), nullptr, &VSStar);
	device->CreatePixelShader(Trivial_PS_PointL, sizeof(Trivial_PS_PointL), nullptr, &PS); // first create the shaders
	
	// Old shader for diffuse light:     //	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &PS); // first create the shaders

	//////////////////////////////////////////////////////////////////////////////////
	// POINT LIGHT 

	D3D11_BUFFER_DESC constBufferDescLight = { 0 };
	constBufferDescLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDescLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDescLight.ByteWidth = sizeof(SV_WorldMatrix) * 2;
	constBufferDescLight.Usage = D3D11_USAGE_DYNAMIC;

	constBuffer_resource = { 0 };
	device->CreateBuffer(&constBufferDesc, nullptr, &constantBufferPointLight);

		
	///////////////////////////////////////////////////////////////////////////////
	// skybox
	
	
	
	
	device->CreateVertexShader(SkyBox_VS, sizeof(SkyBox_VS), NULL, &SKYMAP_VS);
	device->CreatePixelShader(SkyBox_PS, sizeof(SkyBox_PS), NULL, &SKYMAP_PS);

	//ID3D11Texture2D* SMTexture = 0;
	
	hr = CreateDDSTextureFromFile(device, L"SunsetSkybox.dds", NULL, &SkyTexture );

	//(ID3D11Resource**)&SMTexture

	//deviceContext->PSSetShaderResources(0, 1, &GroundTexture);
	//D3D11_TEXTURE2D_DESC SMTextureDesc;
	//SMTexture->GetDesc(&SMTextureDesc);
	//
	//D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	//SMViewDesc.Format = SMTextureDesc.Format;
	//SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; 
	//SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	//SMViewDesc.TextureCube.MostDetailedMip = 0;
	//SMViewDesc.Texture3D.MipLevels = SMTextureDesc.MipLevels;
	//SMViewDesc.Texture3D.MostDetailedMip = 0;
	//hr = device->CreateShaderResourceView(SMTexture, &SMViewDesc, &smrv);
	
	//SAFE_RELEASE(SMTexture);



	// end skybox 


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// rasterizer state // put this into a serparate function
	D3D11_RASTERIZER_DESC rasterizer_Description;
	ZeroMemory(&rasterizer_Description, sizeof(rasterizer_Description));
	rasterizer_Description.AntialiasedLineEnable = true;
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

	rtbd.BlendEnable			= blend;
	rtbd.SrcBlend				= D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend				= D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp				= D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			= D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			= D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			= D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	= D3D10_COLOR_WRITE_ENABLE_ALL;

	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.RenderTarget[0] = rtbd;

	device->CreateBlendState(&blendStateDesc, &Transparency);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// rasterizer description 

	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.DepthBias = 0.0f;
	cmdesc.DepthClipEnable = true;
	cmdesc.FrontCounterClockwise = true;
	hr = device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = device->CreateRasterizerState(&cmdesc, &CWcullMode);



	//*******************************************************
	// Member function for window resize 
	ResizeWindow();

	////////////////////////////////////////////////////////
	// LIGHT  setup 
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	light.pos =		XMFLOAT3(0.0f, 0.0f, 0.0f);
	light.range =	100.0f;
	light.att =		XMFLOAT3(0.0f, 0.2f, 0.0f);
	light.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


	hr = device->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);
	
	// assigning vertex shaders
	device->CreateVertexShader(VS_Point, sizeof(VS_Point), NULL, &POINT_VS);
	device->CreatePixelShader(PS_Point, sizeof(PS_Point), NULL, &POINT_PS);
	


	///////////MAKE THE CUBE /////////////////////////
	MakeCube();///////////////////////////////////////
	//////////////////////////////////////////////////

	///////// MAKE THE GROUND ////////////////////////
	MakeGround(4.0f);/////////////////////////////////
	//////////////////////////////////////////////////

	///////// MAKE THE SKYBOX ////////////////////////
	MakeSky(10, 10);  ////////////////////////////////
	//////////////////////////////////////////////////
	
	std::vector< XMVECTOR > vertices;
	std::vector< XMVECTOR > uvs;
	std::vector< XMVECTOR > normals;

	///////// LOAD AN OBJECT  ////////////////////////
	LoadObj("teapot.obj", vertices, uvs, normals );
	//////////////////////////////////////////////////

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = device->CreateSamplerState(&samplerDesc, &CubesTexSamplerState);

	// skybox buffers
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = device->CreateRasterizerState(&cmdesc, &RSCullNone);
	
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	D3D11_BUFFER_DESC constBufferDescSphere = { 0 };
	constBufferDescSphere.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDescSphere.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDescSphere.ByteWidth = sizeof(SV_WorldMatrix)*2;
	constBufferDescSphere.Usage = D3D11_USAGE_DYNAMIC;


	device->CreateBuffer(&constBufferDescSphere, nullptr, &constantBufferSky);


	// light constant buffer 
	D3D11_BUFFER_DESC constBufferDesc_LightPositiong = { 0 };
	constBufferDesc_LightPositiong.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc_LightPositiong.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc_LightPositiong.ByteWidth = sizeof(XMVECTOR);
	constBufferDesc_LightPositiong.Usage = D3D11_USAGE_DYNAMIC;


	device->CreateBuffer(&constBufferDesc_LightPositiong, nullptr, &constantBuffer_lightPosition);

	XMVECTOR eyePos = XMVectorSet();
	XMVECTOR focusPos;
	XMVECTOR worl

	m_skyboxData.world = XMMatrixIdentity();
	m_sceneData.projection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians()
		);
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{



	// center 

	XMmatic ciew matric_copy = MMatrixInvers(null, m_scenedata.view);
	m_Skybox.word = XMmatri

		viewmatrix_copy.r[3].m128_f32[0];
	viewmatrix_copy.r[3].m128_f32[1];
	viewmatrix_copy.r[3].m128_f32[2];



	D3D11_BUFFER_DESC dext
		indexBuffer->GetBufferDescription(dext);
	amount to drwa = dext.bytewidth / sizeof(Onject-Vert)

		// create sampler state



	deviceContext->IASetInputLayout(pInputLayout);

	deviceContext->OMSetRenderTargets(1, &renderTargetView, pDSV); // this is there the Z bugger will go 






	////////////////// BACKGROUND COLOR reset
	float color_array[4] = { 0, 0, 0.1f, 0.1f };

	deviceContext->ClearRenderTargetView(renderTargetView, color_array);
	// Create the depth stencil view
	deviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);

	//********************************************************************************************//
	// setting up blending

	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };

	// sets up the default blend state for opaque objects 
	deviceContext->OMSetBlendState(0, 0, 0xffffffff);
	//Set the blend state for transparent objects
	deviceContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

	
	/// with DAN
	// set index buffer 
	deviceContext->IASetVertexBuffers(0,1, 
		// set vertex puffet
	//  
//		set input layont input layout 

// IASetVerte

// set the vertex shaders
// set pixel shader
// set constsnt buffers
// 


	const UINT stride = sizeof(OBJ_VERT);
	UINT offest = 0;
	UINT offset;









	// LIGHT ////////////////////////////////////////////////////////////////////////////

	deviceContext->VSSetShader(POINT_VS, nullptr, NULL);
	deviceContext->PSSetShader(POINT_PS, nullptr, NULL);

	cbPerObject cbPerObj2;
	//sky_Matrix = XMMatrixTranspose(sky_Matrix);
	//sphereWorld = XMMatrixTranspose(sphereWorld);
	XMMATRIX WVP2 = SV_ViewMatrix * SV_Perspective;
	cbPerObj2.World = XMMatrixTranspose(SV_WorldMatrix);
	cbPerObj2.WVP = XMMatrixTranspose(WVP2);

	D3D11_MAPPED_SUBRESOURCE mapped_resource = { 0 };
	deviceContext->Map(constantBufferSky, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &cbPerObj2, sizeof(cbPerObj2));
	deviceContext->Unmap(constantBufferSky, 0);
	////////////////////////////////////////////////////////////////////////////////////



	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// some constants the rest 
	const UINT stride = sizeof(OBJ_VERT);
	UINT offest = 0;
	UINT offset;
	offset = 0;





	///////////////////////////////////////////////////////////////////////////////////
	// SKYBOX
	SV_Skybox_World = XMMatrixIdentity();

	XMMATRIX Scale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	Translation = XMMatrixIdentity();
	Translation = XMMatrixTranslation(XMVectorGetX(camPosition), XMVectorGetY(camPosition), XMVectorGetZ(camPosition));
	SV_Skybox_World = Scale * Translation;
	//SV_Skybox_World =  camRotationMatrix * SV_Skybox_World ;
	//XMMATRIX sky_Matrix  = sphereWorld * SV_ViewMatrix * SV_Perspective;

	XMMATRIX WVP = SV_Skybox_World * XMMatrixInverse(nullptr, SV_ViewMatrix) * SV_Perspective;
//  XMMATRIX WVP = SV_Perspective * XMMatrixInverse(nullptr, SV_ViewMatrix) * SV_Skybox_World;
//	XMMATRIX WVP = XMMatrixIdentity();//SV_Perspective; //SV_ViewMatrix;// SV_ViewMatrix * SV_Perspective * SV_Skybox_World;

	//sky_Matrix = XMMatrixTranspose(sky_Matrix);
	//sphereWorld = XMMatrixTranspose(sphereWorld);

	cbPerObject cbPerObj;
	cbPerObj.World = XMMatrixTranspose(SV_Skybox_World);
	cbPerObj.WVP = XMMatrixTranspose(WVP);

	mapped_resource = { 0 };
	deviceContext->Map(constantBufferSky, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &cbPerObj, sizeof(cbPerObj));
	deviceContext->Unmap(constantBufferSky, 0);


	//deviceContext->UpdateSubresource(constantBufferShere, 0, NULL, &cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &constantBufferSky);
	deviceContext->PSSetShaderResources(0, 1, &SkyTexture);


	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);
	deviceContext->RSSetState(RSCullNone);

	deviceContext->IASetVertexBuffers(0, 1, &VertBufferSky, &stride, &offset);
	deviceContext->IASetIndexBuffer(IndexBufferSky, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->VSSetShader(SKYMAP_VS, nullptr, NULL);
	deviceContext->PSSetShader(SKYMAP_PS, nullptr, NULL);
	deviceContext->OMSetDepthStencilState(DSLessEqual, 0);

	deviceContext->DrawIndexed(72, 0, 0);													// DRAW CALL sky 

	deviceContext->OMSetDepthStencilState(NULL, 0);
	deviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);







	///////////  GROUND  /////////////////////////////////////////////////////////////////////////////////////////////
	mapped_resource = { 0 };
	
	XMMATRIX scene[2] = { SV_WorldMatrix, SV_Perspective };
	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, scene, sizeof(scene));
	deviceContext->Unmap(constantBuffer, 0);

	if (reverse_light)
	{
		shiftLight -= deltatime / 10000;
	}
	else
		shiftLight += deltatime / 10000;


	if (shiftLight > 10 || shiftLight < -10)
	{
		reverse_light = !reverse_light;

	}
	XMVECTOR lightVector_Ground = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); // light starting position
	XMVECTOR point_light_Ground = XMVectorSet(XMVectorGetX(lightVector_Ground) + shiftLight, XMVectorGetY(lightVector_Ground), XMVectorGetZ(lightVector_Ground), 0.0f);
	//point_light = XMVectorRotateRight(point_light, shiftLight);
	//point_light = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	mapped_resource = { 0 };
	deviceContext->Map(constantBuffer_lightPosition, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &point_light_Ground, sizeof(XMVECTOR));
	deviceContext->Unmap(constantBuffer_lightPosition, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &constantBuffer_lightPosition);


	deviceContext->VSSetShader(POINT_VS, nullptr, NULL);
	deviceContext->PSSetShader(POINT_PS, nullptr, NULL);

	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // the 0 is slot index 0, 1 is the num buffers 
	deviceContext->PSSetShaderResources(0, 1, &GroundTexture);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);
	deviceContext->RSSetState(rasterizerState);
	
	deviceContext->IASetInputLayout(pInputLayout);
	deviceContext->IASetIndexBuffer(IndexBufferGround, DXGI_FORMAT_R32_UINT, offset);
	deviceContext->IASetVertexBuffers(0, 1, &VertBufferGround, &stride, &offest);

	
	/// problem here, when enabled, disables the ground
	

	deviceContext->VSSetShader(VSStar, nullptr, NULL);
	deviceContext->PSSetShader(PS, nullptr, NULL);

	deviceContext->RSSetState(CCWcullMode); // counter clockwise 
	deviceContext->DrawIndexed(6, 0, 0);																	// DRAW CALL

	deviceContext->RSSetState(CWcullMode);  // clockwise 
	deviceContext->DrawIndexed(6, 0, 0);																	// DRAW CALL

	
	

	/////////// DRAW CUBE////////////////////////////////////////////////////////////////////////////////////////////////
	// setting the texture for the ground


	XMMATRIX scene2[2] = { SV_WorldMatrix, SV_Perspective };
	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, scene2, sizeof(scene2));
	deviceContext->Unmap(constantBuffer, 0);

	// rotate the cube
//	mapped_resource = { 0 };
//	SV_CubeMatrix = XMMatrixRotationY(deltatime);
//	SV_CubeMatrix =  SV_CubeMatrix * XMMatrixTranslation(0.0f, 0.1f, 5.0f);
	
	
//	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
//	memcpy(mapped_resource.pData, &SV_CubeMatrix, sizeof(SV_CubeMatrix) * 2);
//	deviceContext->Unmap(constantBuffer, 0);

	// the 0 is slot index 0, 1 is the num buffers 

	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // the 0 is slot index 0, 1 is the num buffers 
	deviceContext->PSSetShaderResources(0, 1, &CubesTexture);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);
	deviceContext->RSSetState(rasterizerState);

	// loading passing the light position

	XMVECTOR lightVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//lightVector = XMVector3TransformCoord(lightVector, SV_WorldMatrix);


	if (reverse_light)
	{
		shiftLight -= deltatime / 10000;
	} else 
    	shiftLight += deltatime / 10000;


	if ( shiftLight > 10 || shiftLight < -10)
	{
	reverse_light = !reverse_light;
		
	} 
	

	deviceContext->IASetInputLayout(pInputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &VertBufferCube, &stride, &offest);
	deviceContext->IASetIndexBuffer(IndexBufferCube, DXGI_FORMAT_R32_UINT, offest);
//	deviceContext->VSSetShader(VSStar, nullptr, NULL);
//  deviceContext->PSSetShader(PS, nullptr, NULL);

	deviceContext->VSSetShader(POINT_VS, nullptr, NULL);
	deviceContext->PSSetShader(POINT_PS, nullptr, NULL);
	//culling 
	deviceContext->RSSetState(CCWcullMode); // counter clockwise 
	deviceContext->DrawIndexed(ARRAYSIZE(Cube_indicies), 0, 0);													// DRAW CALL

	deviceContext->RSSetState(CWcullMode);  // clockwise 
	deviceContext->DrawIndexed(ARRAYSIZE(Cube_indicies), 0, 0);													// DRAW CALL
	
	
	//////////////////////////////////////////////////////////////////////////////
	// MODEL 


	mapped_resource = { 0 };
	SV_CubeMatrix = XMMatrixIdentity();
	SV_CubeMatrix = SV_CubeMatrix * XMMatrixTranslation(0.0f, 0.1f, 5.0f);


	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &SV_CubeMatrix, sizeof(SV_CubeMatrix) * 2);
	deviceContext->Unmap(constantBuffer, 0);


	// the 0 is slot index 0, 1 is the num buffers 

	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // the 0 is slot index 0, 1 is the num buffers 
	deviceContext->PSSetShaderResources(0, 1, &CubesTexture);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);
	deviceContext->RSSetState(rasterizerState);


	deviceContext->IASetInputLayout(pInputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &VertBufferModel, &stride, &offest);
	deviceContext->IASetIndexBuffer(IndexBufferModel, DXGI_FORMAT_R32_UINT, offest);
	deviceContext->VSSetShader(VSStar, nullptr, NULL);
	deviceContext->PSSetShader(PS, nullptr, NULL);
	
	/// problem here, when enabled, disables the ground
//	deviceContext->VSSetShader(POINT_VS, nullptr, NULL);
//	deviceContext->PSSetShader(POINT_PS, nullptr, NULL);

	
	//culling 
	deviceContext->RSSetState(CCWcullMode); // counter clockwise 
	deviceContext->DrawIndexed(model_indicies_Size, 0, 0);													// DRAW CALL

	deviceContext->RSSetState(CWcullMode); // counter clockwise 
	deviceContext->DrawIndexed(model_indicies_Size, 0, 0);
	
	
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
	if (!theApp->InitializeDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}


	while (msg.message != WM_QUIT && myApp.Run())
	{
		/////////////////////////////////////////////////////
		/// MAIN GAME LOOP
     
		// this is where the messages being handled.
		// input handling
		// move things
		// update stuff here
		// clear backbuffer 
		// draw
		// press buttons
		
		xTime.Signal();
		deltatime += xTime.Delta();


		//theApp->Update(deltatime);
		theApp->DetectInput(deltatime);
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


// changes the size of the window
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

bool DEMO_APP::InitializeDirectInput(HINSTANCE hInstance)
{

	HRESULT hr = DirectInput8Create(hInstance,
									DIRECTINPUT_VERSION,
									IID_IDirectInput8,
									(void**)&DirectInput,
									NULL);

	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
									&DIKeyboard,
									NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse,
									&DIMouse,
									NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	
	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	// controls and sets the mouse pointer to locked 
	// ---------------------------------------------
	//hr = DIMouse->SetCooperativeLevel(window, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	
	// controls and sets the mouse pointer to lose 
	// ---------------------------------------------
	hr = DIMouse->SetCooperativeLevel(window, DISCL_EXCLUSIVE | DISCL_NOWINKEY );
	//
	ShowCursor(true);
	return true;

}

void DEMO_APP::DetectInput(double time)
{
	// holds mouse info 
	DIMOUSESTATE mouseCurrState = {0};

	// breakdown 
	//LONG lX; 
	//LONG lY;
	//LONG lZ;
	//BYTE rgbButtons[4];

	BYTE keyboardState[256] = {0};

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(window, WM_DESTROY, 0, 0);

//	float speed = 15.0f * time;
	
	if ( keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= movemet_speed;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += movemet_speed;
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += movemet_speed;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= movemet_speed;
	}
	if (keyboardState[DIK_1] & 0x80)
	{
		first_person = !first_person;
	}
	
	if (keyboardState[DIK_SPACE] & 0x80)
	{
		riseY -= movemet_speed;
	}
	if (mouseCurrState.lZ > 0)
	{
		
		cam_View_Angle += 2.0f;
	}
	else if (mouseCurrState.lZ < 0)
	{
		cam_View_Angle -= 2.0f;
	}
// switching between first and freelook camera
	if (first_person)
	{

		if ( ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY)))
		{
			camYaw += mouseLastState.lX * MOSE_SPEED;
			camPitch += mouseCurrState.lY * MOSE_SPEED;
			mouseLastState = mouseCurrState;
		}
	}
	else
	{
		if (((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY)) && GetAsyncKeyState(VK_RBUTTON))
		{
			camYaw += mouseLastState.lX * MOSE_SPEED;
			camPitch += mouseCurrState.lY * MOSE_SPEED;
			mouseLastState = mouseCurrState;
		}

	}

	UpdateCamera(deltatime);

	

	return;
}

void DEMO_APP::UpdateCamera(double deltaTime)
{
	
	//SV_Perspective = XMMatrixPerspectiveLH(1.0f, 1.0f,1.0f,10.0f);

	
	SV_Perspective  = theApp->PerspectiveProjectionMatrix(cam_View_Angle, 100.0f, 0.1f, Aspect);

	//SV_Perspective = Perspective_Projection_Matrix(90.0f, 100.0f, 0.1f, Aspect); // <<<<<< Projection 
	// not using the roll paramenter, so setting to 0
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0.0f);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	// finding the new direction of the camera 
	XMMATRIX temp_RotateOnY;
	temp_RotateOnY = XMMatrixRotationY(camPitch);
	XMMATRIX temp_RotateOnX;
	temp_RotateOnX = XMMatrixRotationY(camYaw);

	camRight = XMVector3TransformCoord(DefaultRight, temp_RotateOnX);
	camForward = XMVector3TransformCoord(camTarget, temp_RotateOnY);
	camUp = XMVector3TransformCoord(camUp, temp_RotateOnY);

	camPosition += moveLeftRight* camRight;
	camPosition += moveBackForward *camForward ;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;

	SV_ViewMatrix = XMMatrixLookAtLH(camPosition, camTarget, camUp);
	
	XMVECTOR rotXaxis = XMVectorSet( movemet_speed, 0.0f,			0.0f,			0.0f);
	XMVECTOR rotYaxis = XMVectorSet( 0.0f,			movemet_speed,	0.0f,			0.0f);
	XMVECTOR rotZaxis = XMVectorSet( 0.0f,			0.0f,			movemet_speed,	0.0f);

	RotationY = XMMatrixRotationAxis(rotYaxis, rot);
	RotationX = XMMatrixRotationAxis(rotXaxis, rotx);
	RotationZ = XMMatrixRotationAxis(rotZaxis, rotz);
	Translation = XMMatrixTranslation(0.0f, riseY, 4.0f);

	//Set world space using the transformations
	SV_WorldMatrix = Translation * RotationX * RotationY * RotationZ;

	// Loading the buffers
	D3D11_BUFFER_DESC constBufferDesc_camera = { 0 };
	constBufferDesc_camera.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc_camera.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc_camera.ByteWidth = sizeof(XMMATRIX) * 2;
	constBufferDesc_camera.Usage = D3D11_USAGE_DYNAMIC;
	SAFE_RELEASE(constantBuffer_Camera);                                           // lesson learned here, don't overwrite the camera buffer! (// leak fixed !)

	device->CreateBuffer(&constBufferDesc_camera, nullptr, &constantBuffer_Camera);

	//// MAP ////////////////////////////////////////////////
	/// constantBuffer_Camera

	D3D11_MAPPED_SUBRESOURCE mapped_resource = { 0 };
	XMMATRIX scene[2] = { SV_ViewMatrix, SV_Perspective };
	deviceContext->Map(constantBuffer_Camera, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, scene, sizeof(scene));
	deviceContext->Unmap(constantBuffer_Camera, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &constantBuffer_Camera);  // into slot 1
	
	////////////////////////////////////////////////////////
	/// SV_WorldMatrix
	float offset_for0;
	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &SV_WorldMatrix, sizeof(SV_WorldMatrix));
	deviceContext->Unmap(constantBuffer, 0);
	
	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); 
	// the 0 is slot index 0, 1 is the num buffers 


	// Bind the depth stencil view
	deviceContext->OMSetRenderTargets(	1,                 // One rendertarget view
										&renderTargetView, // Render target view, created earlier
										pDSV);			   // Depth stencil view for the render target

}

void DEMO_APP::MakeCube()
{
	D3D11_BUFFER_DESC indexBufferData_cube		= { 0 };
	indexBufferData_cube.Usage					= D3D11_USAGE_IMMUTABLE;
	indexBufferData_cube.BindFlags				= D3D11_BIND_INDEX_BUFFER;
	indexBufferData_cube.ByteWidth				= sizeof(Cube_indicies);
	indexBufferData_cube.MiscFlags				= 0;
	indexBufferData_cube.CPUAccessFlags			= 0;
	indexBufferData_cube.StructureByteStride	= 0;

	D3D11_SUBRESOURCE_DATA indexBufferDataSR_cube = { 0 };
	indexBufferDataSR_cube.pSysMem = Cube_indicies;
	indexBufferDataSR_cube.SysMemPitch = 0;
	indexBufferDataSR_cube.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&indexBufferData_cube, &indexBufferDataSR_cube, &IndexBufferCube);


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

}

void DEMO_APP::MakeSky(int LatLines, int LongLines)
{

	// new code for skybox
	OBJ_VERT sky_data[8] = { 0 };

	// Generating grid 
	//					X     Y      Z        U     V     M           N          R         M
	sky_data[0] = { { -1.0f, 1.0f , 1.0f },{ 0.0f, 0.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };
	sky_data[1] = { {  1.0f, 1.0f , 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };
	sky_data[2] = { { -1.0f, 1.0f ,-1.0f },{ 0.0f, 1.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };
	sky_data[3] = { {  1.0f, 1.0f ,-1.0f },{ 1.0f, 1.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };

	sky_data[4] = { { -1.0f,-1.0f , 1.0f },{ 0.0f, 0.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };
	sky_data[5] = { { 1.0f, -1.0f , 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };
	sky_data[6] = { { -1.0f,-1.0f ,-1.0f },{ 0.0f, 1.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };
	sky_data[7] = { { 1.0f, -1.0f ,-1.0f },{ 1.0f, 1.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };

	unsigned int sky_indicies[72] =
	{
		0, 1, 2,
		1, 3, 2,

		2, 3, 6,
		3, 7, 6,

		0, 2, 4,
		2, 6, 4,
		
		1, 0, 5,
		0, 4, 5,
		
		3, 1, 7, 
		1, 5, 7,
		
		7, 6, 5, 
		6, 4, 5
	};

	// verts 
	D3D11_BUFFER_DESC VertBufferData_Sky = { 0 };
	VertBufferData_Sky.Usage = D3D11_USAGE_IMMUTABLE;
	VertBufferData_Sky.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertBufferData_Sky.ByteWidth = sizeof(OBJ_VERT) * 8;
	VertBufferData_Sky.MiscFlags = 0;
	VertBufferData_Sky.CPUAccessFlags = 0;
	VertBufferData_Sky.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VertBufferDataSR_Sky = { 0 };
	VertBufferDataSR_Sky.pSysMem = sky_data;
	VertBufferDataSR_Sky.SysMemPitch = 0;
	VertBufferDataSR_Sky.SysMemSlicePitch = 0;

	SAFE_RELEASE(VertBufferSky);
	HRESULT hr = device->CreateBuffer(&VertBufferData_Sky, &VertBufferDataSR_Sky, &VertBufferSky);

	// indecies

	D3D11_BUFFER_DESC indexBufferData_sky = { 0 };
	indexBufferData_sky.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferData_sky.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferData_sky.ByteWidth = sizeof(UINT) * 72;
	indexBufferData_sky.MiscFlags = 0;
	indexBufferData_sky.CPUAccessFlags = 0;
	indexBufferData_sky.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferDataSR_sky = { 0 };
	indexBufferDataSR_sky.pSysMem = sky_indicies;
	indexBufferDataSR_sky.SysMemPitch = 0;
	indexBufferDataSR_sky.SysMemSlicePitch = 0;

	SAFE_RELEASE(IndexBufferSky);
	hr = device->CreateBuffer(&indexBufferData_sky, &indexBufferDataSR_sky, &IndexBufferSky);



}

void DEMO_APP::MakeGround(float scale)
{
	OBJ_VERT Ground_data[4] = { 0 };
	// Generating grid 
	float x_pos = -1.0f * scale;
	float z_pos =  1.0f *scale ;

	float u = 0.0f;
	float v = 0.0f;
	for (UINT i = 0; i < 4; i++)
	{
		Ground_data[i] = { { x_pos, 0.0f , z_pos },{ u, v, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };
		x_pos = - x_pos;
		
		if (i % 2)
		{
	    	z_pos = - z_pos;
		}

		u = u + 1;
		if (u >= 2)
		{
			u = 0.0f;
		}
		if (i==2)
		{
		Ground_data[2].uvw[1] = 1.0f;

		} if (i==3)
		{
	    Ground_data[3].uvw[1] = 1.0f;

		}
	}
	//Ground_data
	//Create the vertex buffer
//	Ground_data[4] =
//	{		//X       Y     Z       u      v     w          N            
		

//	Ground_data[0] = { { -1.0f, 0.0f , 1.0f },{ 0.0f, 0.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };//0
//	Ground_data[1] = { {  1.0f, 0.0f,  1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };//1
//	Ground_data[2] = { { -1.0f, 0.0f, -1.0f },{ 0.0f, 1.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } };//2
//	Ground_data[3] = { {  1.0f, 0.0f, -1.0f },{ 1.0f, 1.0f, 0.0f },{ 0.000000f, 1.000000f, 0.000000f } }; //3
//	};

//	for (UINT i = 0; i < 4; i++)
//	{
//		XMVECTOR temp = { Ground_data[i].pos[0], Ground_data[i].pos[1], Ground_data[i].pos[2], 0 };
//		temp = XMVector4Transform(temp, XMMatrixScaling(scale,0, scale));
//		memcpy(&Ground_data[i].pos[0], &temp, sizeof(Ground_data[i]));
//	}


	unsigned int Ground_indicies[6] =
	{
		0, 1, 2,
		1, 3, 2
	};


	// verts 
	D3D11_BUFFER_DESC VertBufferData_ground = { 0 };
	VertBufferData_ground.Usage = D3D11_USAGE_IMMUTABLE;
	VertBufferData_ground.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertBufferData_ground.ByteWidth = sizeof(OBJ_VERT) * 4;
	VertBufferData_ground.MiscFlags = 0;
	VertBufferData_ground.CPUAccessFlags = 0;
	VertBufferData_ground.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VertBufferDataSR_ground = { 0 };
	VertBufferDataSR_ground.pSysMem = Ground_data;
	VertBufferDataSR_ground.SysMemPitch = 0;
	VertBufferDataSR_ground.SysMemSlicePitch = 0;
	
	SAFE_RELEASE(VertBufferGround);
	HRESULT hr = device->CreateBuffer(&VertBufferData_ground, &VertBufferDataSR_ground, &VertBufferGround);
	
	// indecies

	D3D11_BUFFER_DESC indexBufferData_ground	= { 0 };
	indexBufferData_ground.Usage				= D3D11_USAGE_IMMUTABLE;
	indexBufferData_ground.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	indexBufferData_ground.ByteWidth			= sizeof(UINT) * 6;
	indexBufferData_ground.MiscFlags			= 0;
	indexBufferData_ground.CPUAccessFlags		= 0;
	indexBufferData_ground.StructureByteStride	= 0;

	D3D11_SUBRESOURCE_DATA indexBufferDataSR_ground		= { 0 };
	indexBufferDataSR_ground.pSysMem					= Ground_indicies;
	indexBufferDataSR_ground.SysMemPitch				= 0;
	indexBufferDataSR_ground.SysMemSlicePitch			= 0;

	SAFE_RELEASE(IndexBufferGround);
	hr = device->CreateBuffer(&indexBufferData_ground, &indexBufferDataSR_ground, &IndexBufferGround);
	

}

void DEMO_APP::MakeGrid(float depth, float width)
{
	
	float x = depth;
	float z = width;

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

	// vertecies ////////////////////////////////////////////////////////////
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

	// indecies //////////////////////////////////////////////////////
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

	D3D11_MAPPED_SUBRESOURCE mapped_resource_offest = { 0 };
	deviceContext->Map(constantBuffer_offset, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource_offest);
	memcpy(mapped_resource_offest.pData, &offset, sizeof(float));
	deviceContext->Unmap(constantBuffer_offset, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &constantBuffer_offset); // the 0 is slot index 0, 1 is the num buffers 

//	HRESULT hr =	device->CreateBuffer(&verteciesBufferDesc_grid, &vertexBufferData_grid, &VertBufferGrid);
//	hr =			device->CreateBuffer(&indexBufferData_grid, &indexBufferDataSR_grid, &IndexBufferGrid);

}

XMMATRIX DEMO_APP::PerspectiveProjectionMatrix(float FOV, float zFar, float zNear, float aspect)
{
	XMMATRIX ProjectionMatrix;

	//Yscale = cotangent(� Vertical FOV)
	float YScale = (sin(DegToRad(FOV * 0.5f)) / cos(DegToRad(FOV * 0.5f)));
	float XScale = YScale * (aspect);
	ProjectionMatrix.r[0] = { XScale,	0.0f,		0.0f,								0.0f};
	ProjectionMatrix.r[1] = { 0.0f,		YScale,		0.0f,								0.0f };
	ProjectionMatrix.r[2] = { 0.0f,		0.0f,		zFar / (zFar - zNear),				1.0f };
	ProjectionMatrix.r[3] = { 0.0f,		0.0f,		-(zFar * zNear) / (zFar - zNear),	0.0f };

	return ProjectionMatrix;
}

void DEMO_APP::PointLight1(Light * _light)
{
//	light->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
//	light->range = 100.0f;
//	light->att = XMFLOAT3(0.0f, 0.2f, 0.0f);
//	light->ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
//	light->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

}


bool DEMO_APP::LoadObj(char * filename, 
	std::vector < XMVECTOR > & out_vertices,
	std::vector < XMVECTOR > & out_uvs,
	std::vector < XMVECTOR > & out_normals
	)
{
	
	struct vert
	{
		float x;
		float y;
		float z;
	};

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;

	std::vector< XMVECTOR > temp_vertices;
	std::vector< XMVECTOR > temp_uvs;
	std::vector< XMVECTOR > temp_normals;


	FILE * file = fopen("teapot2.obj", "r");
	
	while (true)
	{

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break;

		//OBJ_VERT vertex;

		if (strcmp(lineHeader, "v") == 0)
		{
			vert vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
		
			XMVECTOR vec = XMVectorSet(vertex.x, vertex.y, vertex.z, 0.0f);
		
			temp_vertices.push_back(vec);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			vert vertex;
			fscanf(file, "%f %f\n", &vertex.x, &vertex.y, &vertex.z);
			XMVECTOR vec = XMVectorSet(vertex.x, vertex.y, vertex.z, 0.0f);
			temp_uvs.push_back(vec);

		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
		vert vertex;
	    fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
		XMVECTOR vec = XMVectorSet(vertex.x, vertex.y, vertex.z, 0.0f);

		temp_normals.push_back(vec);
	     }

		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;

			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);


		}
	}
	vert vertex;
	// processing 

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{

		unsigned int vertexIndex = vertexIndices[i];
		XMVECTOR vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{

		unsigned int vertexIndex = uvIndices[i];
		XMVECTOR vertex = temp_uvs[vertexIndex - 1];
		out_uvs.push_back(vertex);
	}

	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{

		unsigned int vertexIndex = normalIndices[i];
		XMVECTOR vertex = temp_normals[vertexIndex - 1];
		out_normals.push_back(vertex);
	}


	OBJ_VERT * model_data = new OBJ_VERT[out_vertices.size()+1];
	UINT * model_indicies = new UINT[out_vertices.size()+1];

	for (unsigned int i = 0; i < out_vertices.size(); i++)
	{

		model_data[i].pos[0] = XMVectorGetX(out_vertices[i]);
		model_data[i].pos[1] = XMVectorGetY(out_vertices[i]);
		model_data[i].pos[2] = XMVectorGetZ(out_vertices[i]);

		model_data[i].uvw[0] = XMVectorGetX(out_uvs[i]);
		model_data[i].uvw[1] = XMVectorGetY(out_uvs[i]);
		model_data[i].uvw[2] = XMVectorGetZ(out_uvs[i]);

		model_data[i].nrm[0] = XMVectorGetX(out_normals[i]);
		model_data[i].nrm[1] = XMVectorGetY(out_normals[i]);
		model_data[i].nrm[2] = XMVectorGetZ(out_normals[i]);

		model_indicies[i] = i;
		model_indicies_Size = i;
	}
	model_indicies_Size++;
	


	// loading verts 
	// verts 
	D3D11_BUFFER_DESC VertBufferData_model = { 0 };
	VertBufferData_model.Usage = D3D11_USAGE_IMMUTABLE;
	VertBufferData_model.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertBufferData_model.ByteWidth = sizeof(OBJ_VERT) * out_vertices.size();
	VertBufferData_model.MiscFlags = 0;
	VertBufferData_model.CPUAccessFlags = 0;
	VertBufferData_model.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VertBufferDataSR_model = { 0 };
	VertBufferDataSR_model.pSysMem = model_data;
	VertBufferDataSR_model.SysMemPitch = 0;
	VertBufferDataSR_model.SysMemSlicePitch = 0;

	SAFE_RELEASE(VertBufferModel);
	HRESULT hr = device->CreateBuffer(&VertBufferData_model, &VertBufferDataSR_model, &VertBufferModel);


	D3D11_BUFFER_DESC indexBufferData_model = { 0 };
	indexBufferData_model.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferData_model.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferData_model.ByteWidth = sizeof(float) * vertexIndices.size();
	indexBufferData_model.MiscFlags = 0;
	indexBufferData_model.CPUAccessFlags = 0;
	indexBufferData_model.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferDataSR_Model = { 0 };
	indexBufferDataSR_Model.pSysMem = model_indicies;
	indexBufferDataSR_Model.SysMemPitch = 0;
	indexBufferDataSR_Model.SysMemSlicePitch = 0;

	SAFE_RELEASE(IndexBufferModel);
	hr = device->CreateBuffer(&indexBufferData_model, &indexBufferDataSR_Model, &IndexBufferModel);

	return true;
}


//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************


bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6

	deviceContext->ClearState();

	SAFE_RELEASE(BackBuffer);
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
	SAFE_RELEASE(IndexBufferGround);
	SAFE_RELEASE(CubesTexture);
	SAFE_RELEASE(CubesTexSamplerState);
	SAFE_RELEASE(constantBuffer_Camera);
	SAFE_RELEASE(pDSV);
	SAFE_RELEASE(pDepthStencil);
//	SAFE_RELEASE(blendState);
//	SAFE_RELEASE(VertBufferGrid);
//	SAFE_RELEASE(VSGrid_p);
	SAFE_RELEASE(rasterizerState);
//	SAFE_RELEASE(PS_Grid_pointer);
	SAFE_RELEASE(Transparency);
	SAFE_RELEASE(CCWcullMode);
	SAFE_RELEASE(CWcullMode);
	SAFE_RELEASE(constantBuffer_offset); 
	SAFE_RELEASE(constantBuffer_Camera);
	SAFE_RELEASE(constantBufferPointLight);
//	SAFE_RELEASE(IndexBufferGrid);
	SAFE_RELEASE(constantBuffer_lightPosition);
	SAFE_RELEASE(VS_Buffer);
	SAFE_RELEASE(PS_Buffer);
	SAFE_RELEASE(chainBuffer);
	SAFE_RELEASE(CubesTexSamplerState);
	SAFE_RELEASE(Transparency);
	SAFE_RELEASE(VertBufferGround);
	SAFE_RELEASE(IndexBufferCube);
	SAFE_RELEASE(GroundTexture);
	SAFE_RELEASE(cbPerFrameBuffer); // for light 
	SAFE_RELEASE(constantBufferSphere);
	SAFE_RELEASE(constantBufferSky);
	SAFE_RELEASE(VertBufferSky);
	SAFE_RELEASE(IndexBufferSky);
	//skybox
	//SAFE_RELEASE(sphereIndexBuffer);
//	SAFE_RELEASE(sphereVertBuffer);
	SAFE_RELEASE(SKYMAP_VS);
	SAFE_RELEASE(SKYMAP_PS);
	SAFE_RELEASE(POINT_VS);
	SAFE_RELEASE(POINT_PS);

//	SAFE_RELEASE(SKYMAP_VS_Buffer);
//	SAFE_RELEASE(SKYMAP_PS_Buffer);
//	SAFE_RELEASE(smrv);
	SAFE_RELEASE(DSLessEqual);
	SAFE_RELEASE(RSCullNone);
	SAFE_RELEASE(SkyTexture);
	SAFE_RELEASE(VertBufferModel);
	SAFE_RELEASE(IndexBufferModel);
	//direct input
	DIKeyboard->Unacquire();
	DIMouse->Unacquire();
	DirectInput->Release();

	SAFE_RELEASE(DIKeyboard);
	SAFE_RELEASE(DIMouse);
	SAFE_RELEASE(DirectInput);

	UnregisterClass(L"DirectXApplication", application);

	return true;
}