cbuffer gs_cbPerObject : register(c0)
{
	float4x4 WVP;
	float4x4 World;
	float4x4 WorldView;
	float4x4 ProjMatrix;
	float4 diffuseColor;
	bool hasTexture;
};

cbuffer gs_cbInverseView : register(c1)
{
	float4x4 InvWorldView;
	float4 CameraPosition;
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 wPos : POSITION;
	float2 tex : TEXCOORD;
	float4 nor : NORMAL;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 wPos : POSITION;
	float2 tex : TEXCOORD;
	float4 nor : NORMAL;
};

[maxvertexcount(3)]
void bcGS_main(triangle VS_OUT input[3], inout TriangleStream< GS_OUT > output)
{

	
	float4 camPos = CameraPosition;
	//float4 camPos = mul(CameraPosition, InvWorldView);
	float4 faceNormal = input[0].nor;
	float4 camDir = mul(float4(input[0].wPos.xyz - camPos.xyz, 0.0f), InvWorldView);
	camDir = -normalize(camDir);

	float dotProduct = dot(camDir, faceNormal );

	//float4 transNormals = mul(input[0].nor, WorldView);


	if (dotProduct >= 0.0f)
	{
		for (uint i = 0; i < 3; i++)
		{
			GS_OUT element;
			element.pos = input[i].pos;
			element.wPos = input[i].wPos;
			element.tex = input[i].tex;
			element.nor = input[i].nor;
			output.Append(element);
		}
	}

	output.RestartStrip();
	
	/*float fx = (input[0].nor.x + input[1].nor.x + input[2].nor.x) / 3;
	float fy = (input[0].nor.y + input[1].nor.y + input[2].nor.y) / 3;
	float fz = (input[0].nor.z + input[1].nor.z + input[2].nor.z) / 3;*/

	

		//float3 edge1 = input[1].pos.xyz - input[0].pos.xyz;
		//float3 edge2 = input[2].pos.xyz - input[0].pos.xyz;

		//float3 normal = cross(edge1, edge2);
		//float4 normal = normalize(faceNormal);
	//	float4 normal = normalize(faceNormal);
	//	//normal = mul(float4(normal.xyz, 0.0f), WorldView);
	//	//normalize(normal);


	//	float4 viewPoint = mul(float4(input[0].wPos.xyz, 1.0f), WorldView);
	//		//float4 viewPoint = float4(input[0].wPos.xyz, 0.0f);
	//		//viewPoint = -viewPoint;
	//		//viewPoint = normalize(viewPoint);

	//float namn = dot(viewPoint, normal);

	//float3 edge1 = input[1].wPos.xyz - input[0].wPos.xyz;
	//	float3 edge2 = input[2].wPos.xyz - input[0].wPos.xyz;

	//	float4 normal = float4(cross(edge1, edge2), 0.0f);

	//	//normal = normalize(mul(normal, WorldView));
	//	normal = normalize(normal);
	////float4 viewPoint = mul(float4(input[0].wPos.xyz, 1.0f), WorldView);
	//float4 viewPoint = float4(input[0].wPos);
	//	//viewPoint = -viewPoint;

	//float namn = dot(viewPoint, normal);




}