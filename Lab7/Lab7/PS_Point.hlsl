



//struct Light
//{
//	float3 dir;
//	float3 pos;
//	float  range;
//	float3 att;
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


float4 main(float4 pos : POS, float3 uvm : UVM ,float3 nrm : NRM ) : SV_TARGET
{

	float4 baseColor = baseTexture.Sample(filters[0], uvm.xy);

	float3 norm = normalize(nrm);

	float4 finalColor = baseColor;

	float3 toLight = float3(0.0f, 1.0f, 0.0f) - pos.xyz;
	 
    float4 lightColor = float4(1.0f, 0.3f, 0.3f, 1.0f);
	
	float min_dist = 1.0f;
	float max_dist = 5.0f;

	float distance = length(toLight);

	float strength =  1 - saturate( (distance - min_dist ) / ( max_dist - min_dist));
	
	finalColor = strength * lightColor * baseColor;

//	LIGHTDIR = NORMALIZE(LIGHTPOS – SURFACEPOS)
//	float LIGHTRATIO = CLAMP(DOT(LIGHTDIR, SURFACENORMAL))
//		RESULT = LIGHTRATIO * LIGHTCOLOR * SURFACECOLOR


//	input.normal = normalize(input.normal);
//
//float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
//
//float3 finalColor = float3(0.0f, 0.0f, 0.0f);
//
////Create the vector between light position and pixels position
//float3 lightToPixelVec = light.pos - input.worldPos;
//
////Find the distance between the light pos and pixel pos
//float d = length(lightToPixelVec);
//
////Create the ambient light
//float3 finalAmbient = diffuse * light.ambient;
//
////If pixel is too far, return pixel color with ambient light
//if (d > light.range)
//return float4(finalAmbient, diffuse.a);
//
////Turn lightToPixelVec into a unit length vector describing
////the pixels direction from the lights position
//lightToPixelVec /= d;
//
////Calculate how much light the pixel gets by the angle
////in which the light strikes the pixels surface
//float howMuchLight = dot(lightToPixelVec, input.normal);
//
////If light is striking the front side of the pixel
//if (howMuchLight > 0.0f)
//{
//	//Add light to the finalColor of the pixel
//	finalColor += howMuchLight * diffuse * light.diffuse;
//
//	//Calculate Light's Falloff factor
//	finalColor /= light.att[0] + (light.att[1] * d) + (light.att[2] * (d*d));
//}
//
////make sure the values are between 1 and 0, and add the ambient
//finalColor = saturate(finalColor + finalAmbient);
//
////Return Final Color
return finalColor;
}