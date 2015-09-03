#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>
#include "defines.h"
#include <DirectXMath.h>

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

DirectX::XMMATRIX Rotationx;
DirectX::XMMATRIX Rotationz;

