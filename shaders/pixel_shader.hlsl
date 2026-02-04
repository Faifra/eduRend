Texture2D texDiffuse : register(t0);
Texture2D texNormal : register(t1);
SamplerState texSampler : register(s0);

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
    float3 normalSample = texNormal.Sample(texSampler, input.TexCoord).xyz;
    normalSample = normalSample * 2.0f - 1.0f;

    float3 T = normalize(input.TangentWorld);
    float3 B = normalize(input.BinormalWorld);
    float3 N = normalize(input.NormalWorld);

    float3x3 TBN = float3x3(T, B, N);
    float3 Nmap = normalize(mul(normalSample, TBN));

    float3 L = normalize(light_pos.xyz - input.PosWorld);
    float3 V = normalize(camera_pos.xyz - input.PosWorld);
    float3 R = reflect(-L, Nmap);

    float3 texColor = texDiffuse.Sample(texSampler, input.TexCoord).rgb;

    float3 ambient = AmbientColor.rgb * texColor;
    float3 diffuse = texColor * saturate(dot(Nmap, L));
    float3 specular = SpecularColor.rgb * pow(saturate(dot(R, V)), Shininess.x);

    return float4(ambient + diffuse + specular, 1);
}