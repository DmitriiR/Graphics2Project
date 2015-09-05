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
#include <DirectXMath.h>
#include "DDSTextureLoader.h"
#include <dinput.h>


using namespace std;
using namespace DirectX;



#define BACKBUFFER_WIDTH	1280
#define BACKBUFFER_HEIGHT	768
#define MOSE_SPEED			0.005f

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
	
	//ID3D11Buffer * VertBuffer;
	//ID3D11Buffer * VertBuffer_triangle;
	//ID3D11Buffer * VertBufferStar;
	ID3D11Buffer * VertBufferCube = nullptr; // << unload this
//	ID3D11Buffer * VertBufferGrid = nullptr;
	ID3D11Buffer * VertBufferGround = 0;
	//ID3D11Buffer * IndexBufferStar;
	ID3D11Buffer * IndexBufferCube = nullptr;
	ID3D11Buffer * IndexBufferGround = 0;;
//	ID3D11Buffer * IndexBufferGrid = nullptr;
	ID3D11Buffer * VS_Buffer = nullptr;
	ID3D11Buffer * PS_Buffer = nullptr;
	ID3D11Buffer * constantBuffer = nullptr;
	ID3D11Buffer * constantBuffer_offset = nullptr; //<< clear later
	ID3D11Buffer * constantBuffer_Camera = nullptr;

	ID3D11Resource				* chainBuffer = nullptr;
	ID3D11ShaderResourceView	* CubesTexture = nullptr;
	ID3D11SamplerState			* CubesTexSamplerState = nullptr;

	ID3D11PixelShader* PS = nullptr;
	//ID3D11PixelShader* PS_Grid_pointer = nullptr;
	ID3D11VertexShader* VS = nullptr;
	ID3D11VertexShader* VSStar = nullptr;
	//ID3D11VertexShader* VSGrid_p = nullptr;

	ID3D11DepthStencilView* pDSV = nullptr;
	ID3D11Buffer* cbPerFrameBuffer = nullptr;

	ID3D11Texture2D* pDepthStencil = NULL;

	// All Shader Variables (Always Pre-fixed by “SV_”)
	XMMATRIX SV_WorldMatrix = XMMatrixIdentity();
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

	struct Light
	{
		Light()
		{
			ZeroMemory(this, sizeof(Light));
		}

		XMFLOAT3	dir;
		float		pad;
		XMFLOAT4	ambient;
		XMFLOAT4	diffuse;
	};

	Light light;

	struct cbPerFrame
	{
		Light light;
	};

	cbPerFrame constbuffPerFrame;


	struct SEND_TO_VRAM
	{
		float constantColor[4];
		float constantOffset[2];
		float padding[2];
	};

	struct WorldBuffer {
		XMFLOAT4X4 worldMatrix;
	};

	struct Scene {
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projMatrix;

	};
	// TODO: PART 3 STEP 4a
	SEND_TO_VRAM toShader;

public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void ResizeWindow(); // prototype the resize 
	void UpdateCamera(double deltaTime); // prototype the camera function
	bool InitializeDirectInput(HINSTANCE hInstance); // initialize the direct input
	void DetectInput(double deltaTime);
	void MakeCube();
	void MakeGround(float scale = 2.0f);
	void MakeGrid(float depth, float width);
	XMMATRIX PerspectiveProjectionMatrix(float FOV, float zFar, float zNear, float aspect);
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

	
	// fill the back buffer
	hr = chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&chainBuffer);

	//Create the Render Target
	hr = device->CreateRenderTargetView(chainBuffer, NULL, &renderTargetView);

	// this a window in the backbuffer 
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = BACKBUFFER_WIDTH;
	viewPort.Height = BACKBUFFER_HEIGHT;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;


// MAKES CUVBE buffers
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


///////////MAKE THE CUBE /////////////////////////
MakeCube();///////////////////////////////////////
//////////////////////////////////////////////////

///////// MAKE THE GROUND ////////////////////////
MakeGround();/////////////////////////////////////
//////////////////////////////////////////////////


D3D11_INPUT_ELEMENT_DESC vLayout[] =
{
	{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UVM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NRM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

	D3D11_BUFFER_DESC constBufferDesc = { 0 };
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.ByteWidth = sizeof(SV_WorldMatrix);
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

	// cube texturing

//	ID3D11Texture2D * tex;
//	D3D11_TEXTURE2D_DESC tdesc;
//	D3D11_SUBRESOURCE_DATA tbsd[numbers_test_numlevels];
//	ZeroMemory(tbsd, sizeof(tbsd));
//	for (unsigned int i = 0; i < numbers_test_numlevels; i++)
//	{
//		tbsd[i].pSysMem = &numbers_test_pixels[numbers_test_leveloffsets[i]] ;
//		tbsd[i].SysMemPitch = sizeof(UINT) * (numbers_test_width >> i);
//	}
//
//
//	tdesc.Width = numbers_test_width ;
//	tdesc.Height = numbers_test_height;
//	tdesc.MipLevels = numbers_test_numlevels;
//	tdesc.ArraySize = 1;
//
//	tdesc.SampleDesc.Count = 1;
//	tdesc.SampleDesc.Quality = 0;
//	tdesc.Usage = D3D11_USAGE_DEFAULT;
//	tdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//
//	tdesc.CPUAccessFlags = 0;
//	tdesc.MiscFlags = 0;
//
//	device->CreateTexture2D(&tdesc, tbsd, &tex);
//	hr = device->CreateSamplerState(&samplerDesc, &CubesTexSamplerState);
//	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceView_desc;
//	shaderResourceView_desc.Format = tdesc.Format;
//	shaderResourceView_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	shaderResourceView_desc.Texture2D.MostDetailedMip = 0;
//	shaderResourceView_desc.Texture2D.MipLevels = numbers_test_numlevels;
//	
//	////// get TEXTURE from file 
//	SAFE_RELEASE(tex);// !!!
	hr = CreateDDSTextureFromFile(device, L"fieldDDs.dds", NULL, &CubesTexture);

	

	// set the shaders
	device->CreateVertexShader(VS_Star, sizeof(VS_Star), nullptr, &VSStar);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &PS); // first create the shaders

	device->CreateInputLayout(vLayout, 3, VS_Star, sizeof(VS_Star), &pInputLayout);







	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ANTIALISING 
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

	rtbd.BlendEnable			= true;
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

	cmdesc.FrontCounterClockwise = true;
	hr = device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = device->CreateRasterizerState(&cmdesc, &CWcullMode);

	//*******************************************************
	// Member function for window resize 
	ResizeWindow();


	// LIGHT //////////////////////////////////////////////   setup 
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = device->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);


	// Setting Up the light 
	light.dir		= XMFLOAT3(0.25f,	0.5f, -1.0f);
	light.ambient	= XMFLOAT4(0.2f,	0.2f, 0.2f, 1.0f);
	light.diffuse	= XMFLOAT4(1.0f,	1.0f, 1.0f, 1.0f);



}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{


	deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL); // this is thwre the Z bugger will go 

	////////////////// BACKGROUND COLOR reset
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

	

	// LIGHT ////////////////////////////////////////////////////////////////////////////
	constbuffPerFrame.light = light;
	deviceContext->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);
	////////////////////////////////////////////////////////////////////////////////////




	const UINT stride = sizeof(OBJ_VERT);
	UINT offest = 0;
	
	UINT offset;
	offset = 0;
	
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // the 0 is slot index 0, 1 is the num buffers 
	deviceContext->PSSetShaderResources(0, 1, &CubesTexture);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);
	deviceContext->RSSetState(rasterizerState);
	
	
	/////////// DRAW GROUND//////////////////////
	deviceContext->IASetInputLayout(pInputLayout);
	deviceContext->IASetIndexBuffer(IndexBufferGround, DXGI_FORMAT_R32_UINT, offset);
	deviceContext->IASetVertexBuffers(0, 1, &VertBufferGround, &stride, &offest);
	deviceContext->VSSetShader(VSStar, nullptr, NULL);
	deviceContext->PSSetShader(PS, nullptr, NULL);

	deviceContext->DrawIndexed(6, 0, 0);
	

	/////////// DRAW CUBE/////////////////////////
	deviceContext->IASetInputLayout(pInputLayout);
	deviceContext->IASetVertexBuffers(0, 1, &VertBufferCube, &stride, &offest);
	deviceContext->IASetIndexBuffer(IndexBufferCube, DXGI_FORMAT_R32_UINT, offest);
	deviceContext->VSSetShader(VSStar, nullptr, NULL);
	deviceContext->PSSetShader(PS, nullptr, NULL);
	//culling 
	deviceContext->RSSetState(CCWcullMode); // counter clockwise 
	deviceContext->DrawIndexed(ARRAYSIZE(Cube_indicies), 0, 0);

	deviceContext->RSSetState(CWcullMode);  // clockwise 
	deviceContext->DrawIndexed(ARRAYSIZE(Cube_indicies), 0, 0);
	//****************************************************************************
	
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************


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
	DIMOUSESTATE mouseCurrState;

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

//void DEMO_APP::Update(double deltaTime)
//{
//
//	//Keep the cubes rotating
//	rot += 1.0f * deltaTime;
//	if (rot > 6.28f)
//		rot = 0.0f;
//
//	//Reset cube1World
//	SV_WorldMatrix = XMMatrixIdentity();
//
//	//Define cube1's world space matrix
//	///////////////**************new**************////////////////////
//	XMVECTOR rotyaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//	XMVECTOR rotzaxis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
//	XMVECTOR rotxaxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//
//	// rotate through the access 
//	Rotation = XMMatrixRotationAxis(rotyaxis, rot);
//	Rotationx = XMMatrixRotationAxis(rotxaxis, rotx);
//	Rotationz = XMMatrixRotationAxis(rotzaxis, rotz);
//	Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
//
//	//Set cube1's world space using the transformations
//	SV_WorldMatrix = Translation * Rotation * Rotationx * Rotationz;
//	///////////////**************new**************////////////////////
//
//	//	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
//
////	XMVECTOR rotyaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
////	XMVECTOR rotzaxis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
////	XMVECTOR rotxaxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
////
////	Rotation = XMMatrixRotationAxis(rotyaxis, rot);
////	Rotationx = XMMatrixRotationAxis(rotxaxis, rotx);
////	Rotationz = XMMatrixRotationAxis(rotzaxis, rotz);
////	Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
////
////	// get the vire matrix 
////	SV_ViewMatrix = XMMatrixInverse(NULL, XMMatrixInverse(NULL, SV_WorldMatrix));
////	// do input and manipulation
////	DetectInput(deltatime);
////
////	//inverse it back
////	SV_ViewMatrix = XMMatrixInverse(NULL, XMMatrixInverse(NULL, SV_WorldMatrix));
////
////
////	SV_WorldMatrix = Translation * Rotation * Rotationx * Rotationz;
////
////	//	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
////	//	camTarget = XMVector3Normalize(camTarget);
//
//}

void DEMO_APP::UpdateCamera(double deltaTime)
{
	
	//SV_Perspective = XMMatrixPerspectiveLH(1.0f, 1.0f,1.0f,10.0f);

	
	SV_Perspective  = theApp->PerspectiveProjectionMatrix(90.0f, 100.0f, 0.1f, Aspect);

	//SV_Perspective = Perspective_Projection_Matrix(90.0f, 100.0f, 0.1f, Aspect); // <<<<<< Projection 
	// not using the roll paramenter, so setting to 0
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0.0f);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	// finding the new direction of the camera 
	XMMATRIX temp_RotateOnY;
	temp_RotateOnY = XMMatrixRotationY(camPitch);

	camRight = XMVector3TransformCoord(DefaultRight, temp_RotateOnY);
	camForward = XMVector3TransformCoord(DefaultForward, temp_RotateOnY);
	camUp = XMVector3TransformCoord(camUp, temp_RotateOnY);

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

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

	//Set cube1's world space using the transformations
	SV_WorldMatrix = Translation * RotationX * RotationY * RotationZ;

	// Loading the buffers
	D3D11_BUFFER_DESC constBufferDesc_camera = { 0 };
	constBufferDesc_camera.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc_camera.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//constBufferDesc.ByteWidth = sizeof(SEND_TO_VRAM);
	constBufferDesc_camera.ByteWidth = sizeof(XMMATRIX) * 2;
	constBufferDesc_camera.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&constBufferDesc_camera, nullptr, &constantBuffer_Camera);

	//// MAP ////////////////////////////////////////////////
	/// constantBuffer_Camera

	D3D11_MAPPED_SUBRESOURCE mapped_resource = { 0 };
	XMMATRIX scene[2] = { SV_ViewMatrix, SV_Perspective };
	deviceContext->Map(constantBuffer_Camera, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, scene, sizeof(scene));
	deviceContext->Unmap(constantBuffer_Camera, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &constantBuffer_Camera);

	////////////////////////////////////////////////////////
	/// SV_WorldMatrix
	float offset_for0;
	deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	memcpy(mapped_resource.pData, &SV_WorldMatrix, sizeof(SV_WorldMatrix));
	deviceContext->Unmap(constantBuffer, 0);

	
	// TODO: PART 3 STEP 6
	deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // the 0 is slot index 0, 1 is the num buffers 

	// Bind the depth stencil view
	deviceContext->OMSetRenderTargets(1,          // One rendertarget view
		&renderTargetView,						  // Render target view, created earlier
		pDSV);									  // Depth stencil view for the render target



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


}

void DEMO_APP::MakeGround(float scale)
{
	OBJ_VERT Ground_data[4] = { 0 };
	// Generating grid 
	float x_pos = -1.0f;
	float z_pos =  1.0f;

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

	//for (UINT i = 0; i < 4; i++)
	//{
	//	XMVECTOR temp = { Ground_data[i].pos[0], Ground_data[i].pos[1], Ground_data[i].pos[2], 0 };
	//	temp = XMVector4Transform(temp, XMMatrixScaling(scale,0, scale));
	//	memcpy(&Ground_data[i].pos[0], &temp, sizeof(Ground_data[i]));
	//}


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
	
	// indeciies 

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

	//Yscale = cotangent(½ Vertical FOV)
	float YScale = (sin(DegToRad(FOV * 0.5f)) / cos(DegToRad(FOV * 0.5f)));
	float XScale = YScale * (aspect);
	ProjectionMatrix.r[0] = { XScale,	0.0f,		0.0f,								0.0f};
	ProjectionMatrix.r[1] = { 0.0f,		YScale,		0.0f,								0.0f };
	ProjectionMatrix.r[2] = { 0.0f,		0.0f,		zFar / (zFar - zNear),				1.0f };
	ProjectionMatrix.r[3] = { 0.0f,		0.0f,		-(zFar * zNear) / (zFar - zNear),	0.0f };

	return ProjectionMatrix;
}


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
//	SAFE_RELEASE(IndexBufferGrid);
	SAFE_RELEASE(VS_Buffer);
	SAFE_RELEASE(PS_Buffer);
	SAFE_RELEASE(chainBuffer);
	SAFE_RELEASE(CubesTexSamplerState);
	SAFE_RELEASE(Transparency);
	SAFE_RELEASE(VertBufferGround);
	SAFE_RELEASE(IndexBufferCube);

	SAFE_RELEASE(cbPerFrameBuffer); // light 
	
	DIKeyboard->Unacquire();
	DIMouse->Unacquire();
	DirectInput->Release();

	SAFE_RELEASE(DIKeyboard);
	SAFE_RELEASE(DIMouse);
	SAFE_RELEASE(DirectInput);
	
	UnregisterClass(L"DirectXApplication", application);

	return true;
}