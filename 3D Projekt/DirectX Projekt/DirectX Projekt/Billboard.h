#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Root.h"


class Billboard
{
public:
	BBVert billBaseVert;					//Pos, Dir, Size;
	wstring	billTexture;					// Texture Name/Location
	ID3D11ShaderResourceView*	billSRV;	// mesh Shader Resource View POINTER
	bool isBill;							// To tell shaders if it is a billboard
											// likely to be removed as it's application
											// might not be relevant.
	bool hasTexture;						//Tell shaders if the object has a texture;

	Billboard();
	~Billboard();
	Billboard(ID3D11Device* device, ID3D11DeviceContext* devContext, XMFLOAT3 vertPos, float height, float width, wstring bbtexFilename);
	Billboard(XMFLOAT3 vertPos, XMFLOAT3 vertDir, float height, float width);

	void setTexture(ID3D11Device* device, wstring filename);

};




#endif