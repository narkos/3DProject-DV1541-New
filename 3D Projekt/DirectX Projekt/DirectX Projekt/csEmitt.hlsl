struct Particle
{
	float3 Position;
	float pad1;
	float3 Velocity;
	float time;
};

AppendStructuredBuffer<Particle>NewSimulationState : register(u0);

cbuffer ParticleParameters
{
	float4 EmitterLocation;
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
void CSEMITT_MAIN(uint3 GTid : SV_GroupThreadID)
{
	Particle particle; 

	//Sets the new particle's position to that of the Emitter
	particle.Position = EmitterLocation.xyz;
	//Creates the initial direction by reflecting the GroupThreadID constant vector
	//with a random vector.
	particle.Velocity = reflect(dirConst[GTid.x], normalize(RandomVector.xyz)) * 5.0f; // 5.ef????? Varför?
	particle.time = 0.0f;
	particle.pad1 = 0.0f;
	NewSimulationState.Append(particle);


}


//[numthreads(1, 1, 1)]
//void main( uint3 DTid : SV_DispatchThreadID )
//{
//
//}