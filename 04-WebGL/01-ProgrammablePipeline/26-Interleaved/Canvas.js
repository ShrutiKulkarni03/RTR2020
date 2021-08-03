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

var modelMatrixUniform;
var viewMatrixUnifrom;
var projectionMatrixUniform;

var lKeyPressedUniform;

var lAUniform;
var lDUniform;
var lSUniform;
var lightPositionUniform;

var kShininessUniform;

var textureSamplerUniform;

var bLight;

var vao_cube;
var vbo_cube;
/*var vbo_cube_position;
var vbo_cube_color;
var vbo_cube_normals;
var vbo_cube_texcoords;*/

var perspectiveProjectionMatrix;

var cubeAngle = 0.0;

var cube_texture;

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
	"in vec4 vColor; \n" +
	"in vec2 vTexcoord; \n" +
	"in vec3 vNormal; \n" +
	"out vec4 out_color; \n" +
	"uniform lowp int u_lKeyPressed; \n" +
	"uniform vec4 u_light_position; \n" +
	"uniform mat4 u_projection_matrix; \n" +
	"uniform mat4 u_model_matrix; \n" +
	"uniform mat4 u_view_matrix; \n" +
	"out vec2 out_texcoord; \n" +
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
	"	out_color = vColor; \n" +
	"	out_texcoord = vTexcoord; \n" +
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
			uninitialize();
		}
	}

	//FRAGMENT SHADER

	var fragmentShaderSourceCode = 
	"#version 300 es \n"+
	"\n"+
	"precision highp float; \n" +
	"uniform lowp int u_lKeyPressed; \n" +
	"in vec4 out_color; \n" +
	"in vec3 transformed_normal; \n" +
	"in vec3 light_direction; \n" +
	"in vec3 view_vector; \n" +
	"in vec2 out_texcoord; \n" +
	"uniform sampler2D u_texture_sampler; \n" +
	"uniform vec3 u_lA; \n" +
	"uniform vec3 u_lD; \n" +
	"uniform vec3 u_lS; \n" +
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
	"		vec3 ambient = u_lA * texture(u_texture_sampler, out_texcoord).rgb; \n" +
	"		vec3 diffuse = u_lD * texture(u_texture_sampler, out_texcoord).rgb * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0); \n" +
	"		vec3 specular = u_lS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess); \n" +
	"		phong_ads_light = ambient + diffuse + specular; \n" +
	"	} \n" +
	"	else \n" +
	"	{ \n" +
	"		phong_ads_light = vec3(1.0, 1.0, 1.0); \n" +
	"	} \n" +
	"	FragColor = vec4(phong_ads_light, 1.0f) * out_color; \n" +
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
			uninitialize();
		}
	}


	//SHADER PROGRAM
	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);
	
	//BINDING OF SHADER PROGRAM

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SPK_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SPK_ATTRIBUTE_COLOR, "vColor");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SPK_ATTRIBUTE_NORMAL, "vNormal");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	gl.linkProgram(shaderProgramObject);

	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
	{
		var error = gl.getShaderInfoLog(shaderProgramObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
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

	kShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_materialShininess");

	textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_texture_sampler");

	//ARRAYS
	/*var cubeVertices = new Float32Array([	1.0, 1.0, 1.0,        //front
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
											1.0, 0.0 	]);*/


									     //vertices             //color               //normals             //texcoords
	var cubeVCNT = new Float32Array([	 1.0, 1.0, 1.0,      1.0, 0.0, 0.0,     0.0, 0.0, 1.0,     1.0, 1.0,
										-1.0, 1.0, 1.0,      1.0, 0.0, 0.0,     0.0, 0.0, 1.0,     0.0, 1.0,
										-1.0, -1.0, 1.0,     1.0, 0.0, 0.0,     0.0, 0.0, 1.0,     0.0, 0.0,
										 1.0, -1.0, 1.0,     1.0, 0.0, 0.0,     0.0, 0.0, 1.0,     1.0, 0.0,
										 1.0, 1.0, -1.0,     0.0, 1.0, 0.0,     1.0, 0.0, 0.0,     0.0, 1.0,
										 1.0, 1.0, 1.0,      0.0, 1.0, 0.0,     1.0, 0.0, 0.0,     1.0, 1.0,
										 1.0, -1.0, 1.0,     0.0, 1.0, 0.0,     1.0, 0.0, 0.0,     1.0, 0.0,
										 1.0, -1.0, -1.0,    0.0, 1.0, 0.0,     1.0, 0.0, 0.0,     0.0, 0.0,
										-1.0, 1.0, -1.0,     0.0, 0.0, 1.0,     0.0, 0.0, -1.0,    0.0, 0.0,
										 1.0, 1.0, -1.0,     0.0, 0.0, 1.0,     0.0, 0.0, -1.0,    1.0, 0.0,
										 1.0, -1.0, -1.0,    0.0, 0.0, 1.0,     0.0, 0.0, -1.0,    1.0, 1.0,
										-1.0, -1.0, -1.0,    0.0, 0.0, 1.0,     0.0, 0.0, -1.0,    0.0, 1.0,
										-1.0, 1.0, 1.0,      0.0, 1.0, 1.0,    -1.0, 0.0, 0.0,     0.0, 1.0,
										-1.0, 1.0, -1.0,     0.0, 1.0, 1.0,    -1.0, 0.0, 0.0,     1.0, 1.0,
										-1.0, -1.0, -1.0,    0.0, 1.0, 1.0,    -1.0, 0.0, 0.0,     1.0, 0.0,
										-1.0, -1.0, 1.0,     0.0, 1.0, 1.0,    -1.0, 0.0, 0.0,     0.0, 0.0,
										 1.0, 1.0, -1.0,     1.0, 0.0, 1.0,     0.0, 1.0, 0.0,     1.0, 1.0,
										-1.0, 1.0, -1.0,     1.0, 0.0, 1.0,     0.0, 1.0, 0.0,     0.0, 1.0,
										-1.0, 1.0, 1.0,      1.0, 0.0, 1.0,     0.0, 1.0, 0.0,     0.0, 0.0,
										 1.0, 1.0, 1.0,      1.0, 0.0, 1.0,     0.0, 1.0, 0.0,     1.0, 0.0,
										 1.0, -1.0, -1.0,    1.0, 1.0, 0.0,     0.0, -1.0, 0.0,    1.0, 1.0,
										-1.0, -1.0, -1.0,    1.0, 1.0, 0.0,     0.0, -1.0, 0.0,    0.0, 1.0,
										-1.0, -1.0, 1.0,     1.0, 1.0, 0.0,     0.0, -1.0, 0.0,    0.0, 0.0,
										 1.0, -1.0, 1.0,     1.0, 1.0, 0.0,     0.0, -1.0, 0.0,    1.0, 0.0 	]);



	//CUBE VAO 

	/*vao_cube = gl.createVertexArray();
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

	gl.bindVertexArray(null);*/


	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);

	vbo_cube = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVCNT, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 11 * 4, 0);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_POSITION);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 11 * 4, 3 * 4);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_COLOR);	
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 11 * 4, 6 * 4);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_NORMAL);
	gl.vertexAttribPointer(WebGLMacros.SPK_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 11 * 4, 9 * 4);
	gl.enableVertexAttribArray(WebGLMacros.SPK_ATTRIBUTE_TEXCOORD);
	
	
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);


	//depth
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	//gl.enable(gl.CULL_FACE);


	//textures
	cube_texture = gl.createTexture();
	cube_texture.image = new Image();
	cube_texture.image.src = "marble.png";
	cube_texture.image.onload = function()
	{
		gl.bindTexture(gl.TEXTURE_2D, cube_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, cube_texture.image);
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
		gl.uniform3f(lAUniform, 0.0, 0.0, 0.0);
		gl.uniform3f(lDUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(lSUniform, 1.0, 1.0, 1.0);		
		gl.uniform4f(lightPositionUniform, 100.0, 100.0, 100.0, 1.0);

		gl.uniform1f(kShininessUniform, 50.0);	
	}
	else
	{
		gl.uniform1i(lKeyPressedUniform, 0);
	}

	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	//cube
	
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -4.0]);
	mat4.scale(modelMatrix, modelMatrix, [0.5, 0.5, 0.5]);
	mat4.rotateX(modelMatrix, modelMatrix, degToRad(cubeAngle));
	mat4.rotateY(modelMatrix, modelMatrix, degToRad(cubeAngle));
	mat4.rotateZ(modelMatrix, modelMatrix, degToRad(cubeAngle));

	projectionMatrix = perspectiveProjectionMatrix;
	
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);


	//texture

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, cube_texture);
	gl.uniform1i(textureSamplerUniform, 0);

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

	if(vbo_cube)
	{
		gl.deleteBuffer(vbo_cube);
		vbo_cube = null;
	}

	/*if(vbo_cube_position)
	{
		gl.deleteBuffer(vbo_cube_position);
		vbo_cube_position = null;
	}

	if(vbo_cube_texcoords)
	{
		gl.deleteBuffer(vbo_cube_texcoords);
		vbo_cube_texcoords = null;
	}*/


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




