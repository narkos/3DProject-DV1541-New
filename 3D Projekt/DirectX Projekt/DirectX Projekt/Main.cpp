
#include "main.h"
 
namespace { Main* mainName = 0; }

LRESULT CALLBACK CallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return mainName->WndProc(hWnd, message, wParam, lParam);
}

void Main::CreateShaders()
{

	//------------VertexShader-----------------------------------------------------------------------------------------------------------
	ID3DBlob* pVS = nullptr;
	RootName::CompileShader(L"VertexShader.hlsl", "VS_main", "vs_5_0", &pVS);
	
	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);
	
	
	//------------Create input layout-----------------------------------------------------------------------------------------------
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0 , 0 ,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};
	
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	pVS->Release();



	//--------------PixelShader----------------------------------------------------------------------------------------------------------------
	ID3DBlob* pPS = nullptr;
	RootName::CompileShader(L"PixelShader.hlsl", "PS_main", "ps_5_0", &pPS);
	
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	pPS->Release();

	//----------------GeometryShader-------------------------------------------------------------------------------------------------------------
	/*ID3DBlob* pGS = nullptr;
	CompileShader(L"GeometryShader.hlsl", "GS_main", "gs_5_0", &pGS);
		
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);
	pGS->Release();*/


	//---------------Billboard Shaders -----------------------------------------------//
	// Billboard Vertex Shader

	ID3DBlob* pBBVS = nullptr;

	RootName::CompileShader(L"bbVertexShader.hlsl", "BBVS_main", "vs_5_0", &pBBVS);

	gDevice->CreateVertexShader(pBBVS->GetBufferPointer(), pBBVS->GetBufferSize(), nullptr, &billboardVS);

	// Billboard input layout
	D3D11_INPUT_ELEMENT_DESC bbinputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	gDevice->CreateInputLayout(bbinputDesc, ARRAYSIZE(bbinputDesc), pBBVS->GetBufferPointer(), pBBVS->GetBufferSize(), &gBillVertLayout);
	pBBVS->Release();

	// Billboard Geometry Shader
	ID3DBlob* pBBGS = nullptr;
	RootName::CompileShader(L"bbGeometryShader.hlsl", "BBGS_main", "gs_5_0", &pBBGS);

	gDevice->CreateGeometryShader(pBBGS->GetBufferPointer(), pBBGS->GetBufferSize(), nullptr, &billboardGS);
	pBBGS->Release();

	//Billboard Pixel Shader
	ID3DBlob* pBBPS = nullptr;
	RootName::CompileShader(L"bbPixelShader.hlsl", "BBPS_main", "ps_5_0", &pBBPS);

	gDevice->CreatePixelShader(pBBPS->GetBufferPointer(), pBBPS->GetBufferSize(), nullptr, &billboardPS);
	pBBPS->Release();

	
	//ParticleSystem* partTest = new ParticleSystem(gDevice, 1);
}

bool Main::InitDirectInput(HINSTANCE hInstance)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance,
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
	hr = DIKeyboard->SetCooperativeLevel(handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(handle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}
 
 
void Main::UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	//Free-Look Camera
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;


	camTarget = camPosition + camTarget;

	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
 
 
}

void Main::DetectInput()
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	//Exit program when escape is pushed down
	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(handle, WM_DESTROY, 0, 0);

	float speed = 0.005f;

	if (keyboardState[DIK_A] & 0x80 || keyboardState[DIK_LEFT] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if (keyboardState[DIK_D] & 0x80 || keyboardState[DIK_RIGHT] & 0x80)
	{
		moveLeftRight += speed;
	}
	if (keyboardState[DIK_W] & 0x80 || keyboardState[DIK_UP] & 0x80)
	{
		moveBackForward += speed;
	}
	if (keyboardState[DIK_S] & 0x80 || keyboardState[DIK_DOWN] & 0x80)
	{
		moveBackForward -= speed;
	}
	part_addParticles = 0;
	if (keyboardState[DIK_P] & 0x80)			// Press 'P' Key to start particle simulation
	{
		if (part_runParticles == 1)
		{
			part_runParticles = 0;
		}
		else if (part_runParticles == 0)
		{
			part_runParticles = 1;
		}		
	}
	if (keyboardState[DIK_O] && 0x80)
	{
		part_addParticles += 1;
	}


	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.001f;

		camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}


	return;
}


void Main::FpsCounter()
{
	

	// Code computes the avarage frame time and frames per second

	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	//Function is beeing initialiased every time a frame is made
	frameCount++;

	// Compute averages over one second period
	if (((mTimer.TotalTime()) - timeElapsed) >= 0.50f)
	{
		float fps = (float)frameCount; // fps = framecount / 1
		float mspf = 1000.0f / fps;
		
		// Makes a String for the window handler
		std::wostringstream outs;

		outs.precision(6);
		outs << mMainWndCaption << L" "
			<< L"        FPS: " << fps << L" "
			<< L"        Frame Time: " << mspf << L" (ms)";
			
		//Prints the text in the window handler
		SetWindowText(handle, outs.str().c_str());

		// Reset for next fps.
		frameCount = 0;
		timeElapsed += 0.25f;

	}

}

void Main::CreateBuffers()
{

	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b;
		float Nx, Ny, Nz;
	}
	triangleVertices[4] =
	{
		-0.5f, -0.5f, 5.0f,	//v0 pos
		0.0f, 1.0f, 	//v0 Color
		0.0f, 0.0f, -1.0f,

		-0.5f, 0.5f, 5.0f,	//v1
		0.0f, 0.0f, 	//v1 
		0.0f, 0.0f, -1.0f,
		
		0.5f, -0.5f, 5.0f, //v2
		1.0f, 1.0f,	//
		0.0f, 0.0f, -1.0f,
		
		0.5f, 0.5f, 5.0f, //v3
		1.0f, 0.0f,	
		0.0f, 0.0f, -1.0f
	};

	D3D11_BUFFER_DESC vBufferDesc;
	memset(&vBufferDesc, 0, sizeof(vBufferDesc));
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	gDevice->CreateBuffer(&vBufferDesc, &data, &gVertexBuffer);

	// Import Obj Data
	sphrThingy = new ObjImport(L"Assets\\shprThingy_01.obj", gDevice, true, true);


	////////////////////////////////////////////////
	// Create Billboard/Particle Data
	billBalls = new Billboard(gDevice,gDeviceContext, XMFLOAT3(3.0f, 1.0f, 2.0f), 0.5f, 0.5f, L"Assets/Particle_Blue_01.png");
	BBVert billVerts[2];
	billVerts[0].Pos = billBalls->billBaseVert.Pos;
	billVerts[0].Size = billBalls->billBaseVert.Size;
	billVerts[1].Pos = XMFLOAT3(billBalls->billBaseVert.Pos.x - 1.0f, billBalls->billBaseVert.Pos.y - 0.2f, billBalls->billBaseVert.Pos.z);
	billVerts[1].Size = billBalls->billBaseVert.Size;

	D3D11_BUFFER_DESC vbbBufferDesc;
	memset(&vbbBufferDesc, 0, sizeof(vbbBufferDesc));
	vbbBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbbBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbbBufferDesc.ByteWidth = sizeof(BBVert);

	D3D11_SUBRESOURCE_DATA bbdata;
	bbdata.pSysMem = billVerts;
	gDevice->CreateBuffer(&vbbBufferDesc, &bbdata, &bbVertexBuffer);

	////////////////////////////////////////////
	// Create Per Object Constant Buffer
	D3D11_BUFFER_DESC cBufferDesc;
	memset(&cBufferDesc, 0, sizeof(cBufferDesc));
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	gDevice->CreateBuffer(&cBufferDesc, NULL, &gConstantBufferCamera);


	//////
	//Create Particle System
	XMFLOAT4 emittLoc = XMFLOAT4(20.0f, 2.0f, 30.0f, 0.0f);
	XMFLOAT4 omittLoc = XMFLOAT4(20.0f, 2.0f, -30.0f, 0.0f);

	partTest = new ParticleSystem(gDevice, gDeviceContext, gSampStateLin, emittLoc, omittLoc, L"Assets/Particle_Blue_01.png", 512*512);
	

	partTest->InitShaders();
	//partTest->UpdateParticles(mTimer.DeltaTime(), nullptr);
}

void Main::CreateStates()
{

	//Create Blend State - Add
	D3D11_BLEND_DESC tr_blendDesc;
	ZeroMemory(&tr_blendDesc, sizeof(tr_blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC tr_rendTargBlendDesc;
	ZeroMemory(&tr_rendTargBlendDesc, sizeof(tr_rendTargBlendDesc));
	tr_rendTargBlendDesc.BlendEnable = true;
	tr_rendTargBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	tr_rendTargBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	tr_rendTargBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	tr_rendTargBlendDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
	tr_rendTargBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	tr_rendTargBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tr_rendTargBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	tr_blendDesc.AlphaToCoverageEnable = false;
	tr_blendDesc.RenderTarget[0] = tr_rendTargBlendDesc;
	gDevice->CreateBlendState(&tr_blendDesc, &gBlendStateAdd);

	HRESULT hr;
	//Create Blend State - Transparency
	D3D11_RENDER_TARGET_BLEND_DESC blendStateRTDesc;
	blendStateRTDesc.BlendEnable = true;
	blendStateRTDesc.SrcBlend = D3D11_BLEND_ONE;
	blendStateRTDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateRTDesc.BlendOp = D3D11_BLEND_OP_ADD;
	blendStateRTDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateRTDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateRTDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateRTDesc.RenderTargetWriteMask = 0x0f;

	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.RenderTarget[0] = blendStateRTDesc;

	hr = gDevice->CreateBlendState(&blendStateDesc, &gBlendStateTrans);



	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	gDevice->CreateSamplerState(&samplerDesc, &gSampStateAni);

	
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	gDevice->CreateSamplerState(&samplerDesc, &gSampStateLin);


	// Create Rasterizer States / Cull Modes
	D3D11_RASTERIZER_DESC cullModeDesc;
	memset(&cullModeDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	cullModeDesc.FillMode = D3D11_FILL_SOLID;
	cullModeDesc.CullMode = D3D11_CULL_BACK;
	cullModeDesc.FrontCounterClockwise = true;
	//Create CCW CullMode
	hr = gDevice->CreateRasterizerState(&cullModeDesc, &RSCullCCW);
	//Create CW CullMode
	cullModeDesc.FrontCounterClockwise = false;
	hr = gDevice->CreateRasterizerState(&cullModeDesc, &RSCullCW);
	//Create NONE CullMode
	cullModeDesc.CullMode = D3D11_CULL_NONE;
	hr = gDevice->CreateRasterizerState(&cullModeDesc, &RSCullNone);

}


XMMATRIX Main::UpdateObj(ObjImport* obj, XMFLOAT3 otrans, XMFLOAT3 oscale)
{
	// Hold and update space matricies before rendering.
	XMMATRIX meshWorld = XMLoadFloat4x4(&obj->o_meshWorldMTX);
	meshWorld = XMMatrixIdentity();
	Rotation = XMMatrixRotationY(3.14f);
	Scale = XMMatrixScaling(oscale.x, oscale.y, oscale.z);
	Translation = XMMatrixTranslation(otrans.x, otrans.y, otrans.z);
	meshWorld = Rotation *  Scale * Translation;
	return meshWorld;

}

void Main::Update()
{


}


void Main::Render()
{
	float clearColor[] = { 0, 0, 0, 1 };
	UINT32 vertexSize = sizeof(float) * 8;
	UINT32 offset = 0;
	gDeviceContext->OMSetBlendState(0,0, 0xffffffff);
	//Initialize WorldMatrix
	World = XMMatrixIdentity();
	setDepthStencilOn();

	gDeviceContext->IASetInputLayout(gVertexLayout);


	DetectInput();
	UpdateCamera();

	WorldView = World * camView;
	camProjection = XMMatrixPerspectiveFovLH((3.14f*(0.4f)), (640.0f / 480.0f), 0.5f, 1000.0f);
	WVP = XMMatrixMultiply(World, XMMatrixMultiply(camView, camProjection));
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	cbPerObj.hasTexture = false;

	gDeviceContext->UpdateSubresource(gConstantBufferCamera, 0, NULL, &cbPerObj, 0, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBufferCamera);

	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	setDepthStencilOn();
	setAlphaBlendingOff();

	gDeviceContext->IASetInputLayout(gVertexLayout);
	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	//Set Shaders and texture
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
	//gDeviceContext->PSSetShaderResources(0, 1, &gTextureView);

	//Draw Object with 4 vertices
	gDeviceContext->Draw(4, 0);

///////////////////////////////////////////////////////
	//// Draw Imported Objects
	XMMATRIX meshWorld = UpdateObj(sphrThingy, XMFLOAT3(-1.0f, 0.0f, 2.0f), XMFLOAT3(0.2f, 0.2f, 0.2f));
	
	for (int i = 0; i < sphrThingy->o_meshGroups; ++i)
	{
		gDeviceContext->IASetIndexBuffer(sphrThingy->o_meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);
		gDeviceContext->IASetVertexBuffers(0, 1, &sphrThingy->o_meshVertBuff, &vertexSize, &offset);


		WVP = meshWorld * camView * camProjection;
		
		cbPerObj.WVP = XMMatrixTranspose(WVP);
		cbPerObj.diffuseColor = sphrThingy->o_materials[sphrThingy->o_meshGroupTexture[i]].oM_difColor;
		cbPerObj.hasTexture = sphrThingy->o_materials[sphrThingy->o_meshGroupTexture[i]].oM_hasTexture;
		gDeviceContext->UpdateSubresource(gConstantBufferCamera, 0, NULL, &cbPerObj, 0, 0); 
		gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBufferCamera);
		gDeviceContext->PSSetConstantBuffers(0, 1, &gConstantBufferCamera);
		
		//Checks if the imported submesh have a texture
		if (sphrThingy->o_materials[sphrThingy->o_meshGroupTexture[i]].oM_hasTexture == true)
		{
			gDeviceContext->PSSetShaderResources(0, 1, &sphrThingy->o_meshSRV[sphrThingy->o_materials[sphrThingy->o_meshGroupTexture[i]].oM_texIndex]);
		}
		int indexStart = sphrThingy->o_meshGroupIndexStart[i];
		int indexDrawAmount = sphrThingy->o_meshGroupIndexStart[i + 1] - sphrThingy->o_meshGroupIndexStart[i];
		gDeviceContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			
	}
	setDepthStencilOff();
	setAlphaBlendingAdd();
	// DRAW BILLBOARD
	//float blendFactor[] = { 0.75f, 0.75f, 0.75f, 0.75f };
	//gDeviceContext->OMSetBlendState(gBlendStateAdd, blendFactor, 0xffffffff);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gDeviceContext->VSSetShader(billboardVS, nullptr, 0);
	gDeviceContext->GSSetShader(billboardGS, nullptr, 0);
	gDeviceContext->PSSetShader(billboardPS, nullptr, 0);
	UINT bbvertexSize = sizeof(float) * 5;
	UINT bboffset = 0;
	gDeviceContext->IASetInputLayout(gBillVertLayout);
	gDeviceContext->IASetVertexBuffers(0, 1, &bbVertexBuffer, &bbvertexSize, &bboffset);
	
	World = XMMatrixIdentity();
	WorldView = World * camView;
	WVP = World * camView *  camProjection;
	cbPerObj.WorldView = XMMatrixTranspose(WorldView);
	cbPerObj.ProjMatrix = XMMatrixTranspose(camProjection);
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	cbPerObj.World = XMMatrixTranspose(World);
	cbPerObj.hasTexture = billBalls->hasTexture;
	gDeviceContext->UpdateSubresource(gConstantBufferCamera, 0, NULL, &cbPerObj, 0, 0);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBufferCamera);
	gDeviceContext->PSSetConstantBuffers(0, 1, &gConstantBufferCamera);
	gDeviceContext->PSSetShaderResources(0, 1, &billBalls->billSRV);
	gDeviceContext->PSSetSamplers(0, 1, &gSampStateLin);
	
	//gDeviceContext->PSSetSamplers(0, 1, &gBillTexSampler);
	gDeviceContext->Draw(2,0);
	

	//gDeviceContext->OMSetBlendState(0, 0, 0xffffffff);

	gDeviceContext->GSSetShader(NULL, 0, 0);
	gDeviceContext->PSSetShader(NULL, 0, 0);
	gDeviceContext->VSSetShader(NULL, 0, 0);
	if (part_runParticles == 1)
	{
		World = XMMatrixIdentity();
		WorldView = World * camView;
		WVP = World * camView *  camProjection;
		cbPerObj.WorldView = XMMatrixTranspose(WorldView);
		cbPerObj.ProjMatrix = XMMatrixTranspose(camProjection);
		cbPerObj.WVP = XMMatrixTranspose(WVP);
		cbPerObj.World = XMMatrixTranspose(World);
		cbPerObj.hasTexture = 1;
		gDeviceContext->UpdateSubresource(gConstantBufferCamera, 0, NULL, &cbPerObj, 0, 0);

		partTest->UpdateParticles(mTimer.DeltaTime(), gConstantBufferCamera, part_addParticles);
	}
	

	setDepthStencilOn();
	setAlphaBlendingOff();
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Main mainObject;
	MSG msg = { 0 };
	HWND wndHandle = mainObject.InitWindow(hInstance);

	mainObject.mTimer.Reset();

	if (wndHandle)
	{
		mainObject.CreateDirect3DContext(wndHandle);

		mainObject.SetViewport();

		mainObject.CreateShaders();

		mainObject.CreateBuffers();

		mainObject.CreateStates();

		mainObject.InitDirectInput(hInstance);




		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				mainObject.mTimer.Tick();
				mainObject.FpsCounter();
				mainObject.Render(); //Rendera
				mainObject.gSwapChain->Present(0, 0); //Växla front- och back-buffer
			}
		}




		DestroyWindow(wndHandle);
	}

	return (int)msg.wParam;
}

HWND Main::InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CallWndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"DirectX 3D Projekt";	
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	mMainWndCaption = L"Direct3D Projekt";

		handle = CreateWindow(
		L"DirectX 3D Projekt",
		L"DirectX 3D Projekt",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT Main::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	//Stäng programmet med escape!!
	case WM_CHAR: //en tanget har tryckts ner
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;


		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


void Main::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Height = (float)480;
	vp.Width = (float)640;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}


HRESULT Main::CreateDirect3DContext(HWND wndHandle)
{
	//create a struct to hold inforamtion about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	//Clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;									// windowed/full-screen mode


	//create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr)){

		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();
		
		//DepthBuffer
	/*	D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthStencilDesc.Width = 640;
		depthStencilDesc.Height = 480;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;*/

		//HRESULT hr1 = gDevice->CreateTexture2D(&depthStencilDesc, NULL, &gDepthStencilBuffer);
		//HRESULT hr2 = gDevice->CreateDepthStencilView(gDepthStencilBuffer, NULL, &gDepthStencilView);

		//DepthBuffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthStencilDesc.Width = 640;
		depthStencilDesc.Height = 480;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		HRESULT hr1 = gDevice->CreateTexture2D(&depthStencilDesc, NULL, &gDepthStencilBuffer);

		// Default Depth Stencil State
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		//Depth test settings
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		//Stencil tests
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;
		//Stencil operations - Pixel Front Facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		//Stencil operations - Pixel Back Facing
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		HRESULT hr3 = gDevice->CreateDepthStencilState(&dsDesc, &gDepthStencilState);
		gDeviceContext->OMSetDepthStencilState(gDepthStencilState, 1);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		HRESULT hr2 = gDevice->CreateDepthStencilView(gDepthStencilBuffer, &descDSV, &gDepthStencilView);

		// Depth Stencil Disable
		D3D11_DEPTH_STENCIL_DESC dsDesc2;
		ZeroMemory(&dsDesc, sizeof(dsDesc2));
		//Depth test settings
		dsDesc2.DepthEnable = false;
		dsDesc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc2.DepthFunc = D3D11_COMPARISON_LESS;
		//Stencil tests
		dsDesc2.StencilEnable = true;
		dsDesc2.StencilReadMask = 0xFF;
		dsDesc2.StencilWriteMask = 0xFF;
		//Stencil operations - Pixel Front Facing
		dsDesc2.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc2.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc2.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc2.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		//Stencil operations - Pixel Back Facing
		dsDesc2.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc2.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc2.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc2.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr3 = gDevice->CreateDepthStencilState(&dsDesc2, &gDepthStencilStateDisable);


	
		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencilView);
	}
	return hr;

}

Main::Main()
{
	gSwapChain =		nullptr;
	gDevice =			nullptr;
	gDeviceContext =	nullptr;
	gBackbufferRTV =	nullptr;
	gTextureView = nullptr;
	gVertexLayout = nullptr;
	gDepthStencilView = nullptr;
	gDepthStencilBuffer = nullptr;
	gVertexBuffer = nullptr;
	//gIndexBuffer = nullptr;
	gVertexBuffer = nullptr;
	gPixelShader = nullptr;
	gGeometryShader = nullptr;
	gConstantBufferCamera = nullptr;

	billboardVS = nullptr;
	billboardGS = nullptr;
	billboardPS = nullptr;
	bbVertexBuffer = nullptr;

		RSCullCW = nullptr;
		RSCullCCW = nullptr;
		//blendTransparency = nullptr;
		RSCullNone = nullptr;
		gSampStateLin = nullptr;


	DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	camYaw = 0.0f;
	camPitch = 0.0f;
	mainName = this;

	part_runParticles = 0;
	part_addParticles = 0;
	
}




Main::~Main()
{
	 gSwapChain->Release();
	 gDevice->Release();
	 gDeviceContext->Release();
	 gBackbufferRTV->Release();
	 //gTextureView->Release();

	 gVertexLayout->Release();
	 gBillVertLayout->Release();
	 gDepthStencilView->Release();
	 gDepthStencilBuffer->Release();

	 gVertexBuffer->Release();
	 //gIndexBuffer->Release();
	 gVertexShader->Release();
	 gPixelShader->Release();
	 //gGeometryShader->Release();
	 gConstantBufferCamera->Release();
	 DIKeyboard->Release();
	 DIMouse->Release();
	 billboardVS->Release();
	 billboardGS->Release();
	 billboardPS->Release();
	 bbVertexBuffer->Release();
	 RSCullCW->Release();
	 RSCullCCW->Release();
	 //gBlendStateDefault->Release();
	 //gBlendStateAdd->Release();
	 //gBlendStateTrans->Release();
	 //gDepthStencilState->Release();
	// gDepthStencilStateDisable->Release();
	 RSCullNone->Release();
	 //gBillTexSampler->Release();


	delete sphrThingy;
	delete billBalls;

}


void Main::setDepthStencilOff()
{
	gDeviceContext->OMSetDepthStencilState(gDepthStencilStateDisable, 1);
	return;
}

void Main::setDepthStencilOn()
{
	gDeviceContext->OMSetDepthStencilState(gDepthStencilState, 1);
	return;
}

void Main::setAlphaBlendingOff()
{
	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	gDeviceContext->OMSetBlendState(gBlendStateDefault, blendFactors, 0xffffffff);
	return;
}

void Main::setAlphaBlendingOn()
{
	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	
	gDeviceContext->OMSetBlendState(gBlendStateTrans, blendFactors, 0xffffffff);
	return;
}

void Main::setAlphaBlendingAdd()
{
	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	gDeviceContext->OMSetBlendState(gBlendStateAdd, blendFactors, 0xffffffff);
	return;
}