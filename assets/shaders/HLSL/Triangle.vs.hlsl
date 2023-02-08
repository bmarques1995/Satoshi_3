struct VSIn
{
    float3 pos : POSITION;
    float4 col : COLOR;
};

struct VSOut
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

VSOut main(VSIn input)
{
    VSOut output;
    output.pos = float4(input.pos, 1.0f);
    output.col = input.col;
    return output;
}