#include "bbParticles.h"


ParticleSystem::ParticleSystem()
{
	vShaderPointer = nullptr;
	gShaderPointer = nullptr;
	pShaderPointer = nullptr;
	vLayout = nullptr;
	worldMTXConstBuffer = nullptr;
}
ParticleSystem::~ParticleSystem()
{

}

ParticleSystem::ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11SamplerState* sampstate, XMFLOAT4 emittL, XMFLOAT4 omittL, wstring texFile, int initBuffSize)
{
	part_addParticles = 0;
	part_firstTimeInit = true;
	gDev = device;
	gDevCon = deviceContext;
	partEmitterLoc = emittL;
	partOmitterLoc = omittL;


	// Used for injecting new particles and update/render render them later.
	part_throttle = 8.0f*30.0f / static_cast<float>(initBuffSize);
	part_tDelta = part_throttle;

	//Allows dynamic usage of thread calls and buffer sizes according to the amount of particles created.
	part_ParticleCount = initBuffSize;


	//Initialize initial particle data

	Particle* particleData = new Particle[part_ParticleCount];
	for (int i = 0; i < part_ParticleCount; i++)
	{
		particleData[i].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particleData[i].Velocity = XMFLOAT3(0.0f, 0.0f, 1.0f);
		//particleData[i].Size = XMFLOAT2(1.0f, 1.0f);
		particleData[i].time = 0.0f;
	}

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = particleData;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	D3D11_SUBRESOURCE_DATA initialData2;
	initialData2.pSysMem = particleData;
	initialData2.SysMemPitch = 0;
	initialData2.SysMemSlicePitch = 0;


	//Create two buffers for current and next simulation states. 
	D3D11_BUFFER_DESC partStructBuffDesc;
	ZeroMemory(&partStructBuffDesc, sizeof(partStructBuffDesc));
	partStructBuffDesc.ByteWidth = part_ParticleCount * sizeof(Particle);
	partStructBuffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	partStructBuffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	partStructBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	partStructBuffDesc.CPUAccessFlags = 0;
	partStructBuffDesc.StructureByteStride = sizeof(Particle);

	ID3D11Buffer* strBuffer1 = nullptr;

	D3D11_BUFFER_DESC partStructBuffDesc2;
	ZeroMemory(&partStructBuffDesc2, sizeof(partStructBuffDesc2));
	partStructBuffDesc2.ByteWidth = part_ParticleCount * sizeof(Particle);
	partStructBuffDesc2.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	partStructBuffDesc2.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	partStructBuffDesc2.Usage = D3D11_USAGE_DEFAULT;
	partStructBuffDesc2.CPUAccessFlags = 0;
	partStructBuffDesc2.StructureByteStride = sizeof(Particle);


	ID3D11Buffer* strBuffer2 = nullptr;

	HRESULT hr = device->CreateBuffer(&partStructBuffDesc, &initialData, &strBuffer1);
	hr = device->CreateBuffer(&partStructBuffDesc2, &initialData2, &strBuffer2);

	//delete[] particleData;

	//Create Unordered Structure View and Shader Resource Views
	//We need two of each because the Compute Shader needs to be able to switch the data between updates

	//Create the first set.

	D3D11_BUFFER_UAV uav;
	ZeroMemory(&uav, sizeof(uav));
	uav.FirstElement = 0;
	uav.NumElements = part_ParticleCount;
	uav.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	D3D11_UNORDERED_ACCESS_VIEW_DESC firstUAV;
	ZeroMemory(&firstUAV, sizeof(firstUAV));
	firstUAV.Format = DXGI_FORMAT_UNKNOWN;
	firstUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	firstUAV.Buffer = uav;

	D3D11_BUFFER_SRV srv;	//Shader Resource View buffer, all other values are default
	ZeroMemory(&srv, sizeof(srv));
	srv.FirstElement = 0;
	srv.NumElements = part_ParticleCount;

	D3D11_SHADER_RESOURCE_VIEW_DESC	 firstSRV;
	ZeroMemory(&firstSRV, sizeof(firstSRV));
	firstSRV.Format = DXGI_FORMAT_UNKNOWN;
	firstSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	firstSRV.Buffer = srv;

	hr = device->CreateUnorderedAccessView(strBuffer1, &firstUAV, &part_UAVs[0]);
	hr = device->CreateShaderResourceView(strBuffer1, &firstSRV, &part_SRVs[0]);

	ZeroMemory(&uav, sizeof(uav));
	ZeroMemory(&firstUAV, sizeof(firstUAV));
	ZeroMemory(&srv, sizeof(srv));
	ZeroMemory(&firstSRV, sizeof(firstSRV));

	uav.FirstElement = 0;
	uav.NumElements = part_ParticleCount;
	uav.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	firstUAV.Format = DXGI_FORMAT_UNKNOWN;
	firstUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	firstUAV.Buffer = uav;

	srv.FirstElement = 0;
	srv.NumElements = part_ParticleCount;

	firstSRV.Format = DXGI_FORMAT_UNKNOWN;
	firstSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	firstSRV.Buffer = srv;

	hr = device->CreateUnorderedAccessView(strBuffer2, &firstUAV, &part_UAVs[1]);
	hr = device->CreateShaderResourceView(strBuffer2, &firstSRV, &part_SRVs[1]);

	// Create buffer for constant buffer to keep track of the particle count.
	// Initializes at sero and is populated CopyStructureCount later

	UINT* partInitCB = new UINT[8];
	for (int i = 0; i < 8; i++)
	{
		partInitCB[i] = 0;
	}
	partInitCB[0] = 10;

	D3D11_SUBRESOURCE_DATA initCBdata;
	initCBdata.pSysMem = partInitCB;
	initCBdata.SysMemPitch = 0;
	initCBdata.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC initCBdesc;
	memset(&initCBdesc, 0, sizeof(initCBdesc));
	initCBdesc.ByteWidth = (4 * sizeof(UINT));
	initCBdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	initCBdesc.MiscFlags = 0;
	initCBdesc.StructureByteStride = 0;
	initCBdesc.Usage = D3D11_USAGE_DEFAULT;
	initCBdesc.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&initCBdesc, &initCBdata, &part_CountConstBuffer);

	// Create Indirect Arguments Buffer
	// All except index 1 is set to 0 since the DrawInstancedIndirect() method needs one to initiate

	partInitCB[0] = 0;
	partInitCB[1] = 1;
	partInitCB[2] = 0;
	partInitCB[3] = 0;
	initCBdata.pSysMem = partInitCB;
	initCBdata.SysMemPitch = 0;
	initCBdata.SysMemSlicePitch = 0;

	initCBdesc.ByteWidth = (4 * sizeof(UINT));
	initCBdesc.BindFlags = 0;
	initCBdesc.CPUAccessFlags = 0;
	initCBdesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	initCBdesc.Usage = D3D11_USAGE_DEFAULT;
	initCBdesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&initCBdesc, &initCBdata, &part_CountIndirAccessBuffer);

	// Create staging buffer to read from GPU to CPU

	for (int i = 0; i < 8; i++)
	{
		partInitCB[i] = 0;
	}
	
	

	D3D11_BUFFER_DESC stagingBufferDesc;
	stagingBufferDesc.ByteWidth = (BUFFER_SIZE*sizeof(UINT));
	stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
	stagingBufferDesc.BindFlags = 0;
	stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingBufferDesc.MiscFlags = 0;
	stagingBufferDesc.StructureByteStride = 0;
	
	hr = device->CreateBuffer(&stagingBufferDesc, &initCBdata, &part_CountStagingBuffer);
	
	part_BufferIndex = 0;

	// Create the other Constant Buffers
	D3D11_BUFFER_DESC emittCBDesc;
	ZeroMemory(&emittCBDesc, sizeof(emittCBDesc));
	emittCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	emittCBDesc.Usage = D3D11_USAGE_DEFAULT;
	emittCBDesc.ByteWidth = sizeof(EmittCB);
	gDev->CreateBuffer(&emittCBDesc, nullptr, &part_cbEmitt);

	D3D11_BUFFER_DESC updateCBDesc;
	ZeroMemory(&updateCBDesc, sizeof(updateCBDesc));
	updateCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	updateCBDesc.Usage = D3D11_USAGE_DEFAULT;
	updateCBDesc.ByteWidth = sizeof(UpdateCB);
	gDev->CreateBuffer(&updateCBDesc, nullptr, &part_cbUpdate);


	D3D11_BUFFER_DESC gsCBDesc;
	ZeroMemory(&gsCBDesc, sizeof(gsCBDesc));
	gsCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gsCBDesc.Usage = D3D11_USAGE_DEFAULT;
	gsCBDesc.ByteWidth = sizeof(UpdateGS);
	gDev->CreateBuffer(&gsCBDesc, nullptr, &part_cbUpdateGS);

	texFileName = texFile;
	CoInitialize(NULL);
	ID3D11ShaderResourceView* tempSRV = nullptr;
	hr = CreateWICTextureFromFile(gDev, texFileName.c_str(), nullptr, &tempSRV);
	if (SUCCEEDED(hr))
	{
		particleSRV = tempSRV;
	}
	// Create Blend State and Texture Sampler
	D3D11_BLEND_DESC addBlendDesc;
	memset(&addBlendDesc, 0, sizeof(addBlendDesc));
	/*addBlendDesc.RenderTarget[0].BlendEnable = true;
	addBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	addBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	addBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/
	addBlendDesc.RenderTarget[0].BlendEnable = true;
	addBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	addBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	addBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	addBlendDesc.AlphaToCoverageEnable = 0x0f;
	addBlendDesc.IndependentBlendEnable = false;

	hr = gDev->CreateBlendState(&addBlendDesc, &blendStateAdd);

	samplerLinear = sampstate;

	
};


void ParticleSystem::InitShaders()
{
	//Particle System Compute Shader - Emitt
	ID3DBlob* pCSInit = nullptr;
	RootName::CompileShader(L"csEmitt.hlsl", "CSEMITT_MAIN", "cs_5_0", &pCSInit);
	gDev->CreateComputeShader(pCSInit->GetBufferPointer(), pCSInit->GetBufferSize(), nullptr, &CSparticleEmitt);
	pCSInit->Release();
	//Particle System Compute Shader - Update
	ID3DBlob* pCSUpd = nullptr;
	RootName::CompileShader(L"csUpdate.hlsl", "CSMAINUPD", "cs_5_0", &pCSUpd);
	gDev->CreateComputeShader(pCSUpd->GetBufferPointer(), pCSUpd->GetBufferSize(), nullptr, &CSparticleUpdate);
	pCSUpd->Release();
	//Particle System Vertex Shader
	ID3DBlob* ppVS = nullptr;
	RootName::CompileShader(L"partVertexShader.hlsl", "PVS_main", "vs_5_0", &ppVS);
	gDev->CreateVertexShader(ppVS->GetBufferPointer(), ppVS->GetBufferSize(), nullptr, &vShaderPointer);

	//ID3DBlob* pVL = nullptr;
	/*D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};*/

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "SV_VERTEXID", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	gDev->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), ppVS->GetBufferPointer(), ppVS->GetBufferSize(), &vLayout);
	
	ppVS->Release();
	//Particle System Geometry Shader
	ID3DBlob* ppGS = nullptr;
	RootName::CompileShader(L"partGeometryShader.hlsl", "partGSMAIN", "gs_5_0", &ppGS);
	gDev->CreateGeometryShader(ppGS->GetBufferPointer(), ppGS->GetBufferSize(), nullptr, &gShaderPointer);
	ppGS->Release();
	//Particle System Pixel Shader
	ID3DBlob* ppPS = nullptr;
	RootName::CompileShader(L"partPixelShader.hlsl", "partPSMAIN", "ps_5_0", &ppPS);
	gDev->CreatePixelShader(ppPS->GetBufferPointer(), ppPS->GetBufferSize(), nullptr, &pShaderPointer);
	ppPS->Release();



	//gShaderPointer = gshader;
	//pShaderPointer = pshader;
	//Particle System Input Layout


	//vLayout = vlay;
}
ID3D11UnorderedAccessView* ParticleSystem::CreateNullUAV(void)
{
	ID3D11UnorderedAccessView* tempNullUAV[1] = { NULL };
	return *tempNullUAV;
}

ID3D11ShaderResourceView* ParticleSystem::CreateNullSRV(void)
{
	ID3D11ShaderResourceView* tempNullSRV[1] = { NULL };
	return *tempNullSRV;
}

void ParticleSystem::ResetShaderSettings(void)
{
	ID3D11UnorderedAccessView* tempNullUAV[2] = { NULL, NULL };
	ID3D11ShaderResourceView* tempNullSRV[2] = { NULL, NULL };
	gDevCon->CSSetUnorderedAccessViews(0, 2, tempNullUAV, nullptr);

	gDevCon->CSSetUnorderedAccessViews(0, 2, tempNullUAV, nullptr);
	gDevCon->CSSetShader(nullptr, nullptr, 0);
	gDevCon->VSSetShader(nullptr, nullptr, 0);
	gDevCon->PSSetShader(nullptr, nullptr, 0);
	gDevCon->GSSetShader(nullptr, nullptr, 0);

	//gDevCon->CSSetShader
}

void ParticleSystem::SetUpdateSettings(void)
{
	gDevCon->CSSetShader(CSparticleUpdate, nullptr, 0);
	gDevCon->CSSetConstantBuffers(0, 1, &part_cbUpdate);
	gDevCon->CSSetConstantBuffers(1, 1, &part_CountConstBuffer);
}

void ParticleSystem::SetRendParameters()
{
	const UINT nullcounts[] = { 0 };
	if (part_firstTimeInit == true)
	{ 
		const UINT startcounts[] = { 10 };
		gDevCon->CSSetUnorderedAccessViews(0, 1, &part_UAVs[0], nullcounts);
		gDevCon->CSSetUnorderedAccessViews(1, 1, &part_UAVs[1], startcounts);
		part_firstTimeInit = false;
	}

	
	else
	{
		const UINT incounts[] = {-1};
		
		gDevCon->CSSetUnorderedAccessViews(0, 1, &part_UAVs[0], incounts);
		gDevCon->CSSetUnorderedAccessViews(1, 1, &part_UAVs[1], incounts);
		/*gDevCon->CSSetUnorderedAccessViews(0, 1, &part_UAVs[0], ((UINT*)part_UAVs[0]));
		gDevCon->CSSetUnorderedAccessViews(1, 1, &part_UAVs[1], ((UINT*)part_UAVs[1]));*/
	}
	static const float scale = 2.0f;
	float rX = ((float)rand() / float(RAND_MAX) * scale - scale/2.0f);
	float rY = ((float)rand() / float(RAND_MAX) * scale - scale / 2.0f);
	float rZ = ((float)rand() / float(RAND_MAX) * scale - scale / 2.0f);

	//XMVECTOR tempRand;
	//XMStore

	part_cbEmittObj.EmitterLocation = partEmitterLoc;
	part_cbEmittObj.RandomVector = XMFLOAT4(rX, rY, rZ, 0.0f);
	gDevCon->UpdateSubresource(part_cbEmitt, 0, NULL, &part_cbEmittObj, 0, 0);

	part_cbUpdateObj.EmitterLocation = partEmitterLoc;
	part_cbUpdateObj.OmitterLocation = partOmitterLoc;
	part_cbUpdateObj.TimeFactors = XMFLOAT4(static_cast<float>(part_updateTime), 0.0f, 0.0f, 0.0f);
	gDevCon->UpdateSubresource(part_cbUpdate, 0, NULL, &part_cbUpdateObj, 0, 0);

	part_cbUpdateGSobj.EmitterLocation = partEmitterLoc;
	part_cbUpdateGSobj.OmitterLocation = partOmitterLoc;
	gDevCon->UpdateSubresource(part_cbUpdateGS, 0, nullptr, &part_cbUpdateGSobj, 0, 0);
}


void ParticleSystem::ExecuteParticles()
{

	//Initialize if this is the first time the simulation is running
	///////////////////////////////////////////////////////////////////////

	if (part_firstTimeInit == true)
	{
		SetRendParameters();
		gDevCon->CSSetShader(CSparticleUpdate, nullptr, 0);
		gDevCon->CSSetConstantBuffers(0, 1, &part_cbUpdate);
		gDevCon->CSSetConstantBuffers(1, 1, &part_CountConstBuffer);
		gDevCon->Dispatch(1, 1, 1);

		gDevCon->CSSetShader(nullptr, nullptr, 0);
	}


	SetRendParameters();

	//If applicable, add more particles to the simulation using the csEmitt shader
	///////////////////////////////////////////////////////////////////////

	/*if (part_tDelta > part_throttle)*/
	if (part_addParticles == 1)
	{
		part_tDelta = 0.0f;
		gDevCon->CSSetShader(CSparticleEmitt, nullptr, 0);
		gDevCon->CSSetConstantBuffers(0, 1, &part_cbEmitt);
		gDevCon->Dispatch(1, 1, 1);

		//gDevCon->CSSetConstantBuffers(0, 1, nullptr);
		gDevCon->CSSetShader(nullptr, nullptr, 0);
	}
	ResetShaderSettings();
	gDevCon->CopyStructureCount(part_CountConstBuffer, 0, part_UAVs[1]);
	
	
	//Update Simulation
	///////////////////////////////////////////////////////////////////////
	gDevCon->CSSetShader(CSparticleUpdate, nullptr, 0);
	gDevCon->CSSetConstantBuffers(0, 1, &part_cbUpdate);
	gDevCon->CSSetConstantBuffers(1, 1, &part_CountConstBuffer);
	SetRendParameters();
	gDevCon->Dispatch(part_ParticleCount / 512, 1, 1);
	//gDevCon->DispatchIndirect(part_CountConstBuffer, 0);
	//Nullify shader and resource settings
	///////////////////////////////////////////////////////////////////////
	
	ResetShaderSettings();
	gDevCon->CopyStructureCount(part_CountIndirAccessBuffer, 0, part_UAVs[0]);

	//Set up and send information to rendering pipeline
	///////////////////////////////////////////////////////////////////////
	void* nulls[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	gDevCon->IASetVertexBuffers(0, 0, (ID3D11Buffer**)nulls, (UINT*)nulls, (UINT*)nulls);
	gDevCon->IASetInputLayout(vLayout);
	gDevCon->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	gDevCon->VSSetShader(vShaderPointer, nullptr, 0);
	gDevCon->GSSetShader(gShaderPointer, nullptr, 0);
	gDevCon->PSSetShader(pShaderPointer, nullptr, 0);
	
	gDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gDevCon->PSSetShaderResources(0, 1, &particleSRV);
	gDevCon->PSSetSamplers(0, 1, &samplerLinear);
	

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	gDevCon->OMSetBlendState(blendStateAdd, blendFactor, 0xffffffff);

	gDevCon->VSSetShaderResources(0, 1, &part_SRVs[1]);
	gDevCon->GSSetConstantBuffers(0, 1, &worldMTXConstBuffer);
	gDevCon->GSSetConstantBuffers(1, 1, &part_cbUpdateGS);

	gDevCon->DrawInstancedIndirect(part_CountIndirAccessBuffer, 0);
	

	//Reset Render settings
	gDevCon->VSSetShader(nullptr, nullptr, 0);
	gDevCon->GSSetShader(nullptr, nullptr, 0);
	gDevCon->PSSetShader(nullptr, nullptr, 0);
	
	gDevCon->PSSetShaderResources(0, 0, nullptr);
	gDevCon->VSSetShaderResources(0, 0, nullptr);

}



void ParticleSystem::UpdateParticles(float time, ID3D11Buffer* wConstBuffer, int addParticles)
{
	part_addParticles = addParticles;
	//Swap Buffers and update Time
	part_tDelta += time;
	part_updateTime = time;

	ID3D11UnorderedAccessView*  tempUAV = nullptr;
	ID3D11ShaderResourceView* tempSRV = nullptr;

	tempUAV = part_UAVs[0];
	tempSRV = part_SRVs[0];

	part_UAVs[0] = part_UAVs[1];
	part_SRVs[0] = part_SRVs[1];

	part_UAVs[1] = tempUAV;
	part_SRVs[1] = tempSRV;
	worldMTXConstBuffer = wConstBuffer;

	ExecuteParticles();
}
