#ifndef BBPARTICLES_H
#define BBPARTICLES_H

#include "Billboard.h"


class ParticleSystem
{
	struct EmittCB
	{
		XMFLOAT4 EmitterLocation;
		XMFLOAT4 RandomVector;
	};
	struct UpdateCB
	{
		XMFLOAT4 TimeFactors;
		XMFLOAT4 EmitterLocation;
		XMFLOAT4 OmitterLocation;
	};
	struct UpdateGS
	{
		XMFLOAT4 EmitterLocation;
		XMFLOAT4 OmitterLocation;
	};

	struct Particle
	{
		XMFLOAT3 Position;
		float pad1;
		XMFLOAT3 Velocity;
		float time;
	};

private:
	ID3D11UnorderedAccessView* CreateNullUAV(void);
	ID3D11ShaderResourceView* CreateNullSRV(void);
	void ResetShaderSettings(void);
	void SetUpdateSettings(void);

public:
	ParticleSystem();
	ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11SamplerState* sampstate,  XMFLOAT4 emittL, XMFLOAT4 omittL, wstring texFile , int initBuffSize);
	~ParticleSystem();

	void UpdateParticles(float time, ID3D11Buffer* wConstBuffer, int addParticles);
	//void UpdateParticles(float time, ID3D11VertexShader* vshader, ID3D11GeometryShader* gshader, ID3D11PixelShader* pshader, ID3D11InputLayout* layout, ID3D11Buffer* wConstBuffer);
	void ExecuteParticles();

	void SetRendParameters();
	void setUsageParameters();
	void InitShaders();

	ID3D11Device* gDev;
	ID3D11DeviceContext* gDevCon;

	ID3D11UnorderedAccessView* part_UAVs[2];
	ID3D11ShaderResourceView* part_SRVs[2];
	ID3D11Buffer* part_CountConstBuffer;
	ID3D11Buffer* part_CountIndirAccessBuffer;
	ID3D11Buffer* part_CountStagingBuffer;
	int part_ParticleCount;
	UINT* part_uintCount;
	unsigned int part_BufferIndex;

#define BUFFER_SIZE 8
#define RAND_MAX 0x7fff

	// Constant buffers
	EmittCB part_cbEmittObj;
	ID3D11Buffer*  part_cbEmitt; //Constant Buffer - EmitterLoc 
												//	 RandomVector
	UpdateCB part_cbUpdateObj;
	ID3D11Buffer* part_cbUpdate; //Constant Buffer - TimeFactors
												//	 EmitterLoc
												//	 OmitterLoc
	UpdateGS part_cbUpdateGSobj;
	ID3D11Buffer* part_cbUpdateGS;


	int part_addParticles;

	float part_tDelta;
	float part_updateTime;
	float part_throttle;
	bool part_firstTimeInit;	//Test whether this is the first time the system has been
							// initialized

	XMFLOAT4 partEmitterLoc;		//EmitterLocation
	XMFLOAT4 partOmitterLoc;		//OmitterLocation

	wstring texFileName;
	ID3D11ShaderResourceView*	particleSRV;
	ID3D11Buffer* worldMTXConstBuffer; //Matrix Constantbuffer from main
	ID3D11BlendState* blendStateAdd;
	ID3D11SamplerState* samplerLinear;
	ID3D11InputLayout* vLayout;
	ID3D11VertexShader* vShaderPointer;
	ID3D11GeometryShader* gShaderPointer;
	ID3D11PixelShader* pShaderPointer;
	ID3D11ComputeShader* CSparticleEmitt; //Pointer to CShader for Creating Particles
	ID3D11ComputeShader* CSparticleUpdate; //Pointer to CShader for particle updating


	/*ID3D11ComputeShader* cshader_InitParticles;
	ID3D11ComputeShader* cshader_UpdateParticles;*/

};



#endif