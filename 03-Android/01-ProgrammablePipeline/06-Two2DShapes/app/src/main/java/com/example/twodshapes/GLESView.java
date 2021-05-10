package com.example.twodshapes;

import android.content.Context;

import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;

import android.opengl.GLES32;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

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
	private int[] vao_triangle = new int[1];
	private int[] vbo_position_triangle = new int[1];
	private int[] vbo_color_triangle = new int[1];

	//quad
	private int[] vao_quad = new int[1];
	private int[] vbo_position_quad = new int[1];

	private int mvpUniform;

	private float perspectiveProjectionMatrix[] = new float[16];
	
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
			"#version 320 es \n"+
			"\n"+
			"in vec4 vPosition; \n"+
			"in vec4 vColor; \n" +
			"uniform mat4 u_mvp_matrix; \n"+
			"out vec4 out_color; \n" +
			"void main(void) \n"+
			"{ \n"+
			"	gl_Position = u_mvp_matrix * vPosition; \n"+
			"	out_color = vColor; \n" +
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
			"#version 320 es \n"+
			"\n"+
			"precision highp float; \n"+
			"in vec4 out_color; \n" +
			"out vec4 FragColor; \n"+
			"void main(void) \n"+
			"{ \n"+
			"	FragColor = out_color; \n" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SPK_ATTRIBUTE_COLOR, "vColor");
			
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

		final float triangleVertices[] = new float[]
		{
			 0.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f
		};

		final float triangleColor[] = new float[]
		{
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};

		final float quadVertices[] = new float[]
		{
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f
		};


		//triangle vao

		GLES32.glGenVertexArrays(1, vao_triangle, 0);
		GLES32.glBindVertexArray(vao_triangle[0]);

		GLES32.glGenBuffers(1, vbo_position_triangle, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_triangle[0]);
		ByteBuffer byteBufferPositionTriangle = ByteBuffer.allocateDirect(triangleVertices.length * 4);
		byteBufferPositionTriangle.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBufferTriangle = byteBufferPositionTriangle.asFloatBuffer();
		verticesBufferTriangle.put(triangleVertices);
		verticesBufferTriangle.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleVertices.length * 4, verticesBufferTriangle, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glGenBuffers(1, vbo_color_triangle, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_triangle[0]);
		ByteBuffer byteBufferColorTriangle = ByteBuffer.allocateDirect(triangleColor.length * 4);
		byteBufferColorTriangle.order(ByteOrder.nativeOrder());
		FloatBuffer colorBuffer = byteBufferColorTriangle.asFloatBuffer();
		colorBuffer.put(triangleColor);
		colorBuffer.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_COLOR);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);


		//quad vao

		GLES32.glGenVertexArrays(1, vao_quad, 0);
		GLES32.glBindVertexArray(vao_quad[0]);

		GLES32.glGenBuffers(1, vbo_position_quad, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);
		ByteBuffer byteBufferPositionQuad = ByteBuffer.allocateDirect(quadVertices.length * 4);
		byteBufferPositionQuad.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBufferQuad = byteBufferPositionQuad.asFloatBuffer();
		verticesBufferQuad.put(quadVertices);
		verticesBufferQuad.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, quadVertices.length * 4, verticesBufferQuad, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glVertexAttrib3f(GLESMacros.SPK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
		

		GLES32.glBindVertexArray(0);




		//depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);
		

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
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

		//triangle

		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(translateMatrix, 0, -2.0f, 0.0f, -6.0f);

		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, translateMatrix, 0);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

		//bind vao

		GLES32.glBindVertexArray(vao_triangle[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 3);

		//unbind vao
		GLES32.glBindVertexArray(0);




		//quad

		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(translateMatrix, 0, 2.0f, 0.0f, -6.0f);


		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, translateMatrix, 0);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

		//bind vao
		GLES32.glBindVertexArray(vao_quad[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

		//unbind vao
		GLES32.glBindVertexArray(0);




		//stop using OpenGL program object
		GLES32.glUseProgram(0);


		requestRender();

	}


	private void uninitialize()
	{
		//code

		if (vao_triangle[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_triangle, 0);
			vao_triangle[0] = 0;
		}

		if (vbo_position_triangle[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_triangle, 0);
			vbo_position_triangle[0] = 0;
		}

		if (vbo_color_triangle[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_color_triangle, 0);
			vbo_color_triangle[0] = 0;
		}


		if (vao_quad[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_quad, 0);
			vao_quad[0] = 0;
		}

		if (vbo_position_quad[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_quad, 0);
			vbo_position_quad[0] = 0;
		}

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

