

struct SKYMAP_VS_OUTPUT	// output structure for skymap vertex shader
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};


SamplerState filters[2] : register(s0); // filter 0 using CLAMP, filter 1 using WRAP


TextureCube SkyMap : register(t0);

float4 main(SKYMAP_VS_OUTPUT input) : SV_TARGET
{

	return SkyMap.Sample(filters[0], input.texCoord);

}
