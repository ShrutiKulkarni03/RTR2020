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

var vao_cube;
var vbo_cube_position;

var mvpUniform;

var perspectiveProjectionMatrix;

var cubeAngle = 0.0;

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
	"uniform mat4 u_mvp_matrix; \n"+
	"void main(void) \n"+
	"{ \n"+
	"	gl_Position = u_mvp_matrix * vPosition; \n"+
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
	"out vec4 FragColor; \n"+
	"void main(void) \n"+
	"{ \n"+
	"	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); \n"+
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

	mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");

	//ARRAYS

	var cubeVertices = new Float32Array([	1.0, 1.0, 1.0,        //front
											-1.0, 1.0, 1.0,
											-1.0, -1.0, 1.0,
											1.0, -1.0, 1.0,
											1.0, 1.0, -1.0,       //right
											1.0, 1.0, 1.0,
											1.0, -1.0, 1.0,
											1.0, -1.0, -1.0,
											-1.0, 1.0, -1.0,       //back
											1.0, 1.0, -1.0,
											1.0, -1.0, -1.0,
											-1.0, -1.0, -1.0,
											-1.0, 1.0, 1.0,        //left
											-1.0, 1.0, -1.0,
											-1.0, -1.0, -1.0,
											-1.0, -1.0, 1.0,
											1.0, 1.0, -1.0,       //top
											-1.0, 1.0, -1.0,
											-1.0, 1.0, 1.0,
											1.0, 1.0, 1.0,
											1.0, -1.0, -1.0,      //bottom
											1.0, -1.0, 1.0,
											-1.0, -1.0, 1.0,
											-1.0, -1.0, -1.0 	]);

	

	//CUBE VAO 

	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);

	vbo_cube_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_position);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
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

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();

	//cube

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -5.0]);

	mat4.scale(modelViewMatrix, modelViewMatrix, [1.0, 1.0, 1.0]);
	
	mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(cubeAngle));

	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(cubeAngle));
	
	mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(cubeAngle));

	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vao_cube);

	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

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
	cubeAngle += 0.5;
	if (cubeAngle >= 360.0)
		cubeAngle = 0.0;

}

function uninitialize()
{
	if(vao_cube)
	{
		gl.deleteVertexArray(vao_cube);
		vao_cube = null;
	}

	if(vbo_cube_position)
	{
		gl.deleteBuffer(vbo_cube_position);
		vbo_cube_position = null;
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

	}
}

function mouseDown()
{
	//code	
}




