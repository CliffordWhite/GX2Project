TextureCube baseTexture : register(t0); // first texture

SamplerState filter : register(s0); // filter 0 using CLAMP, filter 1 using WRAP
struct P_IN
{
	float4 posH : SV_POSITION;
	float4 ColH : COLOR;
	float3 NrmH : NORMAL;
	float3 World : WORLD;
	float3 View : VIEW;
};
float4 main( P_IN input ) : SV_TARGET
{
	return baseTexture.Sample(filter, input.World.xyz);
}