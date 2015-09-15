#pragma pack_matrix(row_major) // this changes the matrix major

struct V_IN
{

	float4 pos : POS;
	float3 uvm : UVM;
	float3 nrm : NRM;

};

cbuffer OBJECT : register(b0)
{
	float4x4 viewMatrix; // view 
	float4x4 WVP;	      // projection
}

cbuffer OBJECT2 : register(b1)
{
	float4x4 worldMatrix; // view 
	    
}
cbuffer OBJECT3 : register(b2)
{
	float4 lightPosition_in; // view 
}


struct VS_OUTPUT
{
	//float4 posH : SV_POSITION;// homoinzed rasterized, on the inbound the value is the same as the vLayour, on the output the pixel shader needs to match the PS parameter 

	float4		wPos : POS;
	float3      TexCoord : UVM;
	float3      normal : NRM;
	float4		pos : SV_POSITION;
	float3      lightPosition :POSITION;
};

VS_OUTPUT main(V_IN input)
{
	VS_OUTPUT output;

	float4 localH = float4(input.pos.xyz, 1);

	localH = mul(localH, worldMatrix);
	localH = mul(localH, WVP);


	output.pos = localH;
	output.wPos = mul(input.pos, worldMatrix).xyzw;
	output.normal = mul( float4(input.nrm,1.0f), worldMatrix).xyzw;
	
	output.TexCoord = input.uvm;
	output.lightPosition = lightPosition_in;

	return output;
}
