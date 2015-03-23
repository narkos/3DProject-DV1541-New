#include "Billboard.h"

Billboard::Billboard()
{
	billBaseVert;
	//billHeight = 0.0f;
	//billWidth = 0.0f;
	billSRV = nullptr;
	billTexture = nullptr;
}


Billboard::~Billboard()
{
	billSRV->Release();
}

Billboard::Billboard(ID3D11Device* device, ID3D11DeviceContext* devContext, XMFLOAT3 vertPos, float height, float width, wstring bbtexFilename)
{
	this->billBaseVert.Pos = vertPos;
	//this->billBaseVert.pos.y = vertPos.y;
	//this->billBaseVert.pos.z = vertPos.z;

	this->billBaseVert.Size = XMFLOAT2(width, height);
	//this->billWidth = width;

	this->billTexture = bbtexFilename;
	this->billSRV = nullptr;
	CoInitialize(NULL);
	ID3D11ShaderResourceView* tempSRV =nullptr;
	HRESULT hr = CreateWICTextureFromFile(device, bbtexFilename.c_str(), nullptr, &tempSRV);
	if (SUCCEEDED(hr))
	{
		this->billSRV = tempSRV;
	}

	this->isBill = true;
	this->hasTexture = true;
}