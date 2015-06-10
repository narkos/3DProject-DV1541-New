struct Particle
{
	float3 Position;
	float pad1;
	float3 Velocity;
	float time;

};

AppendStructuredBuffer<Particle> NewSimulationState : register(u0);
ConsumeStructuredBuffer<Particle> CurrentSimulationState : register(u1);


cbuffer SimulationParameters
{
	float4 TimeFactors; 
	float4 EmitterLocation; 
	float4 OmitterLocation;
};

cbuffer ParticleCount
{
	uint4 NumParticles;
};

static const float Gravity = 10.0f; 
static const float Mass1 = 1.0f; 
static const float Mass2 = 1000.0f; 
static const float EventHorizon = 5.0f;
static const float MassProduct = (Mass1* Mass2);

[numthreads(512, 1, 1)]
void CSMAINUPD(uint3 DispatchThreadID : SV_DispatchThreadID)
{
	
	// Check for if this thread should run or not.
	uint thisID;
	thisID = DispatchThreadID.x + DispatchThreadID.y * 512 + DispatchThreadID.z * 512 * 512;
	if (thisID < NumParticles.x)
	{

		// Get the current particle 
		Particle part = CurrentSimulationState.Consume(); 
		// Calculate the current gravitational force applied to it 
		float3 distance = OmitterLocation.xyz - part.Position;
		float r = length( distance ); 
		
		float3 Force = (Gravity * MassProduct / (r*r)) * normalize(distance);
		// Calculate the new velocity, accounting for the acceleration from 
		// the gravitational force over the current time step. 
		part.Velocity = part.Velocity + (Force / Mass1) * TimeFactors.x;
		// Calculate the new position, accounting for the new velocity value
		// over the current time step. 
		part.Position += part.Velocity * TimeFactors.x;
		// Update the life time left for the particle, 
		part.time += TimeFactors.x;
		// Test to see how close the particle is to the black hole, and 
		// don't pass it to the output list if it is too close, 
	
		if ( r > EventHorizon ) 
		{ 
			if ( part.time < 10.0f ) 
			{ 
				NewSimulationState.Append( part ); 
			} 
		}

	}
}