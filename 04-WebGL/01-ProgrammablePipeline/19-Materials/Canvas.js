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

var angleForXRotation = 0.0;
var angleForYRotation = 0.0;
var angleForZRotation = 0.0;

var keyPressed = 0;

var vWidth;
var vHeight;

var r = 5.0;

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
	"	FragColor = vec4(phong_ads_light, 1.0); \n" +
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

	gl.clearColor(0.2, 0.2, 0.2, 1.0);

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

	vWidth = canvas.width;
	vHeight = canvas.height;

	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix, 45.0, (parseFloat(canvas.width) /6) / (parseFloat(canvas.height) / 4), 0.1, 100.0);	

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
		gl.uniform4f(lightPositionUniform, 0.0, 0.0, 0.0, 1.0);

	}
	else
	{
		gl.uniform1i(lKeyPressedUniform, 0);
	}

	if (keyPressed == 1) //X
	{
		gl.uniform4f(lightPositionUniform, 0.0, r * Math.sin(angleForXRotation), r * Math.cos(angleForXRotation), 1.0);
	}
	else if (keyPressed == 2) //Y
	{
		gl.uniform4f(lightPositionUniform, r * Math.sin(angleForYRotation), 0.0, r * Math.cos(angleForYRotation), 1.0);
	}
	else if (keyPressed == 3) //Z
	{
		gl.uniform4f(lightPositionUniform, r * Math.sin(angleForZRotation), r * Math.cos(angleForZRotation), 0.0, 1.0);
	}

	gl.uniform3f(kAUniform, 0.0, 0.0, 0.0);
	gl.uniform3f(kDUniform, 1.0, 1.0, 1.0);
	gl.uniform3f(kSUniform, 1.0, 1.0, 1.0);	
	gl.uniform1f(kShininessUniform, 50.0);	

	//OpenGL drawing

	drawSpheres();

	gl.useProgram(null);

	update();

	requestAnimationFrame(draw);
}

function drawSpheres()
{
	var sphereScale = 0.75;

	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.perspective(perspectiveProjectionMatrix, 45.0, (parseFloat(canvas.width) / 6) / (parseFloat(canvas.height) / 4), 0.1, 100.0);	
	
	//spheres

	// ***** 1st sphere on 1st row, emerald *****

	gl.uniform3f(kAUniform, 0.0215, 0.1745, 0.0215);
	gl.uniform3f(kDUniform, 0.07568, 0.61424, 0.07568);
	gl.uniform3f(kSUniform, 0.633, 0.727811, 0.633);	
	gl.uniform1f(kShininessUniform, 0.6 * 128.0);	

	gl.viewport(0, 3 * vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 2nd sphere on 1st row, jade *****

	gl.uniform3f(kAUniform, 0.135, 0.2225, 0.1575);
	gl.uniform3f(kDUniform, 0.54, 0.89, 0.63);
	gl.uniform3f(kSUniform, 0.316228, 0.316228, 0.316228);	
	gl.uniform1f(kShininessUniform, 0.1 * 128.0);	

	gl.viewport(vWidth / 6, 3 * vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 3rd sphere on 1st row, obsidian *****

	gl.uniform3f(kAUniform, 0.05375, 0.05, 0.06625);
	gl.uniform3f(kDUniform, 0.18275, 0.17, 0.22525);
	gl.uniform3f(kSUniform, 0.332741, 0.328634, 0.346435);	
	gl.uniform1f(kShininessUniform, 0.3 * 128.0);	

	gl.viewport(vWidth / 3, 3 * vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 4th sphere on 1st row, pearl *****

	gl.uniform3f(kAUniform, 0.25, 0.20725, 0.20725);
	gl.uniform3f(kDUniform, 1.0, 0.829, 0.829);
	gl.uniform3f(kSUniform, 0.296648, 0.296648, 0.296648);	
	gl.uniform1f(kShininessUniform, 0.088 * 128.0);	

	gl.viewport(vWidth / 2, 3 * vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 5th sphere on 1st row, ruby *****

	gl.uniform3f(kAUniform, 0.1745, 0.01175, 0.01175);
	gl.uniform3f(kDUniform, 0.61424, 0.04136, 0.04136);
	gl.uniform3f(kSUniform, 0.727811, 0.626959, 0.626959);	
	gl.uniform1f(kShininessUniform, 0.6 * 128.0);	

	gl.viewport(2 * vWidth / 3, 3 * vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 6th sphere on 1st row, turquoise *****

	gl.uniform3f(kAUniform, 0.1, 0.18725, 0.1745);
	gl.uniform3f(kDUniform, 0.396, 0.74151, 0.69102);
	gl.uniform3f(kSUniform, 0.297254, 0.30829, 0.306678);	
	gl.uniform1f(kShininessUniform, 0.1 * 128.0);	

	gl.viewport(5 * vWidth / 6, 3 * vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 1st sphere on 2nd row, brass *****

	gl.uniform3f(kAUniform, 0.329412, 0.223529, 0.027451);
	gl.uniform3f(kDUniform, 0.780392, 0.568627, 0.113725);
	gl.uniform3f(kSUniform, 0.992157, 0.941176, 0.807843);	
	gl.uniform1f(kShininessUniform, 0.21794872 * 128.0);	

	gl.viewport(0, vHeight / 2, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 2nd sphere on 2nd row, bronze *****

	gl.uniform3f(kAUniform, 0.0215, 0.1275, 0.054);
	gl.uniform3f(kDUniform, 0.714, 0.4284, 0.18144);
	gl.uniform3f(kSUniform, 0.393548, 0.271906, 0.166721);	
	gl.uniform1f(kShininessUniform, 0.2 * 128.0);	

	gl.viewport(vWidth / 6, vHeight / 2, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 3rd sphere on 2nd row, chrome *****

	gl.uniform3f(kAUniform, 0.25, 0.25, 0.25);
	gl.uniform3f(kDUniform, 0.4, 0.4, 0.4);
	gl.uniform3f(kSUniform, 0.774597, 0.774597, 0.774597);	
	gl.uniform1f(kShininessUniform, 0.6 * 128.0);	

	gl.viewport(vWidth / 3, vHeight / 2, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 4th sphere on 2nd row, copper *****

	gl.uniform3f(kAUniform, 0.19125, 0.0735, 0.0225);
	gl.uniform3f(kDUniform, 0.7038, 0.27048, 0.0828);
	gl.uniform3f(kSUniform,  0.256777, 0.137622, 0.086014);	
	gl.uniform1f(kShininessUniform, 0.1 * 128.0);	

	gl.viewport(vWidth / 2, vHeight / 2, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 5th sphere on 2nd row, gold *****

	gl.uniform3f(kAUniform, 0.24725, 0.1995, 0.0745);
	gl.uniform3f(kDUniform, 0.75164, 0.60648, 0.22648);
	gl.uniform3f(kSUniform,  0.628281, 0.555802, 0.366065);	
	gl.uniform1f(kShininessUniform, 0.4 * 128.0);	

	gl.viewport(2 * vWidth / 3, vHeight / 2, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 6th sphere on 2nd row, silver *****

	gl.uniform3f(kAUniform, 0.19225, 0.19225, 0.19225);
	gl.uniform3f(kDUniform, 0.50754, 0.50754, 0.50754);
	gl.uniform3f(kSUniform,   0.508273, 0.508273, 0.508273);	
	gl.uniform1f(kShininessUniform, 0.4 * 128.0);	

	gl.viewport(5 * vWidth / 6, vHeight / 2, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 1st sphere on 3rd row, black *****

	gl.uniform3f(kAUniform, 0.0, 0.0, 0.0);
	gl.uniform3f(kDUniform, 0.01, 0.01, 0.01);
	gl.uniform3f(kSUniform,   0.5, 0.5, 0.5);	
	gl.uniform1f(kShininessUniform, 0.25 * 128.0);	

	gl.viewport(0, vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 2nd sphere on 3rd row, cyan *****

	gl.uniform3f(kAUniform, 0.0, 0.1, 0.06);
	gl.uniform3f(kDUniform, 0.0, 0.50980392, 0.50980392);
	gl.uniform3f(kSUniform, 0.50196078, 0.50196078, 0.50196078);	
	gl.uniform1f(kShininessUniform, 0.25 * 128.0);	

	gl.viewport(vWidth / 6, vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 3rd sphere on 3rd row, green *****

	gl.uniform3f(kAUniform, 0.0, 0.0, 0.0);
	gl.uniform3f(kDUniform, 0.1, 0.35, 0.1);
	gl.uniform3f(kSUniform, 0.45, 0.55, 0.45);	
	gl.uniform1f(kShininessUniform, 0.25 * 128.0);	

	gl.viewport(vWidth / 3, vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 4th sphere on 3rd row, red *****

	gl.uniform3f(kAUniform, 0.0, 0.0, 0.0);
	gl.uniform3f(kDUniform, 0.1, 0.35, 0.1);
	gl.uniform3f(kSUniform, 0.45, 0.55, 0.45);	
	gl.uniform1f(kShininessUniform, 0.25 * 128.0);	

	gl.viewport(vWidth / 2, vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 5th sphere on 3rd row, white *****

	gl.uniform3f(kAUniform, 0.0, 0.0, 0.0);
	gl.uniform3f(kDUniform, 0.55, 0.55, 0.55);
	gl.uniform3f(kSUniform, 0.7, 0.7, 0.7);	
	gl.uniform1f(kShininessUniform, 0.25 * 128.0);	

	gl.viewport(2 * vWidth / 3, vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 6th sphere on 3rd row, yellow plastic *****

	gl.uniform3f(kAUniform, 0.0, 0.0, 0.0);
	gl.uniform3f(kDUniform, 0.5, 0.5, 0.0);
	gl.uniform3f(kSUniform, 0.6, 0.6, 0.5);	
	gl.uniform1f(kShininessUniform, 0.25 * 128.0);	

	gl.viewport(5 * vWidth / 6, vHeight / 4, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 1st sphere on 4th row, black *****

	gl.uniform3f(kAUniform, 0.02, 0.02, 0.02);
	gl.uniform3f(kDUniform, 0.01, 0.01, 0.01);
	gl.uniform3f(kSUniform, 0.4, 0.4, 0.4);	
	gl.uniform1f(kShininessUniform, 0.078125 * 128.0);	

	gl.viewport(0, 0, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 2nd sphere on 4th row, cyan *****

	gl.uniform3f(kAUniform, 0.0, 0.05, 0.05);
	gl.uniform3f(kDUniform, 0.4, 0.5, 0.5);
	gl.uniform3f(kSUniform, 0.04, 0.7, 0.7);	
	gl.uniform1f(kShininessUniform, 0.078125 * 128.0);	

	gl.viewport(vWidth / 6, 0, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 3rd sphere on 4th row, green *****

	gl.uniform3f(kAUniform, 0.0, 0.05, 0.0);
	gl.uniform3f(kDUniform, 0.4, 0.5, 0.4);
	gl.uniform3f(kSUniform, 0.04, 0.7, 0.04);	
	gl.uniform1f(kShininessUniform, 0.078125 * 128.0);	

	gl.viewport(vWidth / 3, 0, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 4th sphere on 4th row, red *****

	gl.uniform3f(kAUniform, 0.05, 0.0, 0.0);
	gl.uniform3f(kDUniform, 0.5, 0.4, 0.4);
	gl.uniform3f(kSUniform, 0.7, 0.04, 0.04);	
	gl.uniform1f(kShininessUniform, 0.078125 * 128.0);	

	gl.viewport(vWidth / 2, 0, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 5th sphere on 4th row, white *****

	gl.uniform3f(kAUniform, 0.05, 0.05, 0.05);
	gl.uniform3f(kDUniform, 0.5, 0.5, 0.5);
	gl.uniform3f(kSUniform, 0.7, 0.7, 0.7);	
	gl.uniform1f(kShininessUniform, 0.078125 * 128.0);	

	gl.viewport(2 * vWidth / 3, 0, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();


	// ***** 6th sphere on 4th row, yellow rubber *****

	gl.uniform3f(kAUniform, 0.05, 0.05, 0.0);
	gl.uniform3f(kDUniform, 0.5, 0.5, 0.0);
	gl.uniform3f(kSUniform, 0.7, 0.7, 0.04);	
	gl.uniform1f(kShininessUniform, 0.078125 * 128.0);	

	gl.viewport(5 * vWidth / 6, 0, parseInt(canvas.width) / 6, parseInt(canvas.height) / 4);

	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(modelMatrix, modelMatrix, [sphereScale, sphereScale, sphereScale]);

	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	sphere.draw();



}

function degToRad(degrees)
{
	return(degrees * Math.PI / 180.0);
}

function update()
{
	if (keyPressed == 1)
	{
		angleForXRotation += 0.01;
	}
	else if (keyPressed == 2)
	{
		angleForYRotation += 0.01;
	}
	else if (keyPressed == 3)
	{
		angleForZRotation += 0.01;
	}
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

		case 88:
			keyPressed = 1;
			angleForXRotation = 0.0;
			break;

		case 89:
			keyPressed = 2;
			angleForYRotation = 0.0;
			break;

		case 90:
			keyPressed = 3;
			angleForZRotation = 0.0;
			break;

	}
}

function mouseDown()
{
	//code	
}




