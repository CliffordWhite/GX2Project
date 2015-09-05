
texture2D baseTexture : register(t0); // first texture

SamplerState filter : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

struct V_IN
{
	float4 posH : SV_POSITION;
	float4 ColH : COLOR;
	float3 Nrm : NORMAL;
};

cbuffer DLight:register(b0)
{
	float4 Col;
	float4 Dir;
};
// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(V_IN Input) : SV_TARGET
{
	float4 color;
	float4 baseColor = baseTexture.Sample(filter, Input.ColH.xy); // get base color
		if (baseColor.a + baseColor.g + baseColor.b + baseColor.r == 0)
		{
			color = Input.ColH;
		}
		else
		{
			color.a = baseColor.b;
			color.r = baseColor.g;
			color.g = baseColor.r;
			color.b = baseColor.a;
		}
	float LIGHTRATIO = saturate(dot(-(float3(Dir.x, Dir.y, Dir.z)), Input.Nrm));
	float4 RESULT = LIGHTRATIO * Col * color;

	return RESULT; // return a transition based on the detail alpha
}
