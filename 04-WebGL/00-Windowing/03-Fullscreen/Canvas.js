//CODE

var canvas = null;
var context = null;

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

	//get width and height of canvas
	console.log("canvas width = " +canvas.width +" canvas height = " +canvas.height +"\n");

	//get drawing context from the canvas
	context = canvas.getContext("2d");

	if(!context)
	{
		console.log("Obtaining Context Failed. \n");
	}
	else
	{
		console.log("Obtaining Context Successful. \n");
	}

	//paint background - black
	context.fillStyle = "black";
	context.fillRect(0, 0, canvas.width, canvas.height);

	drawText("Hello World!");

	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);

}


function drawText(text)
{
	context.textAlign = "center";     
	context.textBaseline = "middle";
	context.font = "48px sans-serif";
	context.fillStyle = "white";
	context.fillText(text, canvas.width/2, canvas.height/2);

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
	}

}


function keyDown(event)
{
	switch(event.keyCode)
	{
		case 70:
			toggleFullscreen();
			drawText("Hello World!");
			break;

	}
}


function mouseDown()
{
	//code	
}




