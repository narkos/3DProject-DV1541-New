cbuffer gs_cbPerObject
{
	float4x4 WVP;
	float4x4 World;
	float4x4 WorldView;
	float4x4 ProjMatrix;
	float4 diffuseColor;
	bool hasTexture;
};


cbuffer ParticleRenderParameters
{
	float4 EmitterLocation;
	float4 ConsumerLocation;
};

static const float4 cornerPositions[4] = {
	float4(-1, 1, 0, 0), float4(1, 1, 0, 0), float4(-1, -1, 0, 0), float4(1, -1, 0, 0)
};

static const float2 gsTex[4] =
{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
};

static const float SizeModifier = 0.5f;

struct BBVS_OUT
{
	float3 wCenter : POSITION;
};

struct BBGS_OUT
{
	float4 posH: SV_POSITION;
	float2 tex: TEXCOORD;
	float4 color : Color;
};



[maxvertexcount(4)]
void partGSMAIN(point BBVS_OUT input[1], inout TriangleStream<BBGS_OUT> gsOutData)
{
	BBGS_OUT output;


	//Finds distance between particle and the consumer. This is used to set a different base color
	//in the pixel shader
	float dist = saturate(length(input[0].wCenter - ConsumerLocation.xyz));

	float4 color = float4(1.0, 0.1f, 0.0f, 0.0f)*dist + float4(0.0f, 0.0f, 0.2f, 0.1f)*(1.0f - dist);
		

	float4 viewPosition = mul(float4(input[0].wCenter, 1.0f), WorldView);

	for (int i = 0; i < 4; i++)
	{
		output.posH = mul(viewPosition + cornerPositions[i]*SizeModifier, ProjMatrix);
		output.tex = gsTex[i];
		output.color = color;
		gsOutData.Append(output);
	}

	gsOutData.RestartStrip();
}


