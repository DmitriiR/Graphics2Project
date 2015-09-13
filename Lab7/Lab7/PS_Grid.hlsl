
struct P_IN
{
	//float3 posL : POSITION;
	float4 pos : SV_POSITION;
	float3 uvm : COLOR;
	float3 nrm : NRM;
	//float4 color : COLOR;
};


texture2D baseTexture : register(t0); // first texture



SamplerState filters[2] : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(P_IN input) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(filters[0], input.uvm); // get base color
	float4 color;
	color.a = 1.0F;
	color.r = 0.0F;
	color.g = 1.0F;
	color.b = baseColor.a;

	return color; // return a transition based on the detail alpha
}