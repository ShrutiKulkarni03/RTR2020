// functions use without 'this' prefix are private to class.
// functions with 'this' prefix are 'public' and hence when they use
function Stack()
{
    var elements=[];
    var top=0;

    this.push=function(matrix)
    {
        // code
        //elements[top] = matrix;
        elements[top] = Object.assign(top, matrix);
        top = top + 1;
    }
    
    this.pop=function()
    {
        top = top - 1;
        return elements.pop();
    }
   
}
