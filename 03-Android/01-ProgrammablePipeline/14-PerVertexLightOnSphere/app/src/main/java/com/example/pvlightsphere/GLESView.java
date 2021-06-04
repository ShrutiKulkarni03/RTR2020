package com.example.pvlightsphere;

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
import java.nio.ShortBuffer;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context context;
	private GestureDetector gestureDetector;

	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;

	//sphere
	private int[] vao_sphere = new int[1];
	private int[] vbo_position_sphere = new int[1];
	private int[] vbo_normal_sphere = new int[1];
	private int[] vbo_element_sphere = new int[1];

	private int numVertices;
	private int numElements;

	private int modelMatrixUniform;
	private int viewMatrixUniform;
	private int projectionMatrixUniform;

	private int lightDoubleTapUniform;

	private int lAUniform;
	private int lDUniform;
	private int lSUniform;
	private int lightPositionUniform;

	private int kAUniform;
	private int kDUniform;
	private int kSUniform;
	private int kShininessUniform;

	private float perspectiveProjectionMatrix[] = new float[16];

	boolean bLight;
	
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
			"uniform vec3 u_lA; \n" +
			"uniform vec3 u_lD; \n" +
			"uniform vec3 u_lS; \n" +
			"uniform vec3 u_kA; \n" +
			"uniform vec3 u_kD; \n" +
			"uniform vec3 u_kS; \n" +
			"uniform float u_materialShininess; \n" +
			"uniform vec4 u_light_position; \n" +
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
			"		vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates)); \n" +
			"		vec3 reflection_vector = reflect(-light_direction, transformed_normal); \n" +
			"		vec3 view_vector = normalize(-eye_coordinates.xyz); \n" +
			"		vec3 ambient = u_lA * u_kA; \n" +
			"		vec3 diffuse = u_lD * u_kD * max(dot(light_direction, transformed_normal), 0.0); \n" +
			"		vec3 specular = u_lS * u_kS * pow(max(dot(reflection_vector, view_vector), 0.0), u_materialShininess); \n" +
			"		phong_ads_light = ambient + diffuse + specular; \n" +
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
			"uniform lowp int u_light_double_tap; \n" +
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
		
		lAUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lA");
		lDUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lD");
		lSUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lS");
		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");

		kAUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kA");
		kDUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kD");
		kSUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kS");
		kShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_materialShininess");
		
		
		Sphere sphere = new Sphere();

		float sphereVertices[] = new float[1146];
		float sphereNormals[] = new float[1146];
		float sphereTexcoords[] = new float[764];
		short sphereElements[] = new short[2280];

		sphere.getSphereVertexData(sphereVertices, sphereNormals, sphereTexcoords, sphereElements);


		numVertices = sphere.getNumberOfSphereVertices();
		numElements = sphere.getNumberOfSphereElements();

	
		//sphere vao

		GLES32.glGenVertexArrays(1, vao_sphere, 0);
		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glGenBuffers(1, vbo_position_sphere, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_sphere[0]);
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphereVertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(sphereVertices);
		verticesBuffer.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphereVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


		GLES32.glGenBuffers(1, vbo_normal_sphere, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal_sphere[0]);
		byteBuffer = ByteBuffer.allocateDirect(sphereNormals.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(sphereNormals);
		verticesBuffer.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphereNormals.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.SPK_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.SPK_ATTRIBUTE_NORMAL);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


		GLES32.glGenBuffers(1, vbo_element_sphere, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);
		byteBuffer = ByteBuffer.allocateDirect(sphereElements.length * 2);
		byteBuffer.order(ByteOrder.nativeOrder());
		ShortBuffer elementBuffer = byteBuffer.asShortBuffer();
		elementBuffer.put(sphereElements);
		elementBuffer.position(0);
		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, sphereElements.length * 2, elementBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);


		//depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);
		
		bLight = false;

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

			GLES32.glUniform3f(lAUniform, 0.0f, 0.0f, 0.0f);
			GLES32.glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
			GLES32.glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
			GLES32.glUniform4f(lightPositionUniform, 100.0f, 100.0f, 100.0f, 1.0f);

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
		float scaleMatrix[] = new float[16];
		
		

		//sphere

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	

		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, 1.5f, 1.5f, 1.5f);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);
		//Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);

		//update
		

		update();
		requestRender();

	}

	private void update()
	{
		//code
	}


	private void uninitialize()
	{
		//code

		if (vao_sphere[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
			vao_sphere[0] = 0;
		}

		if (vbo_position_sphere[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_sphere, 0);
			vbo_position_sphere[0] = 0;
		}

		if (vbo_normal_sphere[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_normal_sphere, 0);
			vbo_normal_sphere[0] = 0;
		}

		if (vbo_element_sphere[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_element_sphere, 0);
			vbo_element_sphere[0] = 0;
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

