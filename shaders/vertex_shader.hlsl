cbuffer TransformationBuffer : register(b0)
{
    matrix ModelToWorldMatrix;
    matrix WorldToViewMatrix;
    matrix ProjectionMatrix;
};

struct VSIn
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEX;

    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float3 Normal : NORMAL;
    float2 TexCoord : TEX;

    float3 PosWorld : POSITION1;
    float3 NormalWorld : NORMAL1;

    float3 TangentWorld : TANGENT1;
    float3 BinormalWorld : BINORMAL1;
};

//-----------------------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------------------

VSOut VS_main(VSIn input)
{
    VSOut output = (VSOut) 0;

    matrix MV = mul(WorldToViewMatrix, ModelToWorldMatrix);
    matrix MVP = mul(ProjectionMatrix, MV);

    output.Pos = mul(MVP, float4(input.Pos, 1));

    float4 worldPos = mul(ModelToWorldMatrix, float4(input.Pos, 1));
    output.PosWorld = worldPos.xyz;

    float3 worldNormal = mul((float3x3) ModelToWorldMatrix, input.Normal);
    output.NormalWorld = normalize(worldNormal);

    output.Normal = normalize(worldNormal);
    output.TexCoord = input.TexCoord;
    
    float3 worldTangent = mul((float3x3) ModelToWorldMatrix, input.Tangent);
    float3 worldBinormal = mul((float3x3) ModelToWorldMatrix, input.Binormal);

    output.TangentWorld = normalize(worldTangent);
    output.BinormalWorld = normalize(worldBinormal);

    return output;
}