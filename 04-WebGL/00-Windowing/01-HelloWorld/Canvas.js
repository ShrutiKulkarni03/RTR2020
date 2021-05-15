function main()
{
	//Get canvas from dom
	var canvas = document.getElementById("SPK");

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
	var context = canvas.getContext("2d");

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

	//centering of the text
	context.textAlign = "center";      //horizontal center
	context.textBaseline = "middle";   //vertical center
	
	//font declaration
	context.font = "48px sans-serif";

	//color of the text
	context.fillStyle = "white";

	//string declaration
	var str = "Hello World!";

	//display the text
	context.fillText(str, canvas.width/2, canvas.height/2);

}

