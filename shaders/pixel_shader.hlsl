Texture2D texDiffuse : register(t0);
SamplerState texSampler : register(s0);

Texture2D texNormal : register(t1);
SamplerState texSamplerNormal : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 light_pos;
    float4 camera_pos;
};

cbuffer MaterialBuffer : register(b1)
{
    float4 AmbientColor;
    float4 DiffuseColor;
    float4 SpecularColor;
    float4 Shininess = 0.1;
};

struct PSIn
{
    float4 Pos : SV_Position;
    float3 Normal : NORMAL;
    float2 TexCoord : TEX;

    float3 PosWorld : POSITION1;
    float3 NormalWorld : NORMAL1;

    float3 TangentWorld : TANGENT1;
    float3 BinormalWorld : BINORMAL1;
};

float4 PS_main(PSIn input) : SV_Target
{
    // Debug shading #1
    // return float4(input.Normal * 0.5 + 0.5, 1);

    // Debug shading #2
    // return float4(input.TexCoord, 0, 1);

    // 1) Sample new normal and map from color to vector form
    float3 normalSample = texNormal.Sample(texSamplerNormal, input.TexCoord).rgb;
    normalSample = normalSample * 2.0f - 1.0f;

    // 2) Construct TBN and transform new normal to world space
    float3 Nworld = normalize(input.NormalWorld);
    float3 T = normalize(input.TangentWorld - Nworld * dot(input.TangentWorld, Nworld));
    float3 B = normalize(cross(Nworld, T));

    float3x3 TBN = float3x3(T, B, Nworld);
    float3 N = normalize(mul(TBN, normalSample));

    // 3) Phong lighting using the new normal
    float3 texColor = texDiffuse.Sample(texSampler, input.TexCoord).rgb;

    float3 L = normalize(light_pos.xyz - input.PosWorld);
    float3 V = normalize(camera_pos.xyz - input.PosWorld);
    float3 R = reflect(-L, N);

    float3 ambient = AmbientColor.rgb * texColor;
    float3 diffuse = texColor * saturate(dot(N, L));
    float3 specular = SpecularColor.rgb * pow(saturate(dot(R, V)), Shininess.x);

    return float4(ambient + diffuse + specular, 1);
}