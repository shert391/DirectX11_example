cbuffer ConstantBuffer : register(b0)
{
    matrix Model;
    matrix View;
    matrix Projection;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(Pos, Model); /*������� ������� ����������*/
    output.Pos = mul(output.Pos, View); /*������� ���������� ������������ ������*/
    output.Pos = mul(output.Pos, Projection); /*���������� ���������� ������������ ������ � ���������� � ���������� [-1, 1]*/
    output.Color = Color;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return input.Color;
}