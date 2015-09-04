#pragma pack_matrix(row_major)
struct V_IN
{
	float3 posL : POSITION;
	float3 ColL : COLOR;
	float3 NorL : NORMAL;

};
struct V_OUT
{
	float4 posH : SV_POSITION;
	float4 ColH : COLOR;
	float3 NrmH : NORMAL;
};
cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}
V_OUT main(V_IN input)
{
	V_OUT output = (V_OUT)0;
	// ensures translation is preserved during matrix multiply
	float4 localH = float4(input.posL.xyz, 1);
	// move local space vertex from vertex buffer into world space.
	localH = mul(localH, worldMatrix);
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);
	// TODO: Move into view space, then projection space
	output.posH = localH;
	output.ColH = float4(input.ColL,1);

	output.NrmH = mul(input.NorL, worldMatrix);




	return output; // send projected vertex to the rasterizer stage
}