package com.example.framebuffer;

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

	private int vertexShaderObjectPyramid;
	private int fragmentShaderObjectPyramid;
	private int shaderProgramObjectPyramid;

	private int vertexShaderObjectCube;
	private int fragmentShaderObjectCube;
	private int shaderProgramObjectCube;

	//triangle
	private int[] vao_pyramid = new int[1];
	private int[] vbo_position_pyramid = new int[1];
	private int[] vbo_texcoord_pyramid = new int[1];

	//cube
	private int[] vao_cube = new int[1];
	private int[] vbo_position_cube = new int[1];
	private int[] vbo_texcoord_cube = new int[1];

	//FBO
	private int[] fbo = new int[1];
	private int[] textureRenderBuffer = new int[1];
	private int[] rbo = new int[1];

	private int mvpUniformPyramid;
	private int textureSamplerUniformPyramid;

	private int mvpUniformCube;
	private int textureSamplerUniformCube;

	private float perspectiveProjectionMatrix[] = new float[16];

	private float pyramidAngle = 0.0f;
	private float cubeAngle = 0.0f;

	private int stone_texture[] = new int[1];
		
	private int fbWidth;
	private int fbHeight;
	
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

		vertexShaderObjectPyramid = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		final String vertexShaderSourceCodePyramid = String.format
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

		GLES32.glShaderSource(vertexShaderObjectPyramid, vertexShaderSourceCodePyramid);

		GLES32.glCompileShader(vertexShaderObjectPyramid);
		int[] iShaderCompiledStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szBuffer = null;

		GLES32.glGetShaderiv(vertexShaderObjectPyramid, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObjectPyramid, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(vertexShaderObjectPyramid);
				System.out.println("SPK: Vertex Shader Pyramid Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}

		//Fragment Shader

		fragmentShaderObjectPyramid = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		final String fragmentShaderSourceCodePyramid = String.format
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

		GLES32.glShaderSource(fragmentShaderObjectPyramid, fragmentShaderSourceCodePyramid);

		GLES32.glCompileShader(fragmentShaderObjectPyramid);
		iShaderCompiledStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetShaderiv(fragmentShaderObjectPyramid, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObjectPyramid, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(fragmentShaderObjectPyramid);
				System.out.println("SPK: Fragment Shader Pyramid Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}


		//Shader Program Object

		
		shaderProgramObjectPyramid = GLES32.glCreateProgram();

		GLES32.glAttachShader(shaderProgramObjectPyramid, fragmentShaderObjectPyramid);
	
		GLES32.glAttachShader(shaderProgramObjectPyramid, fragmentShaderObjectPyramid);
	
		GLES32.glBindAttribLocation(shaderProgramObjectPyramid, GLESMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObjectPyramid, GLESMacros.SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");
			
		GLES32.glLinkProgram(shaderProgramObjectPyramid);


		int[] iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetProgramiv(shaderProgramObjectPyramid, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);

		if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObjectPyramid, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if (iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetProgramInfoLog(shaderProgramObjectPyramid);
				System.out.println("SPK: Shader Program Link Log = "+szBuffer);

				uninitialize();
				System.exit(0);

			}
		}


		//Get Uniform Location

		mvpUniformPyramid = GLES32.glGetUniformLocation(shaderProgramObjectPyramid, "u_mvp_matrix");
		textureSamplerUniformPyramid = GLES32.glGetUniformLocation(shaderProgramObjectPyramid, "u_texture_sampler");


		//Vertex Shader

		vertexShaderObjectCube = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		final String vertexShaderSourceCodeCube = String.format
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

		GLES32.glShaderSource(vertexShaderObjectCube, vertexShaderSourceCodeCube);

		GLES32.glCompileShader(vertexShaderObjectCube);
		iShaderCompiledStatus = new int[1];
		iInfoLogLength = new int[1];
		szBuffer = null;

		GLES32.glGetShaderiv(vertexShaderObjectCube, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObjectCube, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(vertexShaderObjectCube);
				System.out.println("SPK: Vertex Shader Cube Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}

		//Fragment Shader

		fragmentShaderObjectCube = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		final String fragmentShaderSourceCodeCube = String.format
		(
			"#version 320 es \n" +
			"\n" +
			"precision highp float; \n" +
			"uniform highp sampler2D u_texture_sampler; \n" +
			"in vec2 out_texcoord; \n" +
			"out vec4 FragColor; \n" +
			"void main(void) \n" +
			"{ \n"+
			"	vec3 col = texture(u_texture_sampler, out_texcoord).rgb; \n" +
			"	FragColor = vec4(col, 1.0); \n" +
			"} \n"
		);

		GLES32.glShaderSource(fragmentShaderObjectCube, fragmentShaderSourceCodeCube);

		GLES32.glCompileShader(fragmentShaderObjectCube);
		iShaderCompiledStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetShaderiv(fragmentShaderObjectCube, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObjectCube, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(fragmentShaderObjectCube);
				System.out.println("SPK: Fragment Shader Cube Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}


		//Shader Program Object

		
		shaderProgramObjectCube = GLES32.glCreateProgram();

		GLES32.glAttachShader(shaderProgramObjectCube, vertexShaderObjectCube);
	
		GLES32.glAttachShader(shaderProgramObjectCube, fragmentShaderObjectCube);
	
		GLES32.glBindAttribLocation(shaderProgramObjectCube, GLESMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObjectCube, GLESMacros.SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");
			
		GLES32.glLinkProgram(shaderProgramObjectCube);


		iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetProgramiv(shaderProgramObjectCube, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);

		if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObjectCube, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if (iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetProgramInfoLog(shaderProgramObjectCube);
				System.out.println("SPK: Shader Program Link Log = "+szBuffer);

				uninitialize();
				System.exit(0);

			}
		}


		//Get Uniform Location

		mvpUniformCube = GLES32.glGetUniformLocation(shaderProgramObjectCube, "u_mvp_matrix");
		textureSamplerUniformCube = GLES32.glGetUniformLocation(shaderProgramObjectCube, "u_texture_sampler");



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

		/*FBO*/
		GLES32.glGenFramebuffers(1, fbo, 0);
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);

		GLES32.glGenTextures(1, textureRenderBuffer, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, textureRenderBuffer[0]);
		GLES32.glTexImage2D(GLES32.GL_TEXTURE_2D, 0, GLES32.GL_RGB, 1920, 1080, 0, GLES32.GL_RGB, GLES32.GL_UNSIGNED_BYTE, null);      
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR);
		GLES32.glFramebufferTexture2D(GLES32.GL_FRAMEBUFFER, GLES32.GL_COLOR_ATTACHMENT0, GLES32.GL_TEXTURE_2D, textureRenderBuffer[0], 0);

		GLES32.glGenRenderbuffers(1, rbo, 0);
		GLES32.glBindRenderbuffer(GLES32.GL_RENDERBUFFER, rbo[0]);
		GLES32.glRenderbufferStorage(GLES32.GL_RENDERBUFFER, GLES32.GL_DEPTH24_STENCIL8, 1920, 1080);
		GLES32.glFramebufferRenderbuffer(GLES32.GL_FRAMEBUFFER, GLES32.GL_DEPTH_STENCIL_ATTACHMENT, GLES32.GL_RENDERBUFFER, rbo[0]);

		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);


		//depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		//GLES32.glEnable(GLES32.GL_CULL_FACE);

		//textures
		stone_texture[0] = loadTexture(R.raw.stone);
	
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
		fbWidth = width;
		fbHeight = height;

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
		
	}


	private void draw()
	{

		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);

		GLES32.glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		GLES32.glViewport(0, 0, 1920, 1080);

		GLES32.glUseProgram(shaderProgramObjectPyramid);

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

		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.rotateM(rotateMatrixY, 0, pyramidAngle, 0.0f, 1.0f, 0.0f);

		Matrix.multiplyMM(translateMatrix, 0, translateMatrix, 0, rotateMatrixY, 0);
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpUniformPyramid, 1, false, modelViewProjectionMatrix, 0);

		//texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, stone_texture[0]);
		GLES32.glUniform1i(textureSamplerUniformPyramid, 0);


		//bind vao

		GLES32.glBindVertexArray(vao_pyramid[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

		//unbind vao
		GLES32.glBindVertexArray(0);

		//GLES32.glUseProgram(0);

		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);

		


		/*CUBE*/

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		GLES32.glViewport(0, 0, fbWidth, fbHeight);

		GLES32.glUseProgram(shaderProgramObjectCube);

		//cube

		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
		Matrix.setIdentityM(rotateMatrixX, 0);
		Matrix.setIdentityM(rotateMatrixY, 0);
		Matrix.setIdentityM(rotateMatrixZ, 0);

		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
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

		GLES32.glUniformMatrix4fv(mvpUniformCube, 1, false, modelViewProjectionMatrix, 0);

		//texture
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, textureRenderBuffer[0]);
		GLES32.glUniform1i(textureSamplerUniformCube, 0);

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

		//GLES32.glUseProgram(0);


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

		if (fbo[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, fbo, 0);
			fbo[0] = 0;
		}

		if (textureRenderBuffer[0] != 0)
		{
			GLES32.glDeleteBuffers(1, textureRenderBuffer, 0);
			textureRenderBuffer[0] = 0;
		}

		if (rbo[0] != 0)
		{
			GLES32.glDeleteBuffers(1, rbo, 0);
			rbo[0] = 0;
		}

		//textures
		GLES32.glDeleteTextures(1, stone_texture, 0);


		if (shaderProgramObjectPyramid != 0)
		{
			
			if(vertexShaderObjectPyramid != 0)
			{
				GLES32.glDetachShader(shaderProgramObjectPyramid, vertexShaderObjectPyramid);
				GLES32.glDeleteShader(vertexShaderObjectPyramid);
				vertexShaderObjectPyramid = 0;
			}


			if(fragmentShaderObjectPyramid != 0)
			{
				GLES32.glDetachShader(shaderProgramObjectPyramid, fragmentShaderObjectPyramid);
				GLES32.glDeleteShader(fragmentShaderObjectPyramid);
				fragmentShaderObjectPyramid = 0;
			}

			//GLES32.glDeleteProgram(shaderProgramObjectPyramid);
			//shaderProgramObjectPyramid = 0;

		}

		if(shaderProgramObjectPyramid != 0)
		{
			GLES32.glDeleteProgram(shaderProgramObjectPyramid);
			shaderProgramObjectPyramid = 0;
		}
		



		if (shaderProgramObjectCube != 0)
		{
			
			if(vertexShaderObjectCube != 0)
			{
				GLES32.glDetachShader(shaderProgramObjectCube, vertexShaderObjectCube);
				GLES32.glDeleteShader(vertexShaderObjectCube);
				vertexShaderObjectCube = 0;
			}


			if(vertexShaderObjectCube != 0)
			{
				GLES32.glDetachShader(shaderProgramObjectCube, vertexShaderObjectCube);
				GLES32.glDeleteShader(vertexShaderObjectCube);
				vertexShaderObjectCube = 0;
			}

			//GLES32.glDeleteProgram(shaderProgramObjectCube);
			//shaderProgramObjectCube = 0;

		}

		if(shaderProgramObjectCube != 0)
		{
			GLES32.glDeleteProgram(shaderProgramObjectCube);
			shaderProgramObjectCube = 0;
		}


	}

}

