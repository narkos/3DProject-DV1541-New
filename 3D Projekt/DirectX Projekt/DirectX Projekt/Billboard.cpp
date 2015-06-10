#include "Billboard.h"

Billboard::Billboard()
{
	billBaseVert.Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	billBaseVert.Dir = XMFLOAT3(0.0f, 0.0f, 0.0f);
	billBaseVert.Size = XMFLOAT2(0.0f, 0.0f);
	billBaseVert.time = 0.0f;

	billSRV = nullptr;
	//billTexture = nullptr;
	hasTexture = false;
	isBill = true;
	
}

Billboard::Billboard(XMFLOAT3 vertPos, XMFLOAT3 vertDir, float height, float width)
{
	this->billBaseVert.Pos = vertPos;
	this->billBaseVert.Dir = vertDir;
	billBaseVert.Size = XMFLOAT2(width, height);
	billBaseVert.time = 0.0f;

	billSRV = nullptr;
	billTexture = nullptr;
	hasTexture = false;
	isBill = true;

}

void Billboard::setTexture(ID3D11Device* device, wstring filename)
{
	CoInitialize(NULL);
	ID3D11ShaderResourceView* tempSRV = nullptr;
	HRESULT hr = CreateWICTextureFromFile(device, filename.c_str(), nullptr, &tempSRV);
	if (SUCCEEDED(hr))
	{
		this->billSRV = tempSRV;
		this->billTexture = filename;
	}
}

Billboard::~Billboard()
{
	if (billSRV)
		billSRV->Release();
}

Billboard::Billboard(ID3D11Device* device, ID3D11DeviceContext* devContext, XMFLOAT3 vertPos, float height, float width, wstring bbtexFilename)
{
	this->billBaseVert.Pos = vertPos;
	//this->billBaseVert.pos.y = vertPos.y;
	//this->billBaseVert.pos.z = vertPos.z;

	this->billBaseVert.Size = XMFLOAT2(width, height);
	//this->billWidth = width;
	this->billBaseVert.Dir = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->billBaseVert.time = 0.0f;
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