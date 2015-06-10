Texture2D PartTex : register (t0);
SamplerState LinearSampler : register(s0);


struct BBGS_OUT
{
	float4 posH: SV_POSITION;
	float2 tex: TEXCOORD;
	float4 color : Color;
};



float4 partPSMAIN(in BBGS_OUT input) : SV_Target
{
	float4 color = PartTex.Sample(LinearSampler, input.tex);
	
	//color = color * input.color;

	return(color);
}