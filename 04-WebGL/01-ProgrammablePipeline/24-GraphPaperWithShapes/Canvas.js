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

var vao_line;
var vbo_line_position;

var vao_triangle;
var vbo_triangle_position;

var vao_quad;
var vbo_quad_position;

var vao_incircle;
var vbo_incircle_position;

var vao_excircle;
var vbo_excircle_position;

var numVertices = 0;

var mvpUniform;

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
	"in vec4 vPosition; \n"+
	"in vec4 vColor; \n"+
	"uniform mat4 u_mvp_matrix; \n"+
	"out vec4 out_color; \n"+
	"void main(void) \n"+
	"{ \n"+
	"	gl_Position = u_mvp_matrix * vPosition; \n"+
	"	out_color = vColor; \n"+
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
	"in vec4 out_color; \n"+
	"out vec4 FragColor; \n"+
	"void main(void) \n"+
	"{ \n"+
	"	FragColor = out_color; \n"+
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
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SPK_ATTRIBUTE_COLOR, "vColor");

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

	//incircle
	var x1 = 0.0;
	var x2 = -0.5;
	var x3 = 0.5;

	var y1 = 0.5;
	var y2 = -0.5;
	var y3 = -0.5;

	var in_x, in_y;

	var da, db, dc, a, b, c, in_r, value;
	var s;

	//excircle
	var rect_x1 = -0.5;
	var rect_x2 = 0.5;
	var rect_y1 = 0.5;
	var rect_y2 = -0.5;

	var ex_r, diagonal;

	//calculations for in-circle
	da = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3));
	a = Math.sqrt(da);

	db = ((x3 - x1) * (x3 - x1)) + ((y3 - y1) * (y3 - y1));
	b = Math.sqrt(db);

	dc = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
	c = Math.sqrt(dc);

	in_x = ((a * x1) + (b * x2) + (c * x3)) / (a + b + c);
	in_y = ((a * y1) + (b * y2) + (c * y3)) / (a + b + c);

	s = (a + b + c) / 2;

	value = s * (s - a) * (s - b) * (s - c);

	in_r = (Math.sqrt(value)) / s;


	//calculations for excircle
	diagonal = Math.sqrt(((rect_x2 - rect_x1) * (rect_x2 - rect_x1)) + ((rect_y2 - rect_y1) * (rect_y2 - rect_y1)));

	ex_r = diagonal / 2;

	var in_verts = [];
	var ex_verts = [];

	for (var angle = 0.0; angle <= (2 * Math.PI); angle += 0.1)
	{

		in_verts.push(in_r * Math.cos(angle) + in_x);
		in_verts.push(in_r * Math.sin(angle) + in_y);
		in_verts.push(in_r * 0.0);

		ex_verts.push(ex_r * Math.cos(angle));
		ex_verts.push(ex_r * Math.sin(angle));
		ex_verts.push(ex_r * 0.0);

		numVertices++;

		//var x = in_x + in_r * Math.cos(angle);
		//var y = in_y + in_r * Math.sin(angle);
		//var z = 0.0;

		//verts.push(x, y, z);
	}

	var incircleVertices = new Float32Array(in_verts);

	var excircleVertices = new Float32Array(ex_verts);

	var lineVertices = new Float32Array([   0.0, 1.0, 0.0,
											0.0, -1.0, 0.0]);

	var triangleVertices = new Float32Array([	0.0, 0.5, 0.0,
												-0.5, -0.5, 0.0,
												0.5, -0.5, 0.0	]);


	var quadVertices = new Float32Array([	0.5, 0.5, 0.0,
											-0.5, 0.5, 0.0,
											-0.5, -0.5, 0.0,
											0.5, -0.5, 0.0	]);


	//LINE VAO 

	vao_line = gl.createVertexArray();
	gl.bindVertexArray(vao_line);

	vbo_line_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_line_position);
	gl.bufferData(gl.ARRAY_BUFFER, lineVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);

	gl.bindVertexArray(null);


	//TRIANGLE VAO 

	vao_triangle = gl.createVertexArray();
	gl.bindVertexArray(vao_triangle);

	vbo_triangle_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_triangle_position);
	gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0);

	gl.bindVertexArray(null);


	//QUAD VAO

	vao_quad = gl.createVertexArray();
	gl.bindVertexArray(vao_quad);

	vbo_quad_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_quad_position);
	gl.bufferData(gl.ARRAY_BUFFER, quadVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0);

	gl.bindVertexArray(null);

	//INCIRCLE

	vao_incircle = gl.createVertexArray();
	gl.bindVertexArray(vao_incircle);

	vbo_incircle_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_incircle_position);
	gl.bufferData(gl.ARRAY_BUFFER, incircleVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0);

	gl.bindVertexArray(null);

	//EXCIRCLE

	vao_excircle = gl.createVertexArray();
	gl.bindVertexArray(vao_excircle);

	vbo_excircle_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_excircle_position);
	gl.bufferData(gl.ARRAY_BUFFER, excircleVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0);

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
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.useProgram(shaderProgramObject);

	//LINES
	for (var x = -1.0; x <= 1.05; x += 0.05)
	{
		var modelViewMatrix = mat4.create();
		var modelViewProjectionMatrix = mat4.create();

		mat4.translate(modelViewMatrix, modelViewMatrix, [x, 0.0, -3.0]);
		mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
		gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

		gl.bindVertexArray(vao_line);

		gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
		gl.drawArrays(gl.LINES, 0, 2);
		gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0);

		gl.bindVertexArray(null);
	}
		

	for (var y = -1.0; y <= 1.05; y += 0.05)
	{
		modelViewMatrix = mat4.create();
		modelViewProjectionMatrix = mat4.create();

		mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, y, -3.0]);
		mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(90.0));
		mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
		gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

		gl.bindVertexArray(vao_line);

		gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
		gl.drawArrays(gl.LINES, 0, 2);
		gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0);

		gl.bindVertexArray(null);
	}


	//X axis
	modelViewMatrix = mat4.create();
	modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
	mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(90.0));
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vao_line);

	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 0.0, 1.0);
	gl.drawArrays(gl.LINES, 0, 2);
	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0);

	gl.bindVertexArray(null);

	//Y axis
	modelViewMatrix = mat4.create();
	modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vao_line);

	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 0.0, 1.0, 1.0);
	gl.drawArrays(gl.LINES, 0, 2);
	gl.vertexAttrib3f(WebGLMacros.SPK_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0);

	gl.bindVertexArray(null);

	//triangle
	modelViewMatrix = mat4.create();
	modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vao_triangle);

	gl.drawArrays(gl.LINE_LOOP, 0, 3);

	gl.bindVertexArray(null);

	//quad
	modelViewMatrix = mat4.create();
	modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vao_quad);

	gl.drawArrays(gl.LINE_LOOP, 0, 4);

	gl.bindVertexArray(null);

	//incircle
	modelViewMatrix = mat4.create();
	modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vao_incircle);

	gl.drawArrays(gl.LINE_LOOP, 0, numVertices);

	gl.bindVertexArray(null);

	//excircle
	modelViewMatrix = mat4.create();
	modelViewProjectionMatrix = mat4.create();

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	gl.bindVertexArray(vao_excircle);

	gl.drawArrays(gl.LINE_LOOP, 0, numVertices);

	gl.bindVertexArray(null);


	gl.useProgram(null);

	requestAnimationFrame(draw);
}

function degToRad(degrees)
{
	return (degrees * Math.PI / 180.0);
}

function uninitialize()
{
	if (vao_line) {
		gl.deleteVertexArray(vao_line);
		vao_line = null;
	}

	if (vbo_line_position) {
		gl.deleteBuffer(vbo_line_position);
		vbo_line_position = null;
	}

	if (vao_triangle)
	{
		gl.deleteVertexArray(vao_triangle);
		vao_triangle = null;
	}

	if(vbo_triangle_position)
	{
		gl.deleteBuffer(vbo_triangle_position);
		vbo_triangle_position = null;
	}

	if(vao_quad)
	{
		gl.deleteVertexArray(vao_quad);
		vao_quad = null;
	}

	if(vbo__quad_position)
	{
		gl.deleteBuffer(vbo_quad_position);
		vbo_quad_position = null;
	}

	if (vao_incircle) {
		gl.deleteVertexArray(vao_incircle);
		vao_incircle = null;
	}

	if (vbo_incircle_position) {
		gl.deleteBuffer(vbo_incircle_position);
		vbo_incircle_position = null;
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




