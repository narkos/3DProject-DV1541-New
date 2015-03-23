struct Particle
{
	float3 Pos;
	float3 Dir;
	float time;
};
AppendStructuredBuffer<Particle>NewSimulationState : register(c0);

cbuffer ParticleParameters
{
	float4 Emitter_Loc;
	float4 RandomVector;
};

static const float3 dirConst[8] =
{
	normalize(float3(1.0f, 1.0f, 1.0f)),
	normalize(float3(-1.0f, 1.0f, 1.0f)),
	normalize(float3(-1.0f, -1.0f, 1.0f)),
	normalize(float3(1.0f, -1.0f, 1.0f)),
	normalize(float3(1.0f, 1.0f, -1.0f)),
	normalize(float3(-1.0f, 1.0f, -1.0f)),
	normalize(float3(-1.0f, -1.0f, -1.0f)),
	normalize(float3(1.0f, -1.0f, -1.0f))
};


[numthreads(8, 1, 1)]
void CSEMITT_MAIN( uint3 GTid : SV_GroupThreadID )
{
	Particle particle; 

	//Sets the new particle's position to that of the Emitter
	particle.Pos = Emitter_Loc.xyz;
	//Creates the initial direction by reflecting the GroupThreadID constant vector
	//with a random vector.
	particle.Dir = reflect(dirConst[GTid.x], RandomVector.xyz) * 5.0f; // 5.ef????? Varför?

	particle.time = 0.0f;

	NewSimulationState.Append(particle);


}