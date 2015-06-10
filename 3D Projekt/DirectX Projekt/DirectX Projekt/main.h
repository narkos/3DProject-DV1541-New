#ifndef MAIN_H
#define MAIN_H

#include "ObjImport.h"
#include "GameTime.h"
#include "bbParticles.h"
#include "BlendMapObject.h"

struct MatrixBuffer
{
	XMMATRIX WVP;
	XMMATRIX World;
	XMMATRIX WorldView;
	XMMATRIX ProjMatrix;
	XMFLOAT4 diffuseColor;
	int hasTexture;
};

struct InverseMatrix 
{
	XMMATRIX InvWorldView;
	XMFLOAT4 camPos;
};

class Main
{

public:
	void CreateShaders();
	void CreateBuffers();
	void CreateStates();
	void SetViewport();
	
	void Render();
	void Update();
	void FpsCounter();
	HRESULT CreateDirect3DContext(HWND wndHandle);
	HWND InitWindow(HINSTANCE hInstance);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	XMMATRIX UpdateObj(ObjImport* obj, XMFLOAT3 otrans, XMFLOAT3 oscale);

	//Controller Functions
	bool InitDirectInput(HINSTANCE hInstance);
	void UpdateCamera(void);
	void DetectInput();


	//State functions
	void setDepthStencilOff();
	void setDepthStencilOn();
	void setAlphaBlendingOff();
	void setAlphaBlendingOn();
	void setAlphaBlendingAdd();


	GameTimer mTimer;
	std::wstring mMainWndCaption;
	HWND handle;

	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;


	MatrixBuffer cbPerObj;
	InverseMatrix cbInverseObj;
	
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
	ID3D11Buffer* gConstantBufferInverseView;

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;

	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	//Imported Models
	ObjImport* m_sphrThingy;
	ObjImport* m_cubeObjects;

	//Billboards and particles
	ID3D11VertexShader* billboardVS;
	ID3D11GeometryShader* billboardGS;
	ID3D11PixelShader* billboardPS;
	Billboard* billBalls;
	ID3D11Buffer* bbVertexBuffer = nullptr;
	ParticleSystem* partTest;



	//Blending and texturing
	ID3D11DepthStencilState* gDepthStencilState = nullptr;
	ID3D11DepthStencilState* gDepthStencilStateDisable = nullptr;
	ID3D11RasterizerState* RSCullCW ;
	ID3D11RasterizerState* RSCullCCW = nullptr;
	ID3D11BlendState* gBlendStateDefault = nullptr;
	ID3D11BlendState* gBlendStateAdd = nullptr;
	ID3D11BlendState* gBlendStateTrans = nullptr;
	ID3D11SamplerState* gSampStateAni = nullptr;
	ID3D11SamplerState* gSampStateLin = nullptr;
	ID3D11RasterizerState* RSCullNone = nullptr;
	

	//BlendMap
	ID3D11PixelShader* gAlphaMapPixelShader;
	BlendMapObj* t_cubeTexArray;
	BlendMapObj* t_testTexArray;

	// Back-Face Culling
	ID3D11GeometryShader* BCGeometryShader;


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

	int part_runParticles;
	int part_addParticles;
	int part_delParticles;

	float camYaw /*= 0.0f*/;
	float camPitch /*= 0.0f*/;


protected:





};


#endif