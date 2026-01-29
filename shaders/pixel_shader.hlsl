Texture2D texDiffuse : register(t0);

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
    float4 Shininess;
};

struct PSIn
{
    float4 Pos         : SV_Position;
    float3 Normal      : NORMAL;
    float2 TexCoord    : TEX;

    float3 PosWorld    : POSITION1;
    float3 NormalWorld : NORMAL1;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
    // Debug shading #1
    // return float4(input.Normal * 0.5 + 0.5, 1);

    // Debug shading #2
    // return float4(input.TexCoord, 0, 1);

    float3 N = normalize(input.NormalWorld);
    float3 L = normalize(light_pos.xyz - input.PosWorld);
    float3 V = normalize(camera_pos.xyz - input.PosWorld);
    float3 R = reflect(-L, N);

    float3 ambient  = AmbientColor.rgb;
    float3 diffuse  = DiffuseColor.rgb * saturate(dot(N, L));
    float3 specular = SpecularColor.rgb * pow(saturate(dot(R, V)), Shininess.x);

    return float4(ambient + diffuse + specular, 1);
}