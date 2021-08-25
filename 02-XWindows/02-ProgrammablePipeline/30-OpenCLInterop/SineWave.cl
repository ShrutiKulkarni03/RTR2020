//OpenCL KERNEL DEFINITION
//global kernel function declaration
__kernel void SineWave_kernel(__global float4* pos, unsigned int meshWidth, unsigned int meshHeight, float animationTime)
{
	//variable declarations
	unsigned int x = get_global_id(0);
	unsigned int y = get_global_id(1);

	float u = x / (float)meshWidth;
	float v = y / (float)meshHeight;

	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;

	float frequency = 4.0f;

	float w = sin(u * frequency + animationTime) * cos(v * frequency + animationTime) * 0.5f;

	pos[y * meshWidth + x] = (float4)(u, w, v, 1.0f);

}



