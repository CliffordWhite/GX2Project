
texture2D baseTexture : register(t0); // first texture

SamplerState filter : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

struct V_IN
{
	float4 posH : SV_POSITION;
	float4 ColH : COLOR;
};
// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(V_IN Input) : SV_TARGET
{
	float4 baseColor = baseTexture.Sample(filter, Input.ColH.xy); // get base color
	float4 color;
	color.a = baseColor.b;
	color.r = baseColor.g;
	color.g = baseColor.r;
	color.b = baseColor.a;


	return color; // return a transition based on the detail alpha
}
