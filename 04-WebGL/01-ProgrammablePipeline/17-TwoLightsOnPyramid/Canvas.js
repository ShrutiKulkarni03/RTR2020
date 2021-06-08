//CODE


//Global Variables
var canvas = null;
var gl = null;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

const WebGLMacros = 
{
	SPK_ATTRIBUTE_POSITION:0,
	SPK_ATTRIBUTE_COLOR:1,
	SPK_ATTRIBUTE_NORMAL:2,
	SPK_ATTRIBUTE_TEXCOORD:3
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao_pyramid;
var vbo_pyramid_position;
var vbo_pyramid_normal;

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var lKeyPressedUniform;

var lAUniform = Array(2);
var lDUniform = Array(2);
var lSUniform = Array(2);
var lightPositionUniform = Array(2);

var kAUniform;
var kDUniform;
var kSUniform;
var kShininessUniform;

var bLight = false;

var perspectiveProjectionMatrix;

var pyramidAngle = 0.0;

var requestAnimationFrame = window.requestAnimationFrame || 
							window.webkitRequestAnimationFrame ||
							window.wozRequestAnimationFrame ||
							window.oRequestAnimationFrame ||
							window.msRequestAnimationFrame;

var cancelAnimationFrame =  window.cancelAnimationFrame ||
							window.webkitCancelRequestAnimationFrame || window.webkitCancelAnimationFrame ||
							window.mozCancelRequestAnimationFrame || window.mozCancelAnimationFrame ||
							window.oCancelRequestAnimationFrame || window.oCancelAnimationFrame ||
							window.msCancelRequestAnimationFrame || window.msCancelAnimationFrame;


function main()
{
	//Get canvas from dom
	canvas = document.getElementById("SPK");

	if(!canvas)
	{
		console.log("Obtaining Canvas Failed. \n");
	}
	else
	{
		console.log("Obtaining Canvas Successful. \n");
	}

	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;


	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);

	window.addEventListener("resize", resize, false);

	init();

	resize();    //warm-up resize

	draw();      //warm-up draw
}

function toggleFullscreen()
{
	var fullscreen_element = document.fullscreenElement ||             
							 document.webkitFullscreenElement ||      
							 document.mozFullScreenElement ||
							 document.msFullscreenElement ||
							 null;


	if(fullscreen_element == null)
	{
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();

		else if(canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullscreen();

		else if(canvas.mozRequestFullscreen)
			canvas.mozRequestFullScreen();

		else if(canvas.msRequestFullscreen)
			msRequestFullscreen();

		bFullscreen = true;
	}

	else
	{
		if(document.exitFullscreen)
			document.exitFullscreen();

		else if(document.webkitExitFullscreen)
			document.webkitExitFullscreen();

		else if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();

		else if(document.msExitFullscreen)
			document.msExitFullscreen();

		bFullscreen = false;
	}
}

function init()
{
	//get drawing context from the canvas
	gl = canvas.getContext("webgl2");

	if(!gl)
	{
		console.log("WebGL Context Obtaining Failed. \n");
	}
	else
	{
		console.log("WebGL Context Obtained Successful. \n");
	}

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	//VERTEX SHADER

	var vertexShaderSourceCode = 
	"#version 300 es \n"+
	"\n"+
	"in vec4 vPosition; \n"+
	"in vec3 vNormal; \n" +
	"uniform lowp int u_lKeyPressed; \n" +
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
	"if(u_lKeyPressed==1) \n" +
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

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);

	if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//FRAGMENT SHADER

	var fragmentShaderSourceCode = 
	"#version 300 es \n"+
	"\n"+
	"precision highp float; \n"+
	"in vec3 phong_ads_light; \n" +
	"out vec4 FragColor; \n" +
	"void main(void) \n" +
	"{ \n" +
	"	FragColor = vec4(phong_ads_light, 1.0f); \n" +
	"} \n"

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);

	if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}


	//SHADER PROGRAM
	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);
	
	//BINDING OF SHADER PROGRAM

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SPK_ATTRIBUTE_NORMAL, "vNormal");


	gl.linkProgram(shaderProgramObject);

	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
	{
		var error = gl.getShaderInfoLog(shaderProgramObject);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//GET UNIFORM LOCATION

	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");
	lKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_lKeyPressed");
	
	lAUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_lA[0]");
	lDUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_lD[0]");
	lSUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_lS[0]");
	lightPositionUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_light_position[0]");
	
	lAUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_lA[1]");
	lDUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_lD[1]");
	lSUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_lS[1]");
	lightPositionUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_light_position[1]");
	
	kAUniform = gl.getUniformLocation(shaderProgramObject, "u_kA");
	kDUniform = gl.getUniformLocation(shaderProgramObject, "u_kD");
	kSUniform = gl.getUniformLocation(shaderProgramObject, "u_kS");
	kShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_materialShininess");
		


	//ARRAYS

	var pyramidVertices = new Float32Array([	0.0, 0.5, 0.0,     //front
												-0.5, -0.5, 0.5,
												0.5, -0.5, 0.5,
												0.0, 0.5, 0.0,     //right
												0.5, -0.5, 0.5,
												0.5, -0.5, -0.5,
												0.0, 0.5, 0.0,     //back
												0.5, -0.5, -0.5,
												-0.5, -0.5, -0.5,
												0.0, 0.5, 0.0,     //left
												-0.5, -0.5, -0.5,
												-0.5, -0.5, 0.5 	]);

	var pyramidNormals = new Float32Array([	0.0, 0.447214, 0.894427,
											0.0, 0.447214, 0.894427,
											0.0, 0.447214, 0.894427,
											0.894427, 0.447214, 0.0,
											0.894427, 0.447214, 0.0,
											0.894427, 0.447214, 0.0,
											0.0, 0.447214, -0.894427,
											0.0, 0.447214, -0.894427,
											0.0, 0.447214, -0.894427,
											-0.894427, 0.447214, 0.0,
											-0.894427, 0.447214, 0.0,
											-0.894427, 0.447214, 0.0 	]);

	

	//PYRAMID VAO 

	vao_pyramid = gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid);

	vbo_pyramid_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_position);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	vbo_pyramid_normal = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_normal);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_NORMAL);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);


	//depth
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.enable(gl.CULL_FACE);

	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	perspectiveProjectionMatrix = mat4.create();

}

function resize()
{
	if(bFullscreen == true)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else
	{
		canvas.width = canvas_original_width;
		canvas.height = canvas_original_height;
	}

	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
		
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);


	if (bLight == true)
	{
		gl.uniform1i(lKeyPressedUniform, 1);
		gl.uniform3f(lAUniform[0], 0.0, 0.0, 0.0);
		gl.uniform3f(lDUniform[0], 1.0, 0.0, 0.0);
		gl.uniform3f(lSUniform[0], 1.0, 0.0, 0.0);
		gl.uniform4f(lightPositionUniform[0], 2.0, 0.0, 0.0, 0.0);
					
		gl.uniform3f(lAUniform[1], 0.0, 0.0, 0.0);
		gl.uniform3f(lDUniform[1], 0.0, 0.0, 1.0);
		gl.uniform3f(lSUniform[1], 0.0, 0.0, 1.0);
		gl.uniform4f(lightPositionUniform[1], -2.0, 0.0, 0.0, 1.0);
		
		gl.uniform3f(kAUniform, 0.0, 0.0, 0.0);
		gl.uniform3f(kDUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(kSUniform, 1.0, 1.0, 1.0);
		gl.uniform1f(kShininessUniform, 50.0);
	}
	else
	{
		gl.uniform1i(lKeyPressedUniform, 0);
	}


	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	//pyramid

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -3.0]);
	mat4.rotateY(modelMatrix, modelMatrix, degToRad(pyramidAngle));

	projectionMatrix = perspectiveProjectionMatrix;
	
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);


	//bind vao
	gl.bindVertexArray(vao_pyramid);

	gl.drawArrays(gl.TRIANGLES, 0, 12);

	//unbind vao
	gl.bindVertexArray(null);

	gl.useProgram(null);

	update();

	requestAnimationFrame(draw);
}

function degToRad(degrees)
{
	return(degrees * Math.PI / 180.0);
}

function update()
{
	pyramidAngle += 0.5;
	if (pyramidAngle >= 360.0)
		pyramidAngle = 0.0;

}

function uninitialize()
{
	if(vao_pyramid)
	{
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid = null;
	}

	if(vbo_pyramid_position)
	{
		gl.deleteBuffer(vbo_pyramid_position);
		vbo_pyramid_position = null;
	}

	if(vbo_pyramid_normal)
	{
		gl.deleteBuffer(vbo_pyramid_normal);
		vbo_pyramid_normal = null;
	}


	if(shaderProgramObject)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject, fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject = null;
		}

		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject = null;
	}
}


function keyDown(event)
{
	switch(event.keyCode)
	{
		case 27:
		uninitialize();
		window.close();
		break;
		
		case 70:
			toggleFullscreen();
			break;

		case 76:
			if(bLight == true)
			{
				bLight = false;
			}
			else
			{
				bLight = true;
			}
			break;

	}
}

function mouseDown()
{
	//code	
}




