package com.example.tess;

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
	private int tesselationControlShaderObject;
	private int tesselationEvaluationShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;

	private int[] vao = new int[1];
	private int[] vbo = new int[1];
	private int mvpUniform;

	private int numberOfSegmentsUniform;
	private int numberOfStripsUniform;
	private int lineColorUniform;

	private int uiNumberOfLineSegments;

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
		uiNumberOfLineSegments--;
		if (uiNumberOfLineSegments <= 0)
			uiNumberOfLineSegments = 1;
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
		uiNumberOfLineSegments++;
		if (uiNumberOfLineSegments >= 30)
			uiNumberOfLineSegments = 30;
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
			"in vec2 vPosition; \n"+
			"void main(void) \n"+
			"{ \n"+
			"	gl_Position = vec4(vPosition, 0.0, 1.0); \n"+
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

		//Tess Control Shader

		tesselationControlShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);

		final String tesslationControlShaderSourceCode = String.format
		(
			"#version 320 es \n"+
			"\n"+
			"layout(vertices = 4)out; \n" +
			"uniform int numberOfSegments; \n" +
			"uniform int numberOfStrips; \n" +
			"void main(void) \n"+
			"{ \n"+
			"	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; \n" +
			"	gl_TessLevelOuter[0] = float(numberOfStrips); \n" +
			"	gl_TessLevelOuter[1] = float(numberOfSegments); \n" +
			"} \n"
		);

		GLES32.glShaderSource(tesselationControlShaderObject, tesslationControlShaderSourceCode);

		GLES32.glCompileShader(tesselationControlShaderObject);
		iShaderCompiledStatus = new int[1];
		iInfoLogLength = new int[1];
		szBuffer = null;

		GLES32.glGetShaderiv(tesselationControlShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(tesselationControlShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(tesselationControlShaderObject);
				System.out.println("SPK: Tess Control Shader Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}

		//Tess Evaluation Shader

		tesselationEvaluationShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);

		final String tesselationEvaluationShaderSourceCode = String.format
		(
			"#version 320 es \n"+
			"\n"+
			"layout(isolines)in; \n" +
			"uniform mat4 u_mvp_matrix; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	float tessCoord = gl_TessCoord.x; \n" +
			"	vec3 p0 = gl_in[0].gl_Position.xyz; \n" +
			"	vec3 p1 = gl_in[1].gl_Position.xyz; \n" +
			"	vec3 p2 = gl_in[2].gl_Position.xyz; \n" +
			"	vec3 p3 = gl_in[3].gl_Position.xyz; \n" +
			"	vec3 p = (p0 * (1.0 - tessCoord) * (1.0 - tessCoord) * (1.0 - tessCoord)) + (p1 * 3.0 * tessCoord * (1.0 - tessCoord) * (1.0 - tessCoord)) + (p2 * 3.0 * tessCoord * tessCoord * (1.0 - tessCoord)) + (p3 * tessCoord * tessCoord * tessCoord) ; \n" +
			"	gl_Position = u_mvp_matrix * vec4(p, 1.0); \n" +
			"} \n"
		);

		GLES32.glShaderSource(tesselationEvaluationShaderObject, tesselationEvaluationShaderSourceCode);

		GLES32.glCompileShader(tesselationEvaluationShaderObject);
		iShaderCompiledStatus = new int[1];
		iInfoLogLength = new int[1];
		szBuffer = null;

		GLES32.glGetShaderiv(tesselationEvaluationShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(tesselationEvaluationShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(tesselationEvaluationShaderObject);
				System.out.println("SPK: Tess Evaluation Shader Compilation Log = "+szBuffer);

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
			"out vec4 FragColor; \n"+
			"uniform vec4 lineColor; \n" +
			"void main(void) \n"+
			"{ \n"+
			"	FragColor = lineColor; \n" +
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

		GLES32.glAttachShader(shaderProgramObject, tesselationControlShaderObject);

		GLES32.glAttachShader(shaderProgramObject, tesselationEvaluationShaderObject);
	
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
	
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
			
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
		numberOfSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfSegments");
		numberOfStripsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfStrips");
		lineColorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "lineColor");

		final float vertices[] = new float[]
		{
			-1.0f, -1.0f,
			-0.5f, 1.0f,
			0.5f, -1.0f,
			1.0f, 1.0f
		};


		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);

		GLES32.glGenBuffers(1, vbo, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);

		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(vertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(vertices);
		verticesBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, vertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_POSITION, 2, GLES32.GL_FLOAT, false, 0, 0);

		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_POSITION);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		//depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);
		
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		uiNumberOfLineSegments = 1;

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
	}


	private void resize(int width, int height)
	{

		if (height == 0)
			height = 1;

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

		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);

		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);

		//modelViewMatrix[0] = translateMatrix[0];

		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, translateMatrix, 0);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
		GLES32.glUniform1i(numberOfSegmentsUniform, uiNumberOfLineSegments);
		GLES32.glUniform1i(numberOfStripsUniform, 1);
		GLES32.glUniform4f(lineColorUniform, 1.0f, 1.0f, 0.0f, 1.0f);
		//GLES32.glUniform4fv(lineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));


		//bind vao
		GLES32.glBindVertexArray(vao[0]);

		GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES, 4);
		GLES32.glDrawArrays(GLES32.GL_PATCHES, 0, 4);

		//unbind vao
		GLES32.glBindVertexArray(0);

		//stop using OpenGL program object
		GLES32.glUseProgram(0);


		requestRender();

	}


	private void uninitialize()
	{
		//code

		if (vao[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao, 0);
			vao[0] = 0;
		}

		if (vbo[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo, 0);
			vbo[0] = 0;
		}


		if (shaderProgramObject != 0)
		{
			
			if(vertexShaderObject != 0)
			{
				GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
				GLES32.glDeleteShader(vertexShaderObject);
				vertexShaderObject = 0;
			}

			if(tesselationControlShaderObject != 0)
			{
				GLES32.glDetachShader(shaderProgramObject, tesselationControlShaderObject);
				GLES32.glDeleteShader(tesselationControlShaderObject);
				tesselationControlShaderObject = 0;
			}

			if(tesselationEvaluationShaderObject != 0)
			{
				GLES32.glDetachShader(shaderProgramObject, tesselationEvaluationShaderObject);
				GLES32.glDeleteShader(tesselationEvaluationShaderObject);
				tesselationEvaluationShaderObject = 0;
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

