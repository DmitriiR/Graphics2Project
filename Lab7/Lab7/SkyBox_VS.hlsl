#pragma pack_matrix(row_major) // this changes the matrix major


cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
	float4x4 WVP;
}

//V_OUT main(V_IN input )
//{
//	V_OUT output = (V_OUT)0;
//	// ensures translation is preserved during matrix multiply  
//	float4 localH = float4(input.pos.xyz, 1);
//		// move local space vertex from vertex buffer into world space.
//		localH = mul(localH, worldMatrix);
//
//	// TODO: Move into view space, then projection space
//	localH = mul(localH, viewMatrix);
//	localH = mul(localH, projectionMatrix);
//	
//	float4 lightNormals = float4(input.nrm, 1);
//	output.normal = mul(input.nrm, worldMatrix);
//	
//	output.col = float4(input.uvm, 0);
//	
//	output.posH = localH;
//	output.view = float3x3(	viewMatrix._11, viewMatrix._12, viewMatrix._13,
//							viewMatrix._21, viewMatrix._22, viewMatrix._23,
//							viewMatrix._31, viewMatrix._32, viewMatrix._33);
//	return output; // send projected vertex to the rasterizer stage
//}


struct SKYMAP_VS_OUTPUT	
{
	float4 Pos : SV_POSITION;
	float4 texCoord : TEXCOORD;
};

SKYMAP_VS_OUTPUT main(float3 inPos : POS, float3 inTexCoord : UVM, float3 normal : NRM)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	output.Pos = mul(float4(inPos, 1.0f), WVP).xyzw;
	output.texCoord = float4(inPos, 1.0f);

	return output;
}