
cbuffer cbPerObject : register(c0)
{
	float4x4 WVP;
	float4x4 World;
	float4x4 WorldView;
	float4x4 ProjMatrix;
	float4 diffuseColor;
	bool hasTexture;
};

struct VS_IN
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 nor  : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 wPos : POSITION;
	float2 tex : TEXCOORD;
	float4 nor  : NORMAL;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	//float4 position = float4(input.pos,1);

	output.pos = mul(float4(input.pos, 1.0f), WVP);

	//output.wPos = mul(float4(input.pos, 1), World);
	output.wPos =float4(input.pos, 1.0f);
	output.nor = float4(input.nor, 0.0f);
	output.tex = input.tex;

	return output;
}




//struct VS_IN
//{
//	float3 pos : POSITION;
//	float3 tex : TEXCOORD;
//	float4 nor  : NORMAL;
//};
//
//struct VS_OUT
//{
//	float4 pos : SV_POSITION;
//	float3 tex : TEXCOORD;
//	float4 nor  : NORMAL;
//};
//
//VS_OUT VS_main(VS_IN input)
//{
//	VS_OUT output = (VS_OUT)0;
//
//	output.pos = float4(input.pos, 1);
//	//output.nor = float4(input.nor, 0);
//	output.tex = input.tex;
//
//	return output;
//}
