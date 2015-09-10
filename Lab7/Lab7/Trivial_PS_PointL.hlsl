

struct P_IN
{
	//float3 posL : POSITION;
	float4 pos : SV_POSITION;
	float4 uvm : COLOR;
	float3 nrm : NORMAL;
  float3x3 vew : VIEW;
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
	float4 color = {0.0f, 0.0f, 0.0f, 0.0f};
	
	//color.a = baseColor.b;
	//color.r = baseColor.g;
	//color.g = baseColor.r;
	//color.b = baseColor.a;

	// Setting Up the light 
	float3 light_dir		= float3(0.0f, -0.25f,  0.0f);
	float4 light_ambient	= float4(0.1f,	0.1f,	0.1f, 1.0f);
	float4 light_diffuse	= float4(1.0f,	1.0f,	1.0f, 1.0f);

	float4 specular;
	//float3 viewDirection = float3(input.vew, input.vew.z, input.vew.z);
	//float4 textureColor = shaderTexture.Sample(SampleType, input.tex);
	//  color = light_diffuse;

	float3 lightDir = normalize(light_dir - input.pos);

	float lightIntensity = saturate(dot(lightDir, input.nrm));

	color = light_diffuse * baseColor * lightIntensity + light_ambient  * baseColor;
//	if (lightIntensity > 0.0f)
//	{

//		color += (light_diffuse * lightIntensity);
		//color = saturate(color);
		
		// point light 

		//     		LIGHTDIR = NORMALIZE(LIGHTPOS – SURFACEPOS)
		//			LIGHTRATIO = CLAMP(DOT(LIGHTDIR, SURFACENORMAL))
		//			RESULT = LIGHTRATIO * LIGHTCOLOR * SURFACECOLOR

	//	specular = specularIntensity * specularColor * pow(saturate(dot(reflection, normalize(input.viewDirection))), specularPower);

//		float3 reflection = normalize(2 * dot(lightDir, input.nrm) * input.nrm - lightDir);
	//	specular = specularIntensity * specularColor * pow(saturate(dot(reflection, normalize(input.viewDirection))), specularPower);

	//}
//	color = saturate(color);
//	color = saturate(color + specular);
	return color;

}
