struct Particle
{
	float3 Pos;
	float3 Vel;
	float time;
};

AppendStructuredBuffer<Particle> NewSimulationState : register(u0);
ConsumeStructuredBuffer<Particle> CurrentSimulationState : register(u1);

cbuffer SimParameters
{
	float4 Emitter_Loc;
	float4 Consumer_Loc;
	float4 TimeFactors; 
};

cbuffer ParticleCount
{
	uint4 NumOfParticles;
};
//Constants presenting parts of Newton's law of gravitational pull
static const float Gravity =	10.f;
static const float Mass1 = 1.0f;
static const float Mass2 = 1000.0f;
static const float MassProduct = Mass1*Mass2;
static const float EventHorizon = 5.0f;


[numthreads(512, 1, 1)]
void CSUPDATE_MAIN( uint3 DTid : SV_DispatchThreadID )
{
	//Flattens the thread ID to compare with the number of particles
	uint thisID = DTid.x + DTid.y * 512 + DTid.z * 512 * 512;
	//If the ID would succeed the number of particles and enter an empty buffer,
	//the system will not work and likely break.
	if (thisID < NumOfParticles)
	{
		Particle particle = CurrentSimulationState.Consume();

		float3 pullDir = Consumer_Loc.xyz - particle.Pos.xyz;
		float pullDistance = length(pullDir);
		float3 Force = (Gravity * MassProduct / (pullDistance*pullDistance)) * normalize(pullDir);

			particle.Vel = particle.Vel + (Force / Mass1) * TimeFactors.x;
		particle.Pos += particle.Vel * TimeFactors.x;
		particle.time = particle.time + TimeFactors.x;

		if (pullDistance > EventHorizon)
		{
			// The set time of a particle's life. Could be inserted dynamically I think
			if (particle.time < 10.0f)
			{
				NewSimulationState.Append(particle);
			}
		}



	}

}