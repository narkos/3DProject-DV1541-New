cbuffer gs_cbPerObject
{
	float4x4 WVP;
	float4x4 World;
	float4x4 WorldView;
	float4x4 ProjMatrix;
	float4 diffuseColor;
	bool hasTexture;
};

cbuffer gs_cbPerFrame
{
	vector camPosition;
};

static const float4 g_positions[4] =
{
	float4(-1, 1, 0, 0), float4(1, 1, 0, 0), float4(-1, -1, 0, 0), float4(1, -1, 0, 0),
};
static const float2 gsTex[4] =
{
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f)

	/*float2(0.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 1.0f),
	float2(1.0f, 0.0f)*/
};
struct BBVS_OUT
{
	float3 wCenter : POSITION;
	float2 wSize : SIZE;
};

struct BBGS_OUT
{
	float4 posH: SV_POSITION;
	float3 posW: POSITION;
//	float3 norW: NORMAL;
	float2 tex: TEXCOORD;
};

[maxvertexcount(4)]
void BBGS_main(point BBVS_OUT bbgs_in[1]/* : SV_POSITION*/, inout TriangleStream< BBGS_OUT > bbgs_out
)
{

	BBGS_OUT poutput;
	float4 viewPos = mul(float4(bbgs_in[0].wCenter, 1.0f), WorldView);


	float halfWidth = 0.5*bbgs_in[0].wSize.x;
	float halfHeight = 0.5*bbgs_in[0].wSize.y;




		for (int i = 0; i < 4; i++)
		{
			poutput.posH = mul(viewPos + float4(
												g_positions[i].x * halfWidth,
												g_positions[i].y * halfHeight, 0.0f, 0.0f), 
												ProjMatrix);

			poutput.posW = viewPos + float4(	g_positions[i].x * halfWidth,
												g_positions[i].y * halfHeight, 0.0f, 0.0f);
			poutput.tex = gsTex[i];

			bbgs_out.Append(poutput);
		}
		bbgs_out.RestartStrip();


	/*float3 upVec = (0.0f, 1.0f, 0.0f);
	float3 lookVec = camPosition - bbgs_in[0].wCenter;
	lookVec.y = 0.0f;
	lookVec = normalize(lookVec);
	float3 rightVec = cross(upVec, lookVec);


	float halfWidth = 0.5*bbgs_in[0].wSize.x;
	float halfHeight = 0.5*bbgs_in[0].wSize.y;

	float4 newVert[4];

	newVert[0] = float4(bbgs_in[0].wCenter + halfWidth * rightVec - halfHeight*upVec, 1.0f);
	newVert[1] = float4(bbgs_in[0].wCenter + halfWidth * rightVec + halfHeight*upVec, 1.0f);
	newVert[2] = float4(bbgs_in[0].wCenter - halfWidth * rightVec - halfHeight*upVec, 1.0f);
	newVert[3] = float4(bbgs_in[0].wCenter - halfWidth * rightVec + halfHeight*upVec, 1.0f);

	float2 gsTex[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};


	BBGS_OUT gsOut;
	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		gsOut.posH = mul(newVert[i], WVP);
		gsOut.posW = newVert[i].xyz;
		gsOut.norW = lookVec;
		gsOut.tex = gsTex[i];
		bbgs_out.Append(gsOut);
	}*/
}