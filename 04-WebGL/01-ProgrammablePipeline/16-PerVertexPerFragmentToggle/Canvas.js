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

var vertexShaderObjectPV;
var fragmentShaderObjectPV;
var shaderProgramObjectPV;
var vertexShaderObjectPF;
var fragmentShaderObjectPF;
var shaderProgramObjectPF;

var sphere = null;

//PV

var modelMatrixUniformPV;
var viewMatrixUnifromPV;
var projectionMatrixUniformPV;

var lKeyPressedUniformPV;

var lAUniformPV;
var lDUniformPV;
var lSUniformPV;
var lightPositionUniformPV;

var kAUniformPV;
var kDUniformPV;
var kSUniformPV;
var kShininessUniformPV;

//PF

var modelMatrixUniformPF;
var viewMatrixUnifromPF;
var projectionMatrixUniformPF;

var lKeyPressedUniformPF;

var lAUniformPF;
var lDUniformPF;
var lSUniformPF;
var lightPositionUniformPF;

var kAUniformPF;
var kDUniformPF;
var kSUniformPF;
var kShininessUniformPF;


var pvLighting = true;
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


	//PV

	//VERTEX SHADER

	var vertexShaderSourceCodePV = 
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

	vertexShaderObjectPV = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObjectPV, vertexShaderSourceCodePV);
	gl.compileShader(vertexShaderObjectPV);

	if(gl.getShaderParameter(vertexShaderObjectPV, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObjectPV);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//FRAGMENT SHADER

	var fragmentShaderSourceCodePV = 
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

	fragmentShaderObjectPV = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObjectPV, fragmentShaderSourceCodePV);
	gl.compileShader(fragmentShaderObjectPV);

	if(gl.getShaderParameter(fragmentShaderObjectPV, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObjectPV);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}


	//SHADER PROGRAM
	shaderProgramObjectPV = gl.createProgram();
	gl.attachShader(shaderProgramObjectPV, vertexShaderObjectPV);
	gl.attachShader(shaderProgramObjectPV, fragmentShaderObjectPV);
	
	//BINDING OF SHADER PROGRAM

	gl.bindAttribLocation(shaderProgramObjectPV, WebGLMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObjectPV, WebGLMacros.SPK_ATTRIBUTE_NORMAL, "vNormal");


	gl.linkProgram(shaderProgramObjectPV);

	if(!gl.getProgramParameter(shaderProgramObjectPV, gl.LINK_STATUS))
	{
		var error = gl.getShaderInfoLog(shaderProgramObjectPV);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//GET UNIFORM LOCATION

	modelMatrixUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_model_matrix");
	viewMatrixUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_view_matrix");
	projectionMatrixUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_projection_matrix");
	lKeyPressedUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_lKeyPressed");	
	
	lAUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_lA");
	lDUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_lD");
	lSUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_lS");
	lightPositionUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_light_position");
			
	kAUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_kA");
	kDUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_kD");
	kSUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_kS");
	kShininessUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_materialShininess");


	//PF

	//VERTEX SHADER

	var vertexShaderSourceCodePF = 
	"#version 300 es \n"+
	"\n"+
	"in vec4 vPosition; \n" +
	"in vec3 vNormal; \n" +
	"uniform lowp int u_lKeyPressed; \n" +
	"uniform vec4 u_light_position; \n" +
	"uniform mat4 u_projection_matrix; \n" +
	"uniform mat4 u_model_matrix; \n" +
	"uniform mat4 u_view_matrix; \n" +
	"out vec3 transformed_normal; \n" +
	"out vec3 light_direction; \n" +
	"out vec3 view_vector; \n" +
	"void main(void) \n" +
	"{ \n" +
	"	if(u_lKeyPressed==1) \n" +
	"	{ \n" +
	"		vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" +
	"		transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" +
	"		light_direction = vec3(u_light_position - eye_coordinates); \n" +
	"		view_vector = -eye_coordinates.xyz; \n" +
	"	} \n" +
	"	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" +
	"} \n"

	vertexShaderObjectPF = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObjectPF, vertexShaderSourceCodePF);
	gl.compileShader(vertexShaderObjectPF);

	if(gl.getShaderParameter(vertexShaderObjectPF, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObjectPF);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//FRAGMENT SHADER

	var fragmentShaderSourceCodePF = 
	"#version 300 es \n"+
	"\n"+
	"precision highp float; \n"+
	"in vec3 transformed_normal; \n" +
	"in vec3 light_direction; \n" +
	"in vec3 view_vector; \n" +
	"uniform lowp int u_lKeyPressed; \n" +
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
	"	if(u_lKeyPressed==1) \n" +
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
	"		phong_ads_light = vec3(1.0, 1.0, 1.0); \n" +
	"	} \n" +
	"	FragColor = vec4(phong_ads_light, 1.0f); \n" +
	"} \n"

	fragmentShaderObjectPF = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObjectPF, fragmentShaderSourceCodePF);
	gl.compileShader(fragmentShaderObjectPF);

	if(gl.getShaderParameter(fragmentShaderObjectPF, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObjectPF);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}


	//SHADER PROGRAM
	shaderProgramObjectPF = gl.createProgram();
	gl.attachShader(shaderProgramObjectPF, vertexShaderObjectPF);
	gl.attachShader(shaderProgramObjectPF, fragmentShaderObjectPF);
	
	//BINDING OF SHADER PROGRAM

	gl.bindAttribLocation(shaderProgramObjectPF, WebGLMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObjectPF, WebGLMacros.SPK_ATTRIBUTE_NORMAL, "vNormal");


	gl.linkProgram(shaderProgramObjectPF);

	if(!gl.getProgramParameter(shaderProgramObjectPF, gl.LINK_STATUS))
	{
		var error = gl.getShaderInfoLog(shaderProgramObjectPF);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//GET UNIFORM LOCATION

	modelMatrixUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_model_matrix");
	viewMatrixUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_view_matrix");
	projectionMatrixUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_projection_matrix");
	lKeyPressedUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_lKeyPressed");	
	
	lAUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_lA");
	lDUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_lD");
	lSUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_lS");
	lightPositionUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_light_position");
			
	kAUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_kA");
	kDUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_kD");
	kSUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_kS");
	kShininessUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_materialShininess");
		
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

	if(pvLighting==true)
	{
		gl.useProgram(shaderProgramObjectPV);
	}
	else
	{
		gl.useProgram(shaderProgramObjectPF);
	}
	

	//LIGHT

	if (bLight == true)
	{
		if(pvLighting==true)
		{
			gl.uniform1i(lKeyPressedUniformPV, 1);
			gl.uniform3f(lAUniformPV, 0.0, 0.0, 0.0);
			gl.uniform3f(lDUniformPV, 1.0, 1.0, 1.0);
			gl.uniform3f(lSUniformPV, 1.0, 1.0, 1.0);		
			gl.uniform4f(lightPositionUniformPV, 100.0, 100.0, 100.0, 1.0);

			gl.uniform3f(kAUniformPV, 0.0, 0.0, 0.0);
			gl.uniform3f(kDUniformPV, 1.0, 1.0, 1.0);
			gl.uniform3f(kSUniformPV, 1.0, 1.0, 1.0);	
			gl.uniform1f(kShininessUniformPV, 50.0);	
		}
		else
		{
			gl.uniform1i(lKeyPressedUniformPF, 1);
			gl.uniform3f(lAUniformPF, 0.0, 0.0, 0.0);
			gl.uniform3f(lDUniformPF, 1.0, 1.0, 1.0);
			gl.uniform3f(lSUniformPF, 1.0, 1.0, 1.0);		
			gl.uniform4f(lightPositionUniformPF, 100.0, 100.0, 100.0, 1.0);

			gl.uniform3f(kAUniformPF, 0.0, 0.0, 0.0);
			gl.uniform3f(kDUniformPF, 1.0, 1.0, 1.0);
			gl.uniform3f(kSUniformPF, 1.0, 1.0, 1.0);	
			gl.uniform1f(kShininessUniformPF, 50.0);	
		}

		
	}
	else
	{
		if(pvLighting==true)
		{
			gl.uniform1i(lKeyPressedUniformPV, 0);
		}
		else
		{
			gl.uniform1i(lKeyPressedUniformPF, 0);
		}		
	}


	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [0.75, 0.75, 0.75]);

	projectionMatrix = perspectiveProjectionMatrix;

	if(pvLighting==true)
	{
		gl.uniformMatrix4fv(modelMatrixUniformPV, false, modelMatrix);
		gl.uniformMatrix4fv(viewMatrixUniformPV, false, viewMatrix);
		gl.uniformMatrix4fv(projectionMatrixUniformPV, false, projectionMatrix);
	}
	else
	{
		gl.uniformMatrix4fv(modelMatrixUniformPF, false, modelMatrix);
		gl.uniformMatrix4fv(viewMatrixUniformPF, false, viewMatrix);
		gl.uniformMatrix4fv(projectionMatrixUniformPF, false, projectionMatrix);
	}

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


	if(shaderProgramObjectPV)
	{
		if(fragmentShaderObjectPV)
		{
			gl.detachShader(shaderProgramObjectPV, fragmentShaderObjectPV);
			gl.deleteShader(fragmentShaderObjectPV);
			fragmentShaderObjectPV = null;
		}

		if(vertexShaderObjectPV)
		{
			gl.detachShader(shaderProgramObjectPV, vertexShaderObjectPV);
			gl.deleteShader(vertexShaderObjectPV);
			vertexShaderObjectPV = null;
		}

		gl.deleteProgram(shaderProgramObjectPV);
		shaderProgramObjectPV = null;
	}

	
	if(shaderProgramObjectPF)
	{
		if(fragmentShaderObjectPF)
		{
			gl.detachShader(shaderProgramObjectPF, fragmentShaderObjectPF);
			gl.deleteShader(fragmentShaderObjectPF);
			fragmentShaderObjectPF = null;
		}

		if(vertexShaderObjectPF)
		{
			gl.detachShader(shaderProgramObjectPF, vertexShaderObjectPF);
			gl.deleteShader(vertexShaderObjectPF);
			vertexShaderObjectPF = null;
		}

		gl.deleteProgram(shaderProgramObjectPF);
		shaderProgramObjectPF = null;
	}
}


function keyDown(event)
{
	switch(event.keyCode)
	{
		case 27: //esc
		toggleFullscreen();
		break;
		
		case 70: //f
			pvLighting = false;
			break;

		case 76: //l
		if(bLight == true)
		{
			bLight = false;
		}
		else
		{
			bLight = true;
		}
		break;

		case 86: //v
			pvLighting = true;
		break;

		case 81: //q
			uninitialize();
			window.close();
		break;

	}
}

function mouseDown()
{
	//code	
}




