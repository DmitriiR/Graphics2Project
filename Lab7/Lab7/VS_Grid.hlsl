#pragma pack_matrix(row_major)



struct V_IN
{
	//float3 posL : POSITION;
	float4 pos : POS;
	float3 uvm : UVM;
	float3 nrm : NRM;
	//float4 color : COLOR;
};



struct V_OUT
{
	//float4 posH : SV_POSITION;// homoinzed rasterized, on the inbound the value is the same as the vLayour, on the output the pixel shader needs to match the PS parameter 
	
	float4 posH : SV_POSITION;
	float3 col : COLOR;
	float3 nrm : NRM;

};
cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1) // slot
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}
V_OUT main(V_IN input)
{
	V_OUT output = (V_OUT)0;
	// ensures translation is preserved during matrix multiply  
	float4 localH = float4(input.pos.xyz, 1);
		// move local space vertex from vertex buffer into world space.
		localH = mul(localH, worldMatrix);

	// TODO: Move into view space, then projection space
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);


	output.col = float4(input.uvm, 1);

	output.posH = localH;

	return output; // send projected vertex to the rasterizer stage
}