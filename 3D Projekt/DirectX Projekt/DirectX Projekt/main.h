#ifndef MAIN_H
#define MAIN_H

#include "ObjImport.h"
#include "GameTime.h"
#include "Billboard.h"

struct MatrixBuffer
{
	XMMATRIX WVP;
	XMMATRIX World;
	XMMATRIX WorldView;
	XMMATRIX ProjMatrix;
	XMFLOAT4 diffuseColor;
	int hasTexture;
};

class Main
{

public:

	void CreateShaders();
	void CreateBuffers();
	void CreateStates();
	void SetViewport();
	XMMATRIX UpdateObj(ObjImport* obj, XMFLOAT3 otrans, XMFLOAT3 oscale);
	void Render();
	void FpsCounter();
	//int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
	HRESULT CreateDirect3DContext(HWND wndHandle);
	HWND InitWindow(HINSTANCE hInstance);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//LRESULT CALLBACK CallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
	/*struct MatrixBuffer;*/
	

	//Controller Functions
	bool InitDirectInput(HINSTANCE hInstance);
	void UpdateCamera(void);
	void DetectInput();


	GameTimer mTimer;
	std::wstring mMainWndCaption;
	HWND handle;

	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;


	MatrixBuffer cbPerObj;
	
	ObjImport* o_import = nullptr;

	Main();
	~Main();

	//General
	IDXGISwapChain* gSwapChain ;
	ID3D11Device* gDevice ;
	ID3D11DeviceContext* gDeviceContext ;
	ID3D11RenderTargetView* gBackbufferRTV ;
	ID3D11ShaderResourceView* gTextureView ;

	ID3D11InputLayout* gVertexLayout ;
	ID3D11InputLayout* gBillVertLayout ;
	ID3D11DepthStencilView* gDepthStencilView ;
	ID3D11Texture2D* gDepthStencilBuffer ;

	ID3D11Buffer* gVertexBuffer ;
	//ID3D11Buffer* gIndexBuffer;
	ID3D11VertexShader* gVertexShader ;
	ID3D11PixelShader* gPixelShader ;
	ID3D11GeometryShader* gGeometryShader;

	//New from Attila
	ID3D11Buffer* gConstantBufferCamera;

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;

	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	//Objects
	ObjImport* sphrThingy;

	//Billboards and particles
	ID3D11VertexShader* billboardVS;
	ID3D11GeometryShader* billboardGS;
	ID3D11PixelShader* billboardPS;
	Billboard* billBalls;
	ID3D11Buffer* bbVertexBuffer = nullptr;
	
	//Blending and texturing
	ID3D11RasterizerState* RSCullCW ;
	ID3D11RasterizerState* RSCullCCW = nullptr;
	ID3D11BlendState* blendTransparency = nullptr;
	ID3D11RasterizerState* RSCullNone = nullptr;
	ID3D11SamplerState* gBillTexSampler;

	//Camera Objects
	XMMATRIX WVP;
	XMMATRIX World;
	XMMATRIX camView;
	XMMATRIX camProjection;

	XMMATRIX WorldView;


	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;


	XMVECTOR DefaultForward ;
	XMVECTOR DefaultRight ;
	XMVECTOR camForward ;
	XMVECTOR camRight ;

	XMMATRIX camRotationMatrix;
	XMMATRIX groundWorld;

	float moveLeftRight /*= 0.0f*/;
	float moveBackForward /*= 0.0f*/;

	float camYaw /*= 0.0f*/;
	float camPitch /*= 0.0f*/;


protected:





};


#endif