float4x4 ShaderMatrix;
texture BackBufferTex : COLOR;
sampler BackBuffer { Texture = BackBufferTex; };

float Strength = 0.8;
float Fade = 0.4;
float Contrast = 1.0;
float Linearization = 0.5;
float Bleach = 0.0;
float Saturation = -0.15;
float RedCurve = 1.0;
float GreenCurve = 1.0;
float BlueCurve = 1.0;
float BaseCurve = 1.5;
float BaseGamma = 1.0;
float EffectGamma = 0.65;
float3 LumCoeff = float3(0.212656, 0.715158, 0.072186);

struct VS_OUTPUT
{
	float4 PositionOut : POSITION;
	float4 ColorOut : COLOR;
	float2 Texture : TEXCOORD0;
};

//Vertex Shader
VS_OUTPUT VS(float4 PositionIn : POSITION, float4 ColorIn : COLOR, float2 Texture : TEXCOORD0)
{
	VS_OUTPUT VertexOut;
	VertexOut.PositionOut = mul(PositionIn, ShaderMatrix);
	VertexOut.ColorOut = ColorIn;
	VertexOut.Texture = Texture;
	return VertexOut;
}

//Pixel Shader
float4 PS(float4 Color : COLOR, float2 Texture : TEXCOORD0) : COLOR
{
	float3 B = tex2D(BackBuffer, float2(Texture.y, -Texture.x)).rgb;
	float3 G = B;
	float3 H = float3(0.01, 0.01, 0.01);
 
	B = saturate(B);
	B = pow(B, Linearization);
	B = lerp(H, B, Contrast);
 
	float A = dot(B.rgb, LumCoeff);
	float3 D = A;
 
	B = pow(B, 1.0 / BaseGamma);
 
	float a = RedCurve;
	float b = GreenCurve;
	float c = BlueCurve;
	float d = BaseCurve;
 
	float y = 1.0 / (1.0 + exp(a / 2.0));
	float z = 1.0 / (1.0 + exp(b / 2.0));
	float w = 1.0 / (1.0 + exp(c / 2.0));
	float v = 1.0 / (1.0 + exp(d / 2.0));
 
	float3 C = B;
 
	D.r = (1.0 / (1.0 + exp(-a * (D.r - 0.5))) - y) / (1.0 - 2.0 * y);
	D.g = (1.0 / (1.0 + exp(-b * (D.g - 0.5))) - z) / (1.0 - 2.0 * z);
	D.b = (1.0 / (1.0 + exp(-c * (D.b - 0.5))) - w) / (1.0 - 2.0 * w);
 
	D = pow(D, 1.0 / EffectGamma);
 
	float3 Di = 1.0 - D;
 
	D = lerp(D, Di, Bleach);
 
	D.r = pow(abs(D.r), 1.0);
	D.g = pow(abs(D.g), 1.0);
	D.b = pow(abs(D.b), 1.0);
 
	if (D.r < 0.5)
		C.r = (2.0 * D.r - 1.0) * (B.r - B.r * B.r) + B.r;
	else
		C.r = (2.0 * D.r - 1.0) * (sqrt(B.r) - B.r) + B.r;
 
	if (D.g < 0.5)
		C.g = (2.0 * D.g - 1.0) * (B.g - B.g * B.g) + B.g;
	else
		C.g = (2.0 * D.g - 1.0) * (sqrt(B.g) - B.g) + B.g;

	if (D.b < 0.5)
		C.b = (2.0 * D.b - 1.0) * (B.b - B.b * B.b) + B.b;
	else
		C.b = (2.0 * D.b - 1.0) * (sqrt(B.b) - B.b) + B.b;
 
	float3 F = lerp(B, C, Strength);
 
	F = (1.0 / (1.0 + exp(-d * (F - 0.5))) - v) / (1.0 - 2.0 * v);
 
	float r2R = 1.0 - Saturation;
	float g2R = 0.0 + Saturation;
	float b2R = 0.0 + Saturation;
 
	float r2G = 0.0 + Saturation;
	float g2G = (1.0 - Fade) - Saturation;
	float b2G = (0.0 + Fade) + Saturation;
 
	float r2B = 0.0 + Saturation;
	float g2B = (0.0 + Fade) + Saturation;
	float b2B = (1.0 - Fade) - Saturation;
 
	float3 iF = F;
 
	F.r = (iF.r * r2R + iF.g * g2R + iF.b * b2R);
	F.g = (iF.r * r2G + iF.g * g2G + iF.b * b2G);
	F.b = (iF.r * r2B + iF.g * g2B + iF.b * b2B);
 
	float N = dot(F.rgb, LumCoeff);
	float3 Cn = F;
 
	if (N < 0.5)
		Cn = (2.0 * N - 1.0) * (F - F * F) + F;
	else
		Cn = (2.0 * N - 1.0) * (sqrt(F) - F) + F;
 
	Cn = pow(max(Cn,0), 1.0 / Linearization);
 
	return float4( lerp(B, Cn, Strength), 1.0);
}

technique RenderScene
{
    pass P0
    {         
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 PS();
    }
}