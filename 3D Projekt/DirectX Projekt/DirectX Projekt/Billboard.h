#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Root.h"


class Billboard
{
public:
	BBVert billBaseVert;
	//float billHeight;
	//float billWidth;
	ID3D11ShaderResourceView*	billSRV;						// mesh Shader Resource View POINTER
	wstring	billTexture;
	bool isBill;
	bool hasTexture;

	//ID3D11Buffer* billIndexBuffer;
	//ID3D11Buffer* billVertexBuffer;


	Billboard();
	~Billboard();
	Billboard(ID3D11Device* device, ID3D11DeviceContext* devContext, XMFLOAT3 vertPos, float height, float width, wstring bbtexFilename);

};




#endif