
texture2D baseTexture : register(t0); // first texture

SamplerState filter : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

struct V_IN
{
	float4 posH : SV_POSITION;
	float4 ColH : COLOR;
	float3 NrmH : NORMAL;
	float3 World : WORLD;
	float3 View : VIEW;


};

cbuffer AmbientLight:register(b0)
{
	float4 A_Light;
	float3 P_LightPos;
	float motherofone;

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

	float3 pdir = normalize(P_LightPos - Input.World);
		float3 Normal = normalize(Input.NrmH);
		float4 p_light = clamp((dot(pdir, Normal) * baseColor), 0, 1);

		color *= (A_Light + p_light);

	if (color.b <= 0.1f)
		discard;
	return color; // return a transition based on the detail alpha
}
