struct Particle
{
	float3 Pos : POSITION;
	float pad1;
	float3 Vel : VELOCITY;
	float time : TIME;
};

StructuredBuffer<Particle>SimState;

struct PARTVS_IN
{
	uint vID : SV_VertexID;
};


struct BBVS_OUT
{
	float3 wCenter : POSITION;
};

BBVS_OUT PVS_main(in PARTVS_IN bbvs_input) //: SV_POSITION
{
	BBVS_OUT bbvsout;

	bbvsout.wCenter.xyz = SimState[bbvs_input.vID].Pos;

	return bbvsout;
}