

struct P_IN
{
	//float3 posL : POSITION;
	float4 pos : SV_POSITION;
	float4 uvm : COLOR;
	float3 nrm : NORMAL;
	//float4 color : COLOR;
};

//struct light
//{
//	float3 dir;
//	float4 ambient;
//	float4 diffuse;
//};

cbuffer OBJECT : register(b0)
{
	float offset;
	float3 padding;
}


texture2D baseTexture : register(t0); // first texture
SamplerState filters[2] : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(P_IN input) : SV_TARGET
{

	//input.uvm[0] = input.uvm[0] / 4 + offset;
	float4 baseColor = baseTexture.Sample(filters[0], input.uvm.xy); // get base color
	float4 color = baseColor;
	//color.a = baseColor.b;
	//color.r = baseColor.g;
	//color.g = baseColor.r;
	//color.b = baseColor.a;

	// Setting Up the light 
	float3 light_dir		= float3(0.25f, 0.5f,  -1.0f);
	float4 light_ambient	= float4(0.2f,	0.2f,	0.2f, 1.0f);
	float4 light_diffuse	= float4(1.0f,	1.0f,	1.0f, 1.0f);
	

	//input.nrm = normalize(input.nrm);
	float3 finalColor;

	//finalColor = color * light_ambient.xyz;
	//finalColor = saturate(dot(normalize(light_dir), normalize(input.nrm))) * baseColor.xyz ;
	finalColor = saturate((dot(light_dir, input.nrm) + (color * light_diffuse)));
	//finalColor += saturate( dot (light_dir, input.nrm) * light.diffuse * diffuse);
//	return baseColor;
	return float4(finalColor, 1);

	//return color; // return a transition based on the detail alpha
}
