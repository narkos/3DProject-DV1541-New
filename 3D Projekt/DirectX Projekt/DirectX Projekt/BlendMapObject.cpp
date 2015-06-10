#include "BlendMapObject.h"

BlendMapObj::BlendMapObj()
{
	m_textures[0] = 0;
	m_textures[1] = 0;
	m_textures[2] = 0;
}

BlendMapObj::BlendMapObj(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2, WCHAR* filename3)
{
	o_device = device;
	LoadTextures(filename1, filename2, filename3);
	
}

BlendMapObj::~BlendMapObj()
{

}



bool BlendMapObj::LoadTextures(WCHAR* filename1, WCHAR* filename2, WCHAR* filename3)
{
	bool result;


	CoInitialize(NULL);
	ID3D11ShaderResourceView* temp_SRV = nullptr;
	result = CreateWICTextureFromFile(o_device, filename1, NULL, &m_textures[0], NULL);
	if (FAILED(result))
	{
		return false;
	}
	// Load the second texture in.
	result = CreateWICTextureFromFile(o_device, filename2, NULL, &m_textures[1], NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Load the third texture in.
	result = CreateWICTextureFromFile(o_device, filename3, NULL, &m_textures[2], NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void BlendMapObj::Render(ID3D11DeviceContext* deviceContext)
{
	


	// Set the shader parameters that it will use for rendering.
	//result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray);
}



