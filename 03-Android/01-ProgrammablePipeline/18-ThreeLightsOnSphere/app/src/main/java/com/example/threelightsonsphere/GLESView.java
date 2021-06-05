package com.example.threelightsonsphere;

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


	private int vertexShaderObjectPV;
	private int fragmentShaderObjectPV;
	private int shaderProgramObjectPV;

	private int vertexShaderObjectPF;
	private int fragmentShaderObjectPF;
	private int shaderProgramObjectPF;

	//sphere
	private int[] vao_sphere = new int[1];
	private int[] vbo_position_sphere = new int[1];
	private int[] vbo_normal_sphere = new int[1];
	private int[] vbo_element_sphere = new int[1];

	private int numVertices;
	private int numElements;

	private float r = 5.0f;

	boolean bLight;
	boolean pvLighting = true;
	

	//PV

	private int lightDoubleTapUniformPV;

	private int modelMatrixUniformPV;
	private int viewMatrixUniformPV;
	private int projectionMatrixUniformPV;
	
	private int lAUniformPV[] = new int [3];
	private int lDUniformPV[] = new int [3];
	private int lSUniformPV[] = new int [3];
	private int lightPositionUniformPV[] = new int [3];

	private int kAUniformPV;
	private int kDUniformPV;
	private int kSUniformPV;
	private int kShininessUniformPV;

	//PF

	private int lightDoubleTapUniformPF;

	private int modelMatrixUniformPF;
	private int viewMatrixUniformPF;
	private int projectionMatrixUniformPF;
	
	private int lAUniformPF[] = new int [3];
	private int lDUniformPF[] = new int [3];
	private int lSUniformPF[] = new int [3];
	private int lightPositionUniformPF[] = new int [3];

	private int kAUniformPF;
	private int kDUniformPF;
	private int kSUniformPF;
	private int kShininessUniformPF;


	private float perspectiveProjectionMatrix[] = new float[16];
	
	private float lightAngle0 = 0.0f;
	private float lightAngle1 = 0.0f;
	private float lightAngle2 = 0.0f;

	
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
		if(pvLighting==true)
		{
			pvLighting = false;
		}
		else
		{
			pvLighting = true;
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

		//PV LIGHTING

		//Vertex Shader

		vertexShaderObjectPV = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		final String vertexShaderSourceCodePV = String.format
		(
			"#version 320 es \n"+
			"\n"+
			"in vec4 vPosition; \n" +
			"in vec3 vNormal; \n" +
			"uniform lowp int u_light_double_tap; \n" +
			"uniform vec3 u_lA[3]; \n" +
			"uniform vec3 u_lD[3]; \n" +
			"uniform vec3 u_lS[3]; \n" +
			"uniform vec4 u_light_position[3]; \n" +
			"uniform vec3 u_kA; \n" +
			"uniform vec3 u_kD; \n" +
			"uniform vec3 u_kS; \n" +
			"uniform float u_materialShininess; \n" +
			"uniform mat4 u_projection_matrix; \n" +
			"uniform mat4 u_view_matrix; \n" +
			"uniform mat4 u_model_matrix; \n" +
			"out vec3 phong_ads_light; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	if(u_light_double_tap==1) \n" +
			"	{ \n" +
			"		vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" +
			"		vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal); \n" +
			"		vec3 view_vector = normalize(-eye_coordinates.xyz); \n" +
			"		vec3 ambient[3]; \n" +
			"		vec3 diffuse[3]; \n" +
			"		vec3 specular[3]; \n" +
			"		vec3 light_direction[3]; \n" +
			"		vec3 reflection_vector[3]; \n" +
			"		for(int i=0; i<3; i++) \n" +
			"		{ \n" +
			"			light_direction[i] = normalize(vec3(u_light_position[i] -eye_coordinates)); \n" +
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

		GLES32.glShaderSource(vertexShaderObjectPV, vertexShaderSourceCodePV);

		GLES32.glCompileShader(vertexShaderObjectPV);
		int[] iShaderCompiledStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szBuffer = null;

		GLES32.glGetShaderiv(vertexShaderObjectPV, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObjectPV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(vertexShaderObjectPV);
				System.out.println("SPK: Vertex Shader Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}

		//Fragment Shader

		fragmentShaderObjectPV = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		final String fragmentShaderSourceCodePV = String.format
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

		GLES32.glShaderSource(fragmentShaderObjectPV, fragmentShaderSourceCodePV);

		GLES32.glCompileShader(fragmentShaderObjectPV);
		iShaderCompiledStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetShaderiv(fragmentShaderObjectPV, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObjectPV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(fragmentShaderObjectPV);
				System.out.println("SPK: Fragment Shader Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}


		//Shader Program Object

		
		shaderProgramObjectPV = GLES32.glCreateProgram();

		GLES32.glAttachShader(shaderProgramObjectPV, vertexShaderObjectPV);
	
		GLES32.glAttachShader(shaderProgramObjectPV, fragmentShaderObjectPV);
	
		GLES32.glBindAttribLocation(shaderProgramObjectPV, GLESMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObjectPV, GLESMacros.SPK_ATTRIBUTE_NORMAL, "vNormal");

		GLES32.glLinkProgram(shaderProgramObjectPV);


		int[] iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetProgramiv(shaderProgramObjectPV, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);

		if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObjectPV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if (iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetProgramInfoLog(shaderProgramObjectPV);
				System.out.println("SPK: Shader Program Link Log = "+szBuffer);

				uninitialize();
				System.exit(0);

			}
		}


		//Get Uniform Location

		modelMatrixUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_model_matrix");
		viewMatrixUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_view_matrix");
		projectionMatrixUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_projection_matrix");
		lightDoubleTapUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_light_double_tap");
		
		lAUniformPV[0] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lA[0]");
		lDUniformPV[0] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lD[0]");
		lSUniformPV[0] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lS[0]");
		lightPositionUniformPV[0] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_light_position[0]");

		lAUniformPV[1] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lA[1]");
		lDUniformPV[1] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lD[1]");
		lSUniformPV[1] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lS[1]");
		lightPositionUniformPV[1] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_light_position[1]");

		lAUniformPV[2] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lA[2]");
		lDUniformPV[2] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lD[2]");
		lSUniformPV[2] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_lS[2]");
		lightPositionUniformPV[2] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_light_position[2]");

		kAUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_kA");
		kDUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_kD");
		kSUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_kS");
		kShininessUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_materialShininess");
		

		//PF LIGHTING

		//Vertex Shader

		vertexShaderObjectPF = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

		final String vertexShaderSourceCodePF = String.format
		(
			"#version 320 es \n"+
			"\n"+
			"in vec4 vPosition; \n" +
			"in vec3 vNormal; \n" +
			"uniform lowp int u_light_double_tap; \n" +
			"uniform vec4 u_light_position[3]; \n" +
			"uniform mat4 u_projection_matrix; \n" +
			"uniform mat4 u_model_matrix; \n" +
			"uniform mat4 u_view_matrix; \n" +
			"out vec3 transformed_normal; \n" +
			"out vec3 light_direction[3]; \n" +
			"out vec3 view_vector; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	if(u_light_double_tap==1) \n" +
			"	{ \n" +
			"		vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" +
			"		transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" +
			"		view_vector = -eye_coordinates.xyz; \n" +
			"		for(int i = 0; i < 3; i++) \n" +
			"		{ \n" +
			"			light_direction[i] = vec3(u_light_position[i] - eye_coordinates); \n" +
			"		} \n" +
			"	} \n" +
			"	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" +
			"} \n"
		);

		GLES32.glShaderSource(vertexShaderObjectPF, vertexShaderSourceCodePF);

		GLES32.glCompileShader(vertexShaderObjectPF);
		iShaderCompiledStatus = new int[1];
		iInfoLogLength = new int[1];
		szBuffer = null;

		GLES32.glGetShaderiv(vertexShaderObjectPF, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObjectPF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(vertexShaderObjectPF);
				System.out.println("SPK: Vertex Shader Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}

		//Fragment Shader

		fragmentShaderObjectPF = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		final String fragmentShaderSourceCodePF = String.format
		(
			"#version 320 es \n"+
			"\n"+
			"precision highp float; \n"+
			"in vec3 transformed_normal; \n" +
			"in vec3 light_direction[3]; \n" +
			"in vec3 view_vector; \n" +
			"uniform lowp int u_light_double_tap; \n" +
			"uniform vec3 u_lA[3]; \n" +
			"uniform vec3 u_lD[3]; \n" +
			"uniform vec3 u_lS[3]; \n" +
			"uniform vec3 u_kA; \n" +
			"uniform vec3 u_kD; \n" +
			"uniform vec3 u_kS; \n" +
			"uniform float u_materialShininess; \n" +
			"out vec4 FragColor; \n" +
			"vec3 phong_ads_light; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	vec3 normalized_transformed_normals; \n" +
			"	vec3 normalized_view_vector; \n" +
			"	vec3 normalized_light_direction[3]; \n" +
			"	vec3 reflection_vector[3]; \n" +
			"	vec3 ambient[3]; \n" +
			"	vec3 diffuse[3]; \n" +
			"	vec3 specular[3]; \n" +
			"	if(u_light_double_tap==1) \n" +
			"	{ \n" +
			"		normalized_transformed_normals = normalize(transformed_normal); \n" +
			"		normalized_view_vector = normalize(view_vector); \n" +
			"		for(int i = 0; i < 3; i++) \n" +
			"		{ \n" +
			"			normalized_light_direction[i] = normalize(light_direction[i]); \n" +
			"			reflection_vector[i] = reflect(-normalized_light_direction[i], normalized_transformed_normals); \n" +
			"			ambient[i] = u_lA[i] * u_kA; \n" +
			"			diffuse[i] = u_lD[i] * u_kD * max(dot(normalized_light_direction[i], normalized_transformed_normals), 0.0); \n" +
			"			specular[i] = u_lS[i] * u_kS * pow(max(dot(reflection_vector[i], normalized_view_vector), 0.0), u_materialShininess); \n" +
			"			phong_ads_light += ambient[i] + diffuse[i] + specular[i]; \n" +
			"		} \n" +
			"	} \n" +
			"	else \n" +
			"	{ \n" +
			"		phong_ads_light = vec3(1.0, 1.0, 1.0); \n" +
			"	} \n" +
			"	FragColor = vec4(phong_ads_light, 1.0f); \n" +
			"} \n"
		);

		GLES32.glShaderSource(fragmentShaderObjectPF, fragmentShaderSourceCodePF);

		GLES32.glCompileShader(fragmentShaderObjectPF);
		iShaderCompiledStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetShaderiv(fragmentShaderObjectPF, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);

		if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObjectPF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if(iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetShaderInfoLog(fragmentShaderObjectPF);
				System.out.println("SPK: Fragment Shader Compilation Log = "+szBuffer);

				uninitialize();
				System.exit(0);
			}
		}


		//Shader Program Object

		
		shaderProgramObjectPF = GLES32.glCreateProgram();

		GLES32.glAttachShader(shaderProgramObjectPF, vertexShaderObjectPF);
	
		GLES32.glAttachShader(shaderProgramObjectPF, fragmentShaderObjectPF);
	
		GLES32.glBindAttribLocation(shaderProgramObjectPF, GLESMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObjectPF, GLESMacros.SPK_ATTRIBUTE_NORMAL, "vNormal");

		GLES32.glLinkProgram(shaderProgramObjectPF);


		iShaderProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szBuffer = null;

		GLES32.glGetProgramiv(shaderProgramObjectPF, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);

		if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObjectPF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if (iInfoLogLength[0] > 0)
			{
				szBuffer = GLES32.glGetProgramInfoLog(shaderProgramObjectPF);
				System.out.println("SPK: Shader Program Link Log = "+szBuffer);

				uninitialize();
				System.exit(0);

			}
		}


		//Get Uniform Location

		modelMatrixUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_model_matrix");
		viewMatrixUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_view_matrix");
		projectionMatrixUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_projection_matrix");
		lightDoubleTapUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_light_double_tap");
		
		lAUniformPF[0] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lA[0]");
		lDUniformPF[0] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lD[0]");
		lSUniformPF[0] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lS[0]");
		lightPositionUniformPF[0] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_light_position[0]");

		lAUniformPF[1] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lA[1]");
		lDUniformPF[1] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lD[1]");
		lSUniformPF[1] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lS[1]");
		lightPositionUniformPF[1] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_light_position[1]");

		lAUniformPF[2] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lA[2]");
		lDUniformPF[2] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lD[2]");
		lSUniformPF[2] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_lS[2]");
		lightPositionUniformPF[2] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_light_position[2]");

		kAUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_kA");
		kDUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_kD");
		kSUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_kS");
		kShininessUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_materialShininess");





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

		if(pvLighting==true)
		{
			GLES32.glUseProgram(shaderProgramObjectPV);
		}
		else
		{
			GLES32.glUseProgram(shaderProgramObjectPF);
		}
	
		
		if(bLight == true)
		{

			if(pvLighting==true)
			{
				GLES32.glUniform1i(lightDoubleTapUniformPV, 1);

				GLES32.glUniform3f(lAUniformPV[0], 0.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(lDUniformPV[0], 1.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(lSUniformPV[0], 1.0f, 0.0f, 0.0f);
				GLES32.glUniform4f(lightPositionUniformPV[0], 0.0f, r * (float)Math.sin(lightAngle0), r * (float)Math.cos(lightAngle0), 1.0f);

				GLES32.glUniform3f(lAUniformPV[1], 0.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(lDUniformPV[1], 0.0f, 1.0f, 0.0f);
				GLES32.glUniform3f(lSUniformPV[1], 0.0f, 1.0f, 0.0f);
				GLES32.glUniform4f(lightPositionUniformPV[1], r * (float)Math.sin(lightAngle1), 0.0f, r * (float)Math.cos(lightAngle1), 1.0f);

				GLES32.glUniform3f(lAUniformPV[2], 0.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(lDUniformPV[2], 0.0f, 0.0f, 1.0f);
				GLES32.glUniform3f(lSUniformPV[2], 0.0f, 0.0f, 1.0f);
				GLES32.glUniform4f(lightPositionUniformPV[2], r * (float)Math.sin(lightAngle2), r * (float)Math.cos(lightAngle2), 0.0f, 1.0f);

				GLES32.glUniform3f(kAUniformPV, 0.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(kDUniformPV, 1.0f, 1.0f, 1.0f);
				GLES32.glUniform3f(kSUniformPV, 1.0f, 1.0f, 1.0f);
				GLES32.glUniform1f(kShininessUniformPV, 50.0f);
			}
			else
			{
				GLES32.glUniform1i(lightDoubleTapUniformPF, 1);

				GLES32.glUniform3f(lAUniformPF[0], 0.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(lDUniformPF[0], 1.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(lSUniformPF[0], 1.0f, 0.0f, 0.0f);
				GLES32.glUniform4f(lightPositionUniformPF[0], 0.0f, r * (float)Math.sin(lightAngle0), r * (float)Math.cos(lightAngle0), 1.0f);

				GLES32.glUniform3f(lAUniformPF[1], 0.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(lDUniformPF[1], 0.0f, 1.0f, 0.0f);
				GLES32.glUniform3f(lSUniformPF[1], 0.0f, 1.0f, 0.0f);
				GLES32.glUniform4f(lightPositionUniformPF[1], r * (float)Math.sin(lightAngle1), 0.0f, r * (float)Math.cos(lightAngle1), 1.0f);

				GLES32.glUniform3f(lAUniformPF[2], 0.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(lDUniformPF[2], 0.0f, 0.0f, 1.0f);
				GLES32.glUniform3f(lSUniformPF[2], 0.0f, 0.0f, 1.0f);
				GLES32.glUniform4f(lightPositionUniformPF[2], r * (float)Math.sin(lightAngle2), r * (float)Math.cos(lightAngle2), 0.0f, 1.0f);

				GLES32.glUniform3f(kAUniformPF, 0.0f, 0.0f, 0.0f);
				GLES32.glUniform3f(kDUniformPF, 1.0f, 1.0f, 1.0f);
				GLES32.glUniform3f(kSUniformPF, 1.0f, 1.0f, 1.0f);
				GLES32.glUniform1f(kShininessUniformPF, 50.0f);
			}
			
		}
		else
		{
			GLES32.glUniform1i(lightDoubleTapUniformPV, 0);
			GLES32.glUniform1i(lightDoubleTapUniformPF, 0);
		}

		//OpenGL Drawing

		float modelMatrix[] = new float[16];
		float viewMatrix[] = new float[16];
		float projectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];
		float scaleMatrix[] = new float[16];

		//pyramid

		
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

		if(pvLighting==true)
		{
			GLES32.glUniformMatrix4fv(modelMatrixUniformPV, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(viewMatrixUniformPV, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(projectionMatrixUniformPV, 1, false, projectionMatrix, 0);
		}
		else
		{
			GLES32.glUniformMatrix4fv(modelMatrixUniformPF, 1, false, modelMatrix, 0);
			GLES32.glUniformMatrix4fv(viewMatrixUniformPF, 1, false, viewMatrix, 0);
			GLES32.glUniformMatrix4fv(projectionMatrixUniformPF, 1, false, projectionMatrix, 0);
		}

		

		//bind vao

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);


		update();
		
		requestRender();

	}

	private void update()
	{
		lightAngle0 += 0.01f;
		if (lightAngle0 >= 360.0f)
		{
			lightAngle0 = 0.0f;
		}

		lightAngle1 += 0.01f;
		if (lightAngle1 >= 360.0f)
		{
			lightAngle1 = 0.0f;
		}

		lightAngle2 += 0.01f;
		if (lightAngle2 >= 360.0f)
		{
			lightAngle2 = 0.0f;
		}
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

		
		//PV
		if (shaderProgramObjectPV != 0)
		{
			
			if(vertexShaderObjectPV != 0)
			{
				GLES32.glDetachShader(shaderProgramObjectPV, vertexShaderObjectPV);
				GLES32.glDeleteShader(vertexShaderObjectPV);
				vertexShaderObjectPV = 0;
			}


			if(fragmentShaderObjectPV != 0)
			{
				GLES32.glDetachShader(shaderProgramObjectPV, fragmentShaderObjectPV);
				GLES32.glDeleteShader(fragmentShaderObjectPV);
				fragmentShaderObjectPV = 0;
			}

			//GLES32.glDeleteProgram(shaderProgramObject);
			//shaderProgramObject = 0;

		}

		if(shaderProgramObjectPV != 0)
		{
			GLES32.glDeleteProgram(shaderProgramObjectPV);
			shaderProgramObjectPV = 0;
		}



		//PF
		if (shaderProgramObjectPF != 0)
		{
			
			if(vertexShaderObjectPF != 0)
			{
				GLES32.glDetachShader(shaderProgramObjectPF, vertexShaderObjectPF);
				GLES32.glDeleteShader(vertexShaderObjectPF);
				vertexShaderObjectPF = 0;
			}


			if(fragmentShaderObjectPF != 0)
			{
				GLES32.glDetachShader(shaderProgramObjectPF, fragmentShaderObjectPF);
				GLES32.glDeleteShader(fragmentShaderObjectPF);
				fragmentShaderObjectPF = 0;
			}

			//GLES32.glDeleteProgram(shaderProgramObject);
			//shaderProgramObject = 0;

		}

		if(shaderProgramObjectPF != 0)
		{
			GLES32.glDeleteProgram(shaderProgramObjectPF);
			shaderProgramObjectPF = 0;
		}

	}

}

