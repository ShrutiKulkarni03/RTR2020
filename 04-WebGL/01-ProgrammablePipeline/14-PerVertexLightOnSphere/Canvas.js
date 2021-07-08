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

var sphere = null;

var modelMatrixUniform;
var viewMatrixUnifrom;
var projectionMatrixUniform;

var lKeyPressedUniform;

var lAUniform;
var lDUniform;
var lSUniform;
var lightPositionUniform;

var kAUniform;
var kDUniform;
var kSUniform;
var kShininessUniform;

var bLight;

var perspectiveProjectionMatrix;

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
	"in vec4 vPosition; \n" +
	"in vec3 vNormal; \n" +
	"uniform lowp int u_lKeyPressed; \n" +
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
	"	if(u_lKeyPressed==1) \n" +
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
	"uniform lowp int u_lKeyPressed; \n" +
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
	
	lAUniform = gl.getUniformLocation(shaderProgramObject, "u_lA");
	lDUniform = gl.getUniformLocation(shaderProgramObject, "u_lD");
	lSUniform = gl.getUniformLocation(shaderProgramObject, "u_lS");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");
			
	kAUniform = gl.getUniformLocation(shaderProgramObject, "u_kA");
	kDUniform = gl.getUniformLocation(shaderProgramObject, "u_kD");
	kSUniform = gl.getUniformLocation(shaderProgramObject, "u_kS");
	kShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_materialShininess");
		
	//SPHERE

	sphere = new Mesh();
	makeSphere(sphere, 2.0, 50, 50);

	bLight = false;

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

	//LIGHT

	if (bLight == true)
	{
		gl.uniform1i(lKeyPressedUniform, 1);
		gl.uniform3f(lAUniform, 0.0, 0.0, 0.0);
		gl.uniform3f(lDUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(lSUniform, 1.0, 1.0, 1.0);		
		gl.uniform4f(lightPositionUniform, 100.0, 100.0, 100.0, 1.0);

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

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [0.75, 0.75, 0.75]);

	projectionMatrix = perspectiveProjectionMatrix;

	//mat4.multiply(projectionMatrix, perspectiveProjectionMatrix, modelMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();

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
	

}

function uninitialize()
{
	if(sphere)
	{
		sphere.deallocate();
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




