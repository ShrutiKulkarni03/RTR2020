//CUDA KERNEL DEFINITION
//global kernel function declaration
__global__ void SineWave_kernel(float4* pos, unsigned int meshWidth, unsigned int meshHeight, float time)
{
	//variable declarations
	unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

	float u = x / (float)meshWidth;
	float v = y / (float)meshHeight;

	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;

	float frequency = 4.0f;

	float w = sinf(u * frequency + time) * cosf(v * frequency + time) * 0.5f;

	pos[y * meshWidth + x] = make_float4(u, w, v, 1.0f);

}

void launchCUDAKernel(float4* pos, unsigned int meshWidth, unsigned int meshHeight, float time)
{
	dim3 block(8, 8, 1);
	dim3 grid(meshWidth / block.x, meshHeight / block.y, 1);

	/*dim3 block = dim3(8, 8, 1);
	dim3 grid = dim3(meshWidth / block.x, meshHeight / block.y, 1);*/
	

	SineWave_kernel <<< grid, block >>>
		(pos, meshWidth, meshHeight, time);
}

