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

var CHECK_IMAGE_WIDTH = 64;
var CHECK_IMAGE_HEIGHT = 64;

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao_quad;
var vbo_quad_position;
var vbo_quad_texcoords;

var mvpUniform;
var textureSamplerUniform;
var perspectiveProjectionMatrix;

var quadVertices = new Float32Array(12);

var checkImage = new Uint8Array(CHECK_IMAGE_WIDTH * CHECK_IMAGE_HEIGHT * 4);
var texImage;

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
	"in vec2 vTexcoord; \n"+
	"uniform mat4 u_mvp_matrix; \n"+
	"out vec2 out_texcoord; \n"+
	"void main(void) \n"+
	"{ \n"+
	"	gl_Position = u_mvp_matrix * vPosition; \n"+
	"	out_texcoord = vTexcoord; \n"+
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
	"uniform highp sampler2D u_texture_sampler; \n"+
	"in vec2 out_texcoord; \n"+
	"out vec4 FragColor; \n"+
	"void main(void) \n"+
	"{ \n"+
	"	FragColor = texture(u_texture_sampler, out_texcoord); \n"+
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
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
	textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_texture_sampler");

	//ARRAYS

	var quadTexcoords = new Float32Array([	0.0, 0.0,        //front
											0.0, 1.0,
											1.0, 1.0,
											1.0, 0.0 ]);


	//QUAD VAO 

	vao_quad = gl.createVertexArray();
	gl.bindVertexArray(vao_quad);

	vbo_quad_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_quad_position);
	gl.bufferData(gl.ARRAY_BUFFER, 4 * 3 * 4, gl.DYNAMIC_DRAW);    //4*3*4
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	vbo_quad_texcoords = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_quad_texcoords);
	gl.bufferData(gl.ARRAY_BUFFER, quadTexcoords, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_TEXCOORD);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);


	//depth
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	//gl.enable(gl.CULL_FACE);


	//textures
	loadTex();
	
	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	perspectiveProjectionMatrix = mat4.create();

}

function loadTex()
{

	var i;
	var j;
	var c;

	for(i = 0; i < CHECK_IMAGE_HEIGHT; i++)
	{
		for(j = 0; j < CHECK_IMAGE_WIDTH; j++)
		{
			c = ((i & 8) ^ (j & 8)) * 255;

			checkImage[(i * 64 + j) * 4 + 0] = c;
			checkImage[(i * 64 + j) * 4 + 1] = c;
			checkImage[(i * 64 + j) * 4 + 2] = c;
			checkImage[(i * 64 + j) * 4 + 3] = 255;

		}
	}

	texImage = gl.createTexture();

	gl.bindTexture(gl.TEXTURE_2D, texImage);
	gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, gl.RGBA, gl.UNSIGNED_BYTE, checkImage);
	
	gl.bindTexture(gl.TEXTURE_2D, null);
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

	//QUAD

	modelViewMatrix = mat4.create();
	modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.6]);

	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	//texture
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, texImage);
	gl.uniform1i(textureSamplerUniform, 0);
	

	//bind vao

	gl.bindVertexArray(vao_quad);


	quadVertices = new Float32Array([  -2.0, -1.0,
										0.0, -2.0,
										1.0, 0.0,
										0.0, 1.0,
										0.0, 0.0,
									   -1.0, 0.0  ]);

	

	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_quad_position);
	gl.bufferData(gl.ARRAY_BUFFER, quadVertices, gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);


	quadVertices = new Float32Array([  1.0, -1.0,
										0.0, 1.0,
										1.0, 0.0,
										2.41421, 1.0,
										-1.41421, 2.41421,
									   -1.0, -1.41421  ]);

	

	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_quad_position);
	gl.bufferData(gl.ARRAY_BUFFER, quadVertices, gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

	

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
	//code
}

function uninitialize()
{


	if(vao_quad)
	{
		gl.deleteVertexArray(vao_quad);
		vao_quad = null;
	}

	if(vbo_quad_position)
	{
		gl.deleteBuffer(vbo_quad_position);
		vbo_quad_position = null;
	}

	if(vbo_quad_texcoords)
	{
		gl.deleteBuffer(vbo_quad_texcoords);
		vbo_quad_texcoords = null;
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

		case 49:
		case 97:
			pressedDigit = 1;
			break;

		case 50:
		case 98:
			pressedDigit = 2;
			break;

		case 51:
		case 99:
			pressedDigit = 3;
			break;

		case 52:
		case 100:
			pressedDigit = 4;
			break;

		default:
			pressedDigit = 0;
			break;


	}
}

function mouseDown()
{
	//code	
}




