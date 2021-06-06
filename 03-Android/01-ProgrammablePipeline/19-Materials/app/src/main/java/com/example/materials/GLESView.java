package com.example.materials;

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

	private int singleTap = 0;

	private float angleX = 0.0f;
	private float angleY = 0.0f;
	private float angleZ = 0.0f;

	private float perspectiveProjectionMatrix[] = new float[16];

	boolean bLight;

	private int vWidth;
	private int vHeight;

	private float r = 5.0f;
	
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
		singleTap++;
		if(singleTap>3)
		{
			singleTap = 0;
			angleX = 0.0f;
			angleY = 0.0f;
			angleZ = 0.0f;
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
			"uniform vec4 u_light_position; \n" +
			"uniform mat4 u_projection_matrix; \n" +
			"uniform mat4 u_model_matrix; \n" +
			"uniform mat4 u_view_matrix; \n" +
			"out vec3 transformed_normal; \n" +
			"out vec3 light_direction; \n" +
			"out vec3 view_vector; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	if(u_light_double_tap==1) \n" +
			"	{ \n" +
			"		vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" +
			"		transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" +
			"		light_direction = vec3(u_light_position - eye_coordinates); \n" +
			"		view_vector = -eye_coordinates.xyz; \n" +
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
			"in vec3 transformed_normal; \n" +
			"in vec3 light_direction; \n" +
			"in vec3 view_vector; \n" +
			"uniform lowp int u_light_double_tap; \n" +
			"uniform vec3 u_lA; \n" +
			"uniform vec3 u_lD; \n" +
			"uniform vec3 u_lS; \n" +
			"uniform vec3 u_kA; \n" +
			"uniform vec3 u_kD; \n" +
			"uniform vec3 u_kS; \n" +
			"uniform float u_materialShininess; \n" +
			"out vec4 FragColor; \n" +
			"vec3 phong_ads_light; \n" +
			"void main(void) \n" +
			"{ \n" +
			"	if(u_light_double_tap==1) \n" +
			"	{ \n" +
			"		vec3 normalized_transformed_normals = normalize(transformed_normal); \n" +
			"		vec3 normalized_light_direction = normalize(light_direction); \n" +
			"		vec3 normalized_view_vector = normalize(view_vector); \n" +
			"		vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals); \n" +
			"		vec3 ambient = u_lA * u_kA; \n" +
			"		vec3 diffuse = u_lD * u_kD * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0); \n" +
			"		vec3 specular = u_lS * u_kS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess); \n" +
			"		phong_ads_light = ambient + diffuse + specular; \n" +
			"	} \n" +
			"	else \n" +
			"	{ \n" +
			"		phong_ads_light = vec3(1.0f, 1.0f, 1.0f); \n" +
			"	} \n" +
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

		GLES32.glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
	}


	private void resize(int width, int height)
	{
		vWidth = width;
		vHeight = height;

		/*width = vWidth;
		height = vHeight;*/

		GLES32.glViewport(0, 0, (int)width, (int)height);

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, ((float)width / 6) / ((float)height / 4), 0.1f, 100.0f);
		
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
			GLES32.glUniform4f(lightPositionUniform, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			GLES32.glUniform1i(lightDoubleTapUniform, 0);
		}


		if(singleTap==1) //X
		{
			GLES32.glUniform4f(lightPositionUniform, 0.0f, r * (float)Math.sin(angleX), r * (float)Math.cos(angleX), 1.0f);
		}
		else if(singleTap==2) //Y
		{
			GLES32.glUniform4f(lightPositionUniform, r * (float)Math.sin(angleY), 0.0f, r * (float)Math.cos(angleY), 1.0f);
		}
		else if(singleTap==3) //Z
		{
			GLES32.glUniform4f(lightPositionUniform, r * (float)Math.sin(angleZ), r * (float)Math.cos(angleZ), 0.0f , 1.0f);
		}
		

		GLES32.glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		GLES32.glUniform3f(kDUniform, 1.0f, 1.0f, 1.0f);
		GLES32.glUniform3f(kSUniform, 1.0f, 1.0f, 1.0f);
		GLES32.glUniform1f(kShininessUniform, 50.0f);


		//OpenGL Drawing

		drawSpheres();

		GLES32.glUseProgram(0);

		//update
		

		update();
		requestRender();

	}

	private void drawSpheres()
	{
		float modelMatrix[] = new float[16];
		float viewMatrix[] = new float[16];
		float projectionMatrix[] = new float[16];
		float translateMatrix[] = new float[16];
		float scaleMatrix[] = new float[16];
		
		float sphereScale = 1.4f;

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, ((float)vWidth / 6) / ((float)vHeight / 4), 0.1f, 100.0f);
		

		//spheres

		// ***** 1st sphere on 1st row, emerald *****


		GLES32.glUniform3f(kAUniform, 0.0215f, 0.1745f, 0.0215f);
		GLES32.glUniform3f(kDUniform, 0.07568f, 0.61424f, 0.07568f);
		GLES32.glUniform3f(kSUniform, 0.633f, 0.727811f, 0.633f);
		GLES32.glUniform1f(kShininessUniform, 0.6f * 128.0f);

		GLES32.glViewport(0, 3 * vHeight / 4, (int)vWidth / 6, (int)vHeight / 4); 

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

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



		// ***** 2nd sphere on 1st row, jade *****



		GLES32.glUniform3f(kAUniform, 0.135f, 0.2225f, 0.1575f);
		GLES32.glUniform3f(kDUniform, 0.54f, 0.89f, 0.63f);
		GLES32.glUniform3f(kSUniform, 0.316228f, 0.316228f, 0.316228f);
		GLES32.glUniform1f(kShininessUniform, 0.1f * 128.0f);

		GLES32.glViewport(vWidth / 6, 3 * vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 3rd sphere on 1st row, obsidian *****


		GLES32.glUniform3f(kAUniform, 0.05375f, 0.05f, 0.06625f);
		GLES32.glUniform3f(kDUniform, 0.18275f, 0.17f, 0.22525f);
		GLES32.glUniform3f(kSUniform, 0.332741f, 0.328634f, 0.346435f);
		GLES32.glUniform1f(kShininessUniform, 0.3f * 128.0f);

		GLES32.glViewport(vWidth / 3, 3 * vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 4th sphere on 1st row, pearl *****


		GLES32.glUniform3f(kAUniform, 0.25f, 0.20725f, 0.20725f);
		GLES32.glUniform3f(kDUniform, 1.0f, 0.829f, 0.829f);
		GLES32.glUniform3f(kSUniform, 0.296648f, 0.296648f, 0.296648f);
		GLES32.glUniform1f(kShininessUniform, 0.088f * 128.0f);

		GLES32.glViewport(vWidth / 2, 3 * vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);
		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 5th sphere on 1st row, ruby *****


		GLES32.glUniform3f(kAUniform, 0.1745f, 0.01175f, 0.01175f);
		GLES32.glUniform3f(kDUniform, 0.61424f, 0.04136f, 0.04136f);
		GLES32.glUniform3f(kSUniform, 0.727811f, 0.626959f, 0.626959f);
		GLES32.glUniform1f(kShininessUniform, 0.6f * 128.0f);

		GLES32.glViewport(2 * vWidth / 3, 3 * vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);




		// ***** 6th sphere on 1st row, turquoise *****


		GLES32.glUniform3f(kAUniform, 0.1f, 0.18725f, 0.1745f);
		GLES32.glUniform3f(kDUniform, 0.396f, 0.74151f, 0.69102f);
		GLES32.glUniform3f(kSUniform, 0.297254f, 0.30829f, 0.306678f);
		GLES32.glUniform1f(kShininessUniform, 0.1f * 128.0f);

		GLES32.glViewport(5 * vWidth / 6, 3 * vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 1st sphere on 2nd row, brass *****


		GLES32.glUniform3f(kAUniform, 0.329412f, 0.223529f, 0.027451f);
		GLES32.glUniform3f(kDUniform, 0.780392f, 0.568627f, 0.113725f);
		GLES32.glUniform3f(kSUniform, 0.992157f, 0.941176f, 0.807843f);
		GLES32.glUniform1f(kShininessUniform, 0.21794872f * 128.0f);

		GLES32.glViewport(0, vHeight / 2, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 2nd sphere on 2nd row, bronze *****


		GLES32.glUniform3f(kAUniform, 0.0215f, 0.1275f, 0.054f);
		GLES32.glUniform3f(kDUniform, 0.714f, 0.4284f, 0.18144f);
		GLES32.glUniform3f(kSUniform, 0.393548f, 0.271906f, 0.166721f);
		GLES32.glUniform1f(kShininessUniform, 0.2f * 128.0f);

		GLES32.glViewport(vWidth / 6, vHeight / 2, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 3rd sphere on 2nd row, chrome *****



		GLES32.glUniform3f(kAUniform, 0.25f, 0.25f, 0.25f);
		GLES32.glUniform3f(kDUniform, 0.4f, 0.4f, 0.4f);
		GLES32.glUniform3f(kSUniform, 0.774597f, 0.774597f, 0.774597f);
		GLES32.glUniform1f(kShininessUniform, 0.6f * 128.0f);

		GLES32.glViewport(vWidth / 3, vHeight / 2, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 4th sphere on 2nd row, copper *****


		GLES32.glUniform3f(kAUniform, 0.19125f, 0.0735f, 0.0225f);
		GLES32.glUniform3f(kDUniform, 0.7038f, 0.27048f, 0.0828f);
		GLES32.glUniform3f(kSUniform, 0.256777f, 0.137622f, 0.086014f);
		GLES32.glUniform1f(kShininessUniform, 0.1f * 128.0f);

		GLES32.glViewport(vWidth / 2, vHeight / 2, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 5th sphere on 2nd row, gold *****


		GLES32.glUniform3f(kAUniform, 0.24725f, 0.1995f, 0.0745f);
		GLES32.glUniform3f(kDUniform, 0.75164f, 0.60648f, 0.22648f);
		GLES32.glUniform3f(kSUniform, 0.628281f, 0.555802f, 0.366065f);
		GLES32.glUniform1f(kShininessUniform, 0.4f * 128.0f);

		GLES32.glViewport(2 * vWidth / 3, vHeight / 2, (int)vWidth / 6, (int)vHeight / 4);

		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 6th sphere on 2nd row, silver *****


		GLES32.glUniform3f(kAUniform, 0.19225f, 0.19225f, 0.19225f);
		GLES32.glUniform3f(kDUniform, 0.50754f, 0.50754f, 0.50754f);
		GLES32.glUniform3f(kSUniform, 0.508273f, 0.508273f, 0.508273f);
		GLES32.glUniform1f(kShininessUniform, 0.4f * 128.0f);

		GLES32.glViewport(5 * vWidth / 6, vHeight / 2, (int)vWidth / 6, (int)vHeight / 4);


		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		
		// ***** 1st sphere on 3rd row, black *****

		GLES32.glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		GLES32.glUniform3f(kDUniform, 0.01f, 0.01f, 0.01f);
		GLES32.glUniform3f(kSUniform, 0.5f, 0.5f, 0.5f);
		GLES32.glUniform1f(kShininessUniform, 0.25f * 128.0f);

		GLES32.glViewport(0, vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 2nd sphere on 3rd row, cyan *****


		GLES32.glUniform3f(kAUniform, 0.0f, 0.1f, 0.06f);
		GLES32.glUniform3f(kDUniform, 0.0f, 0.50980392f, 0.50980392f);
		GLES32.glUniform3f(kSUniform, 0.50196078f, 0.50196078f, 0.50196078f);
		GLES32.glUniform1f(kShininessUniform, 0.25f * 128.0f);


		GLES32.glViewport(vWidth / 6, vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 3rd sphere on 3rd row, green *****


		GLES32.glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		GLES32.glUniform3f(kDUniform, 0.1f, 0.35f, 0.1f);
		GLES32.glUniform3f(kSUniform, 0.45f, 0.55f, 0.45f);
		GLES32.glUniform1f(kShininessUniform, 0.25f * 128.0f);

		GLES32.glViewport(vWidth / 3, vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 4th sphere on 3rd row, red *****


		GLES32.glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		GLES32.glUniform3f(kDUniform, 0.5f, 0.0f, 0.0f);
		GLES32.glUniform3f(kSUniform, 0.7f, 0.6f, 0.6f);
		GLES32.glUniform1f(kShininessUniform, 0.25f * 128.0f);

		GLES32.glViewport(vWidth / 2, vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);


		// ***** 5th sphere on 3rd row, white *****


		GLES32.glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		GLES32.glUniform3f(kDUniform, 0.55f, 0.55f, 0.55f);
		GLES32.glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
		GLES32.glUniform1f(kShininessUniform, 0.25f * 128.0f);

		GLES32.glViewport(2 * vWidth / 3, vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 6th sphere on 3rd row, yellow plastic *****


		GLES32.glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		GLES32.glUniform3f(kDUniform, 0.5f, 0.5f, 0.0f);
		GLES32.glUniform3f(kSUniform, 0.6f, 0.6f, 0.5f);
		GLES32.glUniform1f(kShininessUniform, 0.25f * 128.0f);

		GLES32.glViewport(5 * vWidth / 6, vHeight / 4, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);




		// ***** 1st sphere on 4th row, black *****


		GLES32.glUniform3f(kAUniform, 0.02f, 0.02f, 0.02f);
		GLES32.glUniform3f(kDUniform, 0.01f, 0.01f, 0.01f);
		GLES32.glUniform3f(kSUniform, 0.4f, 0.4f, 0.4f);
		GLES32.glUniform1f(kShininessUniform, 0.078125f * 128.0f);

		GLES32.glViewport(0, 0, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 2nd sphere on 4th row, cyan *****


		GLES32.glUniform3f(kAUniform, 0.0f, 0.05f, 0.05f);
		GLES32.glUniform3f(kDUniform, 0.4f, 0.5f, 0.5f);
		GLES32.glUniform3f(kSUniform, 0.04f, 0.7f, 0.7f);
		GLES32.glUniform1f(kShininessUniform, 0.078125f * 128.0f);

		GLES32.glViewport(vWidth / 6, 0, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 3rd sphere on 4th row, green *****


		GLES32.glUniform3f(kAUniform, 0.0f, 0.05f, 0.0f);
		GLES32.glUniform3f(kDUniform, 0.4f, 0.5f, 0.4f);
		GLES32.glUniform3f(kSUniform, 0.04f, 0.7f, 0.04f);
		GLES32.glUniform1f(kShininessUniform, 0.078125f * 128.0f);

		GLES32.glViewport(vWidth / 3, 0, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 4th sphere on 4th row, red *****


		GLES32.glUniform3f(kAUniform, 0.05f, 0.0f, 0.0f);
		GLES32.glUniform3f(kDUniform, 0.5f, 0.4f, 0.4f);
		GLES32.glUniform3f(kSUniform, 0.7f, 0.04f, 0.04f);
		GLES32.glUniform1f(kShininessUniform, 0.078125f * 128.0f);


		GLES32.glViewport(vWidth / 2, 0, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);




		// ***** 5th sphere on 4th row, white *****


		GLES32.glUniform3f(kAUniform, 0.05f, 0.05f, 0.05f);
		GLES32.glUniform3f(kDUniform, 0.5f, 0.5f, 0.5f);
		GLES32.glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
		GLES32.glUniform1f(kShininessUniform, 0.078125f * 128.0f);

		GLES32.glViewport(2 * vWidth / 3, 0, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);



		// ***** 6th sphere on 4th row, yellow rubber *****


		GLES32.glUniform3f(kAUniform, 0.05f, 0.05f, 0.0f);
		GLES32.glUniform3f(kDUniform, 0.5f, 0.5f, 0.0f);
		GLES32.glUniform3f(kSUniform, 0.7f, 0.7f, 0.04f);
		GLES32.glUniform1f(kShininessUniform, 0.078125f * 128.0f);

		GLES32.glViewport(5 * vWidth / 6, 0, (int)vWidth / 6, (int)vHeight / 4);


		
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(projectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
	
		
		Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.scaleM(scaleMatrix, 0, sphereScale, sphereScale, sphereScale);
		

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);

		projectionMatrix = perspectiveProjectionMatrix;

		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		//GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

		//bind vao
		

		GLES32.glBindVertexArray(vao_sphere[0]);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere[0]);

		GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//unbind vao
		GLES32.glBindVertexArray(0);

	}

	private void update()
	{
		//code

		if(singleTap==1)
		{
			angleX += 0.01f;
		}
		else if(singleTap==2)
		{
			angleY += 0.01f;
		}
		else if(singleTap==3)
		{
			angleZ += 0.01f;
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

