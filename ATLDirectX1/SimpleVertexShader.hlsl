cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float4 pos : POSITION;
	float3 color : COLOR0;
};

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	//float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
   output.pos = mul(input.pos, model);
   output.pos = mul(output.pos, view);
   output.pos = mul(output.pos, projection);
	//output.pos = pos;

	// Pass through the color without modification.
	output.color = input.color;

	return output;
}
