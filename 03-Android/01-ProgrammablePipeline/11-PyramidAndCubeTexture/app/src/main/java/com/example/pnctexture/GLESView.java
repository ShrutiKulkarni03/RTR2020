package com.example.pnctexture;

import android.content.Context;

import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;

import android.opengl.GLES32;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.opengl.GLUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;


import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;

	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;

	//triangle
	private int[] vao_pyramid = new int[1];
	private int[] vbo_position_pyramid = new int[1];
	private int[] vbo_texcoord_pyramid = new int[1];

	//cube
	private int[] vao_cube = new int[1];
	private int[] vbo_position_cube = new int[1];
	private int[] vbo_texcoord_cube = new int[1];

	private int mvpUniform;
	private int textureSamplerUniform;
	private float perspectiveProjectionMatrix[] = new float[16];

	private float pyramidAngle = 0.0f;
	private float cubeAngle = 0.0f;

	private int stone_texture;
	private int kundali_texture;
	
	
	
	public GLESView(Context drawingContext)
	{
		super(drawingContext);

		context = drawingContext;

		setEGLContextClientVersion(3);

		setRenderer(this);

		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);

	}


	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		String glesVersion = gl.glGetString(GL10.GL_VERSION);
		System.out.println("SPK: OpenGL-ES Version = "+glesVersion);

		String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
		System.out.println("SPK: GLSL Version = "+glslVersion);

		initialize(gl);
	}


	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height)
	{
		resize(width, height);
	}


	@Override
	public void onDrawFrame(GL10 unused)
	{
		draw();
	}


	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		int eventaction = e.getAction();
		if(!gestureDetector.onTouchEvent(e))
			super.onTouchEvent(e);

		return(true);
	}


	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		return(true);
	}


	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return(true);
	}


	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		return(true);
	}


	@Override
	public boolean onDown(MotionEvent e)
	{
		return(true);
	}


	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return(true);
	}


	@Override
	public void onLongPress(MotionEvent e)
	{
		//code
	}


	@Override
	public void onShowPress(MotionEvent e)
	{
		//code
	}


	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		uninitialize();
		System.exit(0);
		return(true);
	}



	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return(true);
	}


	private void initialize(GL10 gl)
	{
		//Vertex Shader

		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		final String vertexShaderSourceCode = String.format
		(
			"#version 320 es \n" +
			"\n" +
			"in vec4 vPosition; \n" +
			"in vec2 vTexcoord; \n" +
			"uniform mat4 u_mvp_matrix; \n" +
			"out vec2 out_texcoord; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	gl_Position = u_mvp_matrix * vPosition; \n" +
			"	out_texcoord = vTexcoord; \n" +
			"} \n"
		);

		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

		GLES32.glCompileShader(vertexShaderObject);
		int[] iShaderCompiledStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szBuffer = null;

		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("SPK: Vertex Shader Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}

		//Fragment Shader

		fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es \n" +
			"\n" +
			"precision highp float; \n" +
			"uniform highp sampler2D u_texture_sampler; \n" +
			"in vec2 out_texcoord; \n" +
			"out vec4 FragColor; \n" +
			"void main(void) \n" +
			"{ \n"+
			"	FragColor = texture(u_texture_sampler, out_texcoord); \n" +
			"} \n"
		);

		GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);

		GLES32.glCompileShader(fragmentShaderObject);
		iShaderCompiledStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("SPK: Fragment Shader Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}


		//Shader Program Object

		
		shaderProgramObject = GLES32.glCreateProgram();

		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
	
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
	
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");
			
		GLES32.glLinkProgram(shaderProgramObject);


		int[] iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);

		if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if (iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("SPK: Shader Program Link Log = "+szBuffer);

				uninitialize();
				System.exit(0);

			}
		}


		//Get Uniform Location

		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
		textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

		final float pyramidVertices[] = new float[]
		{
			0.0f, 0.5f, 0.0f,     //front
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			0.0f, 0.5f, 0.0f,     //right
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,
			0.0f, 0.5f, 0.0f,     //back
			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.0f, 0.5f, 0.0f,     //left
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f 
		};

		final float pyramidTexcoords[] = new float[]
		{
			0.5f, 1.0f,      //front
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.5f, 1.0f,      //right
			1.0f, 0.0f, 
			0.0f, 0.0f,
			0.5f, 1.0f,      //back
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.5f, 1.0f,      //left
			0.0f, 0.0f,
			1.0f, 0.0f
		};


		final float cubeVertices[] = new float[]
		{
			1.0f, 1.0f, 1.0f,        //front
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,       //right
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,       //back
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,        //left
			-1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,       //top
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,      //bottom
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f			
		};


		final float cubeTexcoords[] = new float[]
		{
			1.0f, 1.0f,        //front
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,        //right
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f,        //back
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 1.0f,        //left
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,        //top
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,        //bottom
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f
		};

	
		//pyramid vao

		GLES32.glGenVertexArrays(1, vao_pyramid, 0);
		GLES32.glBindVertexArray(vao_pyramid[0]);

		GLES32.glGenBuffers(1, vbo_position_pyramid, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_pyramid[0]);
		ByteBuffer byteBufferPositionPyramid = ByteBuffer.allocateDirect(pyramidVertices.length * 4);
		byteBufferPositionPyramid.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBufferPyramid = byteBufferPositionPyramid.asFloatBuffer();
		verticesBufferPyramid.put(pyramidVertices);
		verticesBufferPyramid.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidVertices.length * 4, verticesBufferPyramid, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glGenBuffers(1, vbo_texcoord_pyramid, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord_pyramid[0]);
		ByteBuffer byteBufferTexcoordPyramid = ByteBuffer.allocateDirect(pyramidTexcoords.length * 4);
		byteBufferTexcoordPyramid.order(ByteOrder.nativeOrder());
		FloatBuffer texcoordBufferPyramid = byteBufferTexcoordPyramid.asFloatBuffer();
		texcoordBufferPyramid.put(pyramidTexcoords);
		texcoordBufferPyramid.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidTexcoords.length * 4, texcoordBufferPyramid, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_TEXCOORD);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);


		//cube vao

		GLES32.glGenVertexArrays(1, vao_cube, 0);
		GLES32.glBindVertexArray(vao_cube[0]);

		GLES32.glGenBuffers(1, vbo_position_cube, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_cube[0]);
		ByteBuffer byteBufferPositionCube = ByteBuffer.allocateDirect(cubeVertices.length * 4);
		byteBufferPositionCube.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBufferCube = byteBufferPositionCube.asFloatBuffer();
		verticesBufferCube.put(cubeVertices);
		verticesBufferCube.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, verticesBufferCube, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glGenBuffers(1, vbo_texcoord_cube, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord_cube[0]);
		ByteBuffer byteBufferTexcoordCube = ByteBuffer.allocateDirect(cubeTexcoords.length * 4);
		byteBufferTexcoordCube.order(ByteOrder.nativeOrder());
		FloatBuffer texcoordBufferCube = byteBufferTexcoordCube.asFloatBuffer();
		texcoordBufferCube.put(cubeTexcoords);
		texcoordBufferCube.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeTexcoords.length * 4, texcoordBufferCube, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_TEXCOORD);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);


		//depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);

		//textures
		stone_texture = loadTexture(R.raw.stone);
		kundali_texture = loadTexture(R.raw.vijay_kundali);
		

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
	}


	private int loadTexture(int imageFileResourceID)
	{
		BitmapFactory.Options options = new BitmapFactory.Options();

		options.inScaled = false;

		Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageFileResourceID, options);

		int texture[] = new int[1];

		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);

		GLES32.glGenTextures(1, texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);

		//setting texture parameters
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);

		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);

		GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);

		return(texture[0]);

	}


	private void resize(int width, int height)
	{
		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
		
	}


	private void draw()
	{
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		//OpenGL Drawing
		float modelViewMatrix[] = new float[16];
		float modelViewProjectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];
		float scaleMatrix[] = new float[16];
		float rotateMatrixX[] = new float[16];
		float rotateMatrixY[] = new float[16];
		float rotateMatrixZ[] = new float[16];

		//pyramid

		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(rotateMatrixY, 0);

		Matrix.translateM(translateMatrix, 0, -1.0f, 0.0f, -3.0f);
		Matrix.rotateM(rotateMatrixY, 0, pyramidAngle, 0.0f, 1.0f, 0.0f);

		Matrix.multiplyMM(translateMatrix, 0, translateMatrix, 0, rotateMatrixY, 0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

		//texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, stone_texture);
		GLES32.glUniform1i(textureSamplerUniform, 0);


		//bind vao

		GLES32.glBindVertexArray(vao_pyramid[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

		//unbind vao
		GLES32.glBindVertexArray(0);




		//cube

		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
		Matrix.setIdentityM(rotateMatrixX, 0);
		Matrix.setIdentityM(rotateMatrixY, 0);
		Matrix.setIdentityM(rotateMatrixZ, 0);

		Matrix.translateM(translateMatrix, 0, 1.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, 0.45f, 0.45f, 0.45f);
		Matrix.rotateM(rotateMatrixX, 0, cubeAngle, 1.0f, 0.0f, 0.0f);
		Matrix.rotateM(rotateMatrixY, 0, cubeAngle, 0.0f, 1.0f, 0.0f);
		Matrix.rotateM(rotateMatrixZ, 0, cubeAngle, 0.0f, 0.0f, 1.0f);

		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, scaleMatrix, 0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotateMatrixX, 0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotateMatrixY, 0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotateMatrixZ, 0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

		//texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, kundali_texture);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		//bind vao

		GLES32.glBindVertexArray(vao_cube[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

		//unbind vao
		GLES32.glBindVertexArray(0);



		update();
		requestRender();

	}

	private void update()
	{
		pyramidAngle += 0.5f;
		if (pyramidAngle >= 360.0f)
			pyramidAngle = 0.0f;

		cubeAngle += 0.5f;
		if (cubeAngle >= 360.0f)
			cubeAngle = 0.0f;
	}


	private void uninitialize()
	{
		//code

		if (vao_pyramid[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_pyramid, 0);
			vao_pyramid[0] = 0;
		}

		if (vbo_position_pyramid[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_pyramid, 0);
			vbo_position_pyramid[0] = 0;
		}

		if (vbo_texcoord_pyramid[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_texcoord_pyramid, 0);
			vbo_texcoord_pyramid[0] = 0;
		}


		if (vao_cube[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_cube, 0);
			vao_cube[0] = 0;
		}

		if (vbo_position_cube[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_cube, 0);
			vbo_position_cube[0] = 0;
		}

		if (vbo_texcoord_cube[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_texcoord_cube, 0);
			vbo_texcoord_cube[0] = 0;
		}

		//textures
		//GLES32.glDeleteTextures(1, stone_texture);
		//GLES32.glDeleteTextures(1, kundali_texture);

		

		if (shaderProgramObject != 0)
		{
			
			if(vertexShaderObject != 0)
			{
				GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
				GLES32.glDeleteShader(vertexShaderObject);
				vertexShaderObject = 0;
			}


			if(fragmentShaderObject != 0)
			{
				GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
				GLES32.glDeleteShader(fragmentShaderObject);
				fragmentShaderObject = 0;
			}

			//GLES32.glDeleteProgram(shaderProgramObject);
			//shaderProgramObject = 0;

		}

		if(shaderProgramObject != 0)
		{
			GLES32.glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
		}

	}

}

