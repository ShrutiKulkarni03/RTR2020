package com.example.twolightsonpyramid;

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

	/*private class Light
	{
		float lightAmbient[] = new float[4];
		float lightDiffuse[] = new float[4];
		float lightSpecular[] = new float[4];
		float lightPosition[] = new float[4];
	}*/

	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;

	//triangle
	private int[] vao_pyramid = new int[1];
	private int[] vbo_position_pyramid = new int[1];
	private int[] vbo_normal_pyramid = new int[1];

	private int modelMatrixUniform;
	private int viewMatrixUniform;
	private int projectionMatrixUniform;

	private int lightDoubleTapUniform;

	//private Light light[] = new Light[2];

	private int lAUniform[] = new int [2];
	private int lDUniform[] = new int [2];
	private int lSUniform[] = new int [2];
	private int lightPositionUniform[] = new int [2];

	private int kAUniform;
	private int kDUniform;
	private int kSUniform;
	private int kShininessUniform;

	boolean bLight;
	boolean bAnimate = false;

	private float perspectiveProjectionMatrix[] = new float[16];

	private float pyramidAngle = 0.0f;
	
	
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
		if(bLight == true)
		{
			bLight = false;
		}
		else
		{
			bLight = true;
		}
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
		if(bAnimate==true)
		{
			bAnimate = false;
		}
		else
		{
			bAnimate = true;
		}
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
			"in vec4 vPosition; \n" +
			"in vec3 vNormal; \n" +
			"uniform lowp int u_light_double_tap; \n" +
			"uniform vec3 u_lA[2]; \n" +
			"uniform vec3 u_lD[2]; \n" +
			"uniform vec3 u_lS[2]; \n" +
			"uniform vec4 u_light_position[2]; \n" +
			"uniform vec3 u_kA; \n" +
			"uniform vec3 u_kD; \n" +
			"uniform vec3 u_kS; \n" +
			"uniform float u_materialShininess; \n" +
			"uniform mat4 u_projection_matrix; \n" +
			"uniform mat4 u_model_matrix; \n" +
			"uniform mat4 u_view_matrix; \n" +
			"out vec3 phong_ads_light; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	if(u_light_double_tap==1) \n" +
			"	{ \n" +
			"		vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" +
			"		vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal); \n" +
			"		vec3 view_vector = normalize(-eye_coordinates.xyz); \n" +
			"		vec3 ambient[2]; \n" +
			"		vec3 diffuse[2]; \n" +
			"		vec3 specular[2]; \n" +
			"		vec3 light_direction[2]; \n" +
			"		vec3 reflection_vector[2]; \n" +
			"		for(int i=0; i<2; i++) \n" +
			"		{ \n" +
			"			light_direction[i] = normalize(vec3(u_light_position[i] -eye_coordinates)); \n"+
			"			reflection_vector[i] = reflect(-light_direction[i], transformed_normal); \n" +
			"			ambient[i] = u_lA[i] * u_kA; \n" +
			"			diffuse[i] = u_lD[i] * u_kD * max(dot(light_direction[i], transformed_normal), 0.0); \n" +
			"			specular[i] = u_lS[i] * u_kS * pow(max(dot(reflection_vector[i], view_vector), 0.0), u_materialShininess); \n" +
			"			phong_ads_light = phong_ads_light + ambient[i] + diffuse[i] + specular[i]; \n" +
			"		} \n" +
			"	} \n" +
			"	else \n" +
			"	{ \n" +
			"		phong_ads_light = vec3(1.0, 1.0, 1.0); \n" +
			"	} \n" +
			"	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" +
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
			"in vec3 phong_ads_light; \n" +
			"out vec4 FragColor; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	FragColor = vec4(phong_ads_light, 1.0f); \n" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.SPK_ATTRIBUTE_NORMAL, "vNormal");

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

		modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_model_matrix");
		viewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
		projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
		lightDoubleTapUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_double_tap");
		
		lAUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "u_lA[0]");
		lDUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "u_lD[0]");
		lSUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "u_lS[0]");
		lightPositionUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position[0]");

		lAUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "u_lA[1]");
		lDUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "u_lD[1]");
		lSUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "u_lS[1]");
		lightPositionUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position[1]");


		kAUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kA");
		kDUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kD");
		kSUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kS");
		kShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_materialShininess");
		

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

		final float pyramidNormals[] = new float[]
		{
			0.0f, 0.447214f, 0.894427f,
			0.0f, 0.447214f, 0.894427f,
			0.0f, 0.447214f, 0.894427f,
			0.894427f, 0.447214f, 0.0f,
			0.894427f, 0.447214f, 0.0f,
			0.894427f, 0.447214f, 0.0f,
			0.0f, 0.447214f, -0.894427f,
			0.0f, 0.447214f, -0.894427f,
			0.0f, 0.447214f, -0.894427f,
			-0.894427f, 0.447214f, 0.0f,
			-0.894427f, 0.447214f, 0.0f,
			-0.894427f, 0.447214f, 0.0f
		};

	
		//pyramid vao

		GLES32.glGenVertexArrays(1, vao_pyramid, 0);
		GLES32.glBindVertexArray(vao_pyramid[0]);

		GLES32.glGenBuffers(1, vbo_position_pyramid, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_pyramid[0]);
		ByteBuffer byteBufferPyramid = ByteBuffer.allocateDirect(pyramidVertices.length * 4);
		byteBufferPyramid.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBufferPyramid = byteBufferPyramid.asFloatBuffer();
		verticesBufferPyramid.put(pyramidVertices);
		verticesBufferPyramid.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidVertices.length * 4, verticesBufferPyramid, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glGenBuffers(1, vbo_normal_pyramid, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal_pyramid[0]);
		byteBufferPyramid = ByteBuffer.allocateDirect(pyramidNormals.length * 4);
		byteBufferPyramid.order(ByteOrder.nativeOrder());
		FloatBuffer normalsBufferPyramid = byteBufferPyramid.asFloatBuffer();
		normalsBufferPyramid.put(pyramidNormals);
		normalsBufferPyramid.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidNormals.length * 4, normalsBufferPyramid, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_NORMAL);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

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

		
		if(bLight == true)
		{
			GLES32.glUniform1i(lightDoubleTapUniform, 1);


			GLES32.glUniform3f(lAUniform[0], 0.0f, 0.0f, 0.0f);
			GLES32.glUniform3f(lDUniform[0], 1.0f, 0.0f, 0.0f);
			GLES32.glUniform3f(lSUniform[0], 1.0f, 0.0f, 0.0f);
			GLES32.glUniform4f(lightPositionUniform[0], 2.0f, 0.0f, 0.0f, 0.0f);

			GLES32.glUniform3f(lAUniform[1], 0.0f, 0.0f, 0.0f);
			GLES32.glUniform3f(lDUniform[1], 0.0f, 0.0f, 1.0f);
			GLES32.glUniform3f(lSUniform[1], 0.0f, 0.0f, 1.0f);
			GLES32.glUniform4f(lightPositionUniform[1], -2.0f, 0.0f, 0.0f, 1.0f);

			GLES32.glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
			GLES32.glUniform3f(kDUniform, 1.0f, 1.0f, 1.0f);
			GLES32.glUniform3f(kSUniform, 1.0f, 1.0f, 1.0f);
			GLES32.glUniform1f(kShininessUniform, 50.0f);
		}
		else
		{
			GLES32.glUniform1i(lightDoubleTapUniform, 0);
		}

		//OpenGL Drawing

		float modelMatrix[] = new float[16];
		float viewMatrix[] = new float[16];
		float projectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];
		float rotateMatrix[] = new float[16];

		//pyramid

		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(rotateMatrix, 0);

		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.rotateM(rotateMatrix, 0, pyramidAngle, 0.0f, 1.0f, 0.0f);

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, rotateMatrix, 0);
		//Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);


		//bind vao

		GLES32.glBindVertexArray(vao_pyramid[0]);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

		//unbind vao
		GLES32.glBindVertexArray(0);


		if(bAnimate==true)
		{
			update();
		}
		
		requestRender();

	}

	private void update()
	{
		pyramidAngle += 0.5f;
		if (pyramidAngle >= 360.0f)
			pyramidAngle = 0.0f;
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

		if (vbo_normal_pyramid[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_normal_pyramid, 0);
			vbo_normal_pyramid[0] = 0;
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

