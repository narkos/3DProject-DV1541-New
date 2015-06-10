//cbuffer cbPerObject : register(c0)
//{
//	float4x4 WVP;
//	float4x4 World;
//	float4 diffuseColor;
//	bool hasTexture;
//};
//cbuffer cbPerFrame:register(c5)
//{
//	float3 camPosition;
//	float pad1;
//};
struct BBVS_IN
{
	float3 Pos : POSITION;
	float3 Dir : DIRECTION;
	float2 Size : SIZE;
	float time : TIME;
};


struct BBVS_OUT
{
	float3 wCenter : POSITION;
	float3 wDir	: DIRECTION;
	float2 wSize : SIZE;
	float time : TIME;
};

BBVS_OUT BBVS_main( BBVS_IN bbvs_input ) //: SV_POSITION
{
	BBVS_OUT bbvsout = (BBVS_OUT)0;
	
	bbvsout.wCenter = bbvs_input.Pos;
	bbvsout.wDir = bbvs_input.Dir;
	bbvsout.wSize = bbvs_input.Size;
	bbvsout.time = bbvs_input.time;
	return bbvsout;
}