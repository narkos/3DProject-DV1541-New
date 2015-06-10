#include "ObjImport.h"
//#include "TextureArrayClass.h"

class BlendMapObj
{
public:
	BlendMapObj();
	BlendMapObj(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2, WCHAR* filename3);
	~BlendMapObj();

	bool LoadTextures(WCHAR*, WCHAR*, WCHAR*);
	void Render(ID3D11DeviceContext* deviceContext);
	void RenderBuffers(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* m_textures[3];
	ID3D11Device* o_device;
	ID3D11PixelShader * gAlphaMapPixel = nullptr;

	
	//bool BlendMapObj::Render(ID3D11DeviceContext* deviceContext, int indexCount; XMMATRIX worldMatrix,
	//XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray);
	//ObjImportObject->o_meshVertBuff

	/*blendmap
	{
		sphrThingy = new ObjImport(filename, gDevice, true, true);


	}*/

	
};