package com.example.solarSystem;

public class stack
{
	private int maxSize = 50;
	private float[][] data = new float[maxSize][16];
	private int top = -1;
	private float[] arr = new float[16];

	public void push(float[] matrix)
	{
		top++;
		for(int i=0; i<16; i++)
		{
			data[top][i] = matrix[i];
		}
		
	}

	public float[] pop()
	{
		for(int i=0; i<16; i++)
		{
			arr[i] = data[top][i];
		}
		top--;

		return (arr);
	}
}
