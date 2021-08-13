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

var vertexShaderObjectPyramid;
var fragmentShaderObjectPyramid;
var vertexShaderObjectCube;
var fragmentShaderObjectCube;

var shaderProgramObjectPyramid;
var shaderProgramObjectCube;

var vao_pyramid;
var vbo_pyramid_position;
var vbo_pyramid_texcoords;

var vao_cube;
var vbo_cube_position;
var vbo_cube_texcoords;

var mvpMatrixUniformPyramid;
var textureSamplerUniformPyramid;

var mvpMatrixUniformCube;
var textureSamplerUniformCube;

var perspectiveProjectionMatrix;

var pyramidAngle = 0.0;
var cubeAngle = 0.0;

var pyramid_texture;

//framebuffer variables
var fbo;                         //frame buffer object
var textureRenderBuffer; 
var rbo;                        //render buffer object

var fbWidth;
var fbHeight;


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

	/*PYRAMID*/

	//VERTEX SHADER

	var vertexShaderSourceCodePyramid = 
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

	vertexShaderObjectPyramid = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObjectPyramid, vertexShaderSourceCodePyramid);
	gl.compileShader(vertexShaderObjectPyramid);

	if(gl.getShaderParameter(vertexShaderObjectPyramid, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObjectPyramid);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//FRAGMENT SHADER

	var fragmentShaderSourceCodePyramid = 
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

	fragmentShaderObjectPyramid = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObjectPyramid, fragmentShaderSourceCodePyramid);
	gl.compileShader(fragmentShaderObjectPyramid);

	if(gl.getShaderParameter(fragmentShaderObjectPyramid, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObjectPyramid);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}


	//SHADER PROGRAM
	shaderProgramObjectPyramid = gl.createProgram();
	gl.attachShader(shaderProgramObjectPyramid, vertexShaderObjectPyramid);
	gl.attachShader(shaderProgramObjectPyramid, fragmentShaderObjectPyramid);
	
	//BINDING OF SHADER PROGRAM

	gl.bindAttribLocation(shaderProgramObjectPyramid, WebGLMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObjectPyramid, WebGLMacros.SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	gl.linkProgram(shaderProgramObjectPyramid);

	if(!gl.getProgramParameter(shaderProgramObjectPyramid, gl.LINK_STATUS))
	{
		var error = gl.getShaderInfoLog(shaderProgramObjectPyramid);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//GET UNIFORM LOCATION

	mvpMatrixUniformPyramid = gl.getUniformLocation(shaderProgramObjectPyramid, "u_mvp_matrix");
	textureSamplerUniformPyramid = gl.getUniformLocation(shaderProgramObjectPyramid, "u_texture_sampler");



	/*CUBE*/

	//VERTEX SHADER

	var vertexShaderSourceCodeCube = 
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

	vertexShaderObjectCube = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObjectCube, vertexShaderSourceCodeCube);
	gl.compileShader(vertexShaderObjectCube);

	if(gl.getShaderParameter(vertexShaderObjectCube, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObjectCube);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//FRAGMENT SHADER

	var fragmentShaderSourceCodeCube = 
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

	fragmentShaderObjectCube = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObjectCube, fragmentShaderSourceCodeCube);
	gl.compileShader(fragmentShaderObjectCube);

	if(gl.getShaderParameter(fragmentShaderObjectCube, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObjectCube);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}


	//SHADER PROGRAM
	shaderProgramObjectCube = gl.createProgram();
	gl.attachShader(shaderProgramObjectCube, vertexShaderObjectCube);
	gl.attachShader(shaderProgramObjectCube, fragmentShaderObjectCube);
	
	//BINDING OF SHADER PROGRAM

	gl.bindAttribLocation(shaderProgramObjectCube, WebGLMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObjectCube, WebGLMacros.SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	gl.linkProgram(shaderProgramObjectCube);

	if(!gl.getProgramParameter(shaderProgramObjectCube, gl.LINK_STATUS))
	{
		var error = gl.getShaderInfoLog(shaderProgramObjectCube);
		if(error.length > 0)
		{
			alert(error);
			uinitialize();
		}
	}

	//GET UNIFORM LOCATION

	mvpMatrixUniformCube = gl.getUniformLocation(shaderProgramObjectCube, "u_mvp_matrix");
	textureSamplerUniformCube = gl.getUniformLocation(shaderProgramObjectCube, "u_texture_sampler");


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



	var pyramidTexcoords = new Float32Array([	0.5, 1.0,      //front
												0.0, 0.0,
												1.0, 0.0,
												0.5, 1.0,      //right
												1.0, 0.0, 
												0.0, 0.0,
												0.5, 1.0,      //back
												1.0, 0.0,
												0.0, 0.0,
												0.5, 1.0,      //left
												0.0, 0.0,
												1.0, 0.0 	]);



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

	var cubeTexcoords = new Float32Array([	1.0, 1.0,        //front
											0.0, 1.0,
											0.0, 0.0,
											1.0, 0.0,
											0.0, 1.0,        //right
											1.0, 1.0,
											1.0, 0.0,
											0.0, 0.0,
											0.0, 0.0,        //back
											1.0, 0.0,
											1.0, 1.0,
											0.0, 1.0,
											0.0, 1.0,        //left
											1.0, 1.0,
											1.0, 0.0,
											0.0, 0.0,
											1.0, 1.0,        //top
											0.0, 1.0,
											0.0, 0.0,
											1.0, 0.0,
											1.0, 1.0,        //bottom
											0.0, 1.0,
											0.0, 0.0,
											1.0, 0.0 	]);

	

	//PYRAMID VAO 

	vao_pyramid = gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid);

	vbo_pyramid_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_position);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	vbo_pyramid_texcoords = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_texcoords);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidTexcoords, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_TEXCOORD);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

	//CUBE VAO 

	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);

	vbo_cube_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_position);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	vbo_cube_texcoords = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_texcoords);
	gl.bufferData(gl.ARRAY_BUFFER, cubeTexcoords, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_TEXCOORD);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);


	/*FBO*/

	fbo = gl.createFramebuffer();
	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

	textureRenderBuffer = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, textureRenderBuffer);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, canvas_original_width, canvas_original_height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, textureRenderBuffer, 0);

	rbo = gl.createRenderbuffer();
	gl.bindRenderbuffer(gl.RENDERBUFFER, rbo);
	gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, canvas_original_width, canvas_original_height);
	gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, rbo);
	//gl.drawBuffers(gl.COLOR_ATTACHMENT0);

	gl.bindFramebuffer(gl.FRAMEBUFFER, null);

	//depth
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.enable(gl.CULL_FACE);


	//textures

	pyramid_texture = gl.createTexture();
	pyramid_texture.image = new Image();
	pyramid_texture.image.src = "stone.png";
	pyramid_texture.image.onload = function()
	{
		gl.bindTexture(gl.TEXTURE_2D, pyramid_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, pyramid_texture.image);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}

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

	fbWidth = canvas.width;
	fbHeight = canvas.height;

	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
		
}

function draw()
{
	//bind fbo
	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

	gl.clearColor(0.2, 0.2, 0.2, 1.0);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.viewport(0, 0, canvas_original_width, canvas_original_height);

	gl.useProgram(shaderProgramObjectPyramid);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();

	//pyramid

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -2.0]);
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(pyramidAngle));

	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	
	gl.uniformMatrix4fv(mvpMatrixUniformPyramid, false, modelViewProjectionMatrix);

	//texture

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, pyramid_texture);
	gl.uniform1i(textureSamplerUniformPyramid, 0);

	//bind vao

	gl.bindVertexArray(vao_pyramid);

	gl.drawArrays(gl.TRIANGLES, 0, 12);

	gl.bindVertexArray(null);

	gl.useProgram(null);

	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	gl.bindFramebuffer(gl.FRAMEBUFFER, null);

	//cube

	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.viewport(0, 0, fbWidth, fbHeight);

	gl.useProgram(shaderProgramObjectCube);

	modelViewMatrix = mat4.create();
	modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
	mat4.scale(modelViewMatrix, modelViewMatrix, [0.5, 0.5, 0.5]);
	mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(cubeAngle));
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(cubeAngle));
	mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(cubeAngle));

	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	
	gl.uniformMatrix4fv(mvpMatrixUniformCube, false, modelViewProjectionMatrix);

	//texture

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, textureRenderBuffer);
	gl.uniform1i(textureSamplerUniformCube, 0);

	//bind vao

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
	pyramidAngle += 0.5;
	if (pyramidAngle >= 360.0)
		pyramidAngle = 0.0;

	cubeAngle += 0.5;
	if (cubeAngle >= 360.0)
		cubeAngle = 0.0;

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

	if(vbo_pyramid_texcoords)
	{
		gl.deleteBuffer(vbo_pyramid_texcoords);
		vbo_pyramid_texcoords = null;
	}


	
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

	if(vbo_cube_texcoords)
	{
		gl.deleteBuffer(vbo_cube_texcoords);
		vbo_cube_texcoords = null;
	}

	gl.deleteFramebuffer(fbo);

	gl.deleteTexture(pyramid_texture);

	if(shaderProgramObjectPyramid)
	{
		if(fragmentShaderObjectPyramid)
		{
			gl.detachShader(shaderProgramObjectPyramid, fragmentShaderObjectPyramid);
			gl.deleteShader(fragmentShaderObjectPyramid);
			fragmentShaderObjectPyramid = null;
		}

		if(vertexShaderObjectPyramid)
		{
			gl.detachShader(shaderProgramObjectPyramid, vertexShaderObjectPyramid);
			gl.deleteShader(vertexShaderObjectPyramid);
			vertexShaderObjectPyramid = null;
		}

		gl.deleteProgram(shaderProgramObjectPyramid);
		shaderProgramObjectPyramid = null;
	}

	if(shaderProgramObjectCube)
	{
		if(fragmentShaderObjectCube)
		{
			gl.detachShader(shaderProgramObjectCube, fragmentShaderObjectCube);
			gl.deleteShader(fragmentShaderObjectCube);
			fragmentShaderObjectCube = null;
		}

		if(vertexShaderObjectCube)
		{
			gl.detachShader(shaderProgramObjectCube, vertexShaderObjectCube);
			gl.deleteShader(vertexShaderObjectCube);
			vertexShaderObjectCube = null;
		}

		gl.deleteProgram(shaderProgramObjectCube);
		shaderProgramObjectCube = null;
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




