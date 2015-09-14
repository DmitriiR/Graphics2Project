#pragma pack_matrix(row_major) // this changes the matrix major

struct V_IN
{

	float4 pos : POS;
	float3 uvm : UVM;
	float3 nrm : NRM;

};

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
	float4x4 WVP;
}

struct VS_OUTPUT
{
	//float4 posH : SV_POSITION;// homoinzed rasterized, on the inbound the value is the same as the vLayour, on the output the pixel shader needs to match the PS parameter 

	float4		wPos : POS;
	float3      TexCoord : UVM;
	float4      normal : NRM;
	
	float4		pos : SV_POSITION;
};

VS_OUTPUT main(V_IN input)
{
	VS_OUTPUT output;

	output.pos = mul(input.pos, WVP).xyzw;
	output.wPos = mul(input.pos, worldMatrix).xyzw;
	output.normal = mul( float4(input.nrm,1.0f), worldMatrix).xyzw;

	output.TexCoord = input.uvm;


	return output;
}

//
//cbuffer OBJECT : register(b0)
//{
//	float4x4 worldMatrix;
//}
//cbuffer SCENE : register(b1) // slot
//{
//	float4x4 viewMatrix;
//	float4x4 projectionMatrix;
//}
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
//
