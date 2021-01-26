/*  Since OpenGL 3.3 and higher the version numbers of GLSL match the version of OpenGL. 
 *  We also explicitly mention we're using core profile functionality.
*/
#version 330 core

/*  We declare all the input vertex attributes in the vertex shader with the in keyword. 
 *  Right now we care about position data so we oneed a single vertex attribute. 
 *  GLSL has a vector datatype that contains 1 to 4 floats based on its postfix digit. 
 *  Since each vertex has a 3D coordinate we create a vec3 input variable with the name aPos. 
 *  We also specifically set the location of the input variable via layout (location = 0).
*/

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 offset;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; 

    /*  To set the output of the vertex shader we have to assign the position data to the predefined 
     *  gl_Position variable which is a vec4 behind the scenes. 
     *  At the end of the main function, whatever we set gl_Position to will be used as the output of the vertex shader. 
     *  Since our input is a vector of size 3 we have to cast this to a vector of size 4. 
     *  We can do this by inserting the vec3 values inside the constructor of vec4 and set its w component to 1.0f.
    */
    gl_Position = projection * view * vec4(FragPos + offset, 1.0);
}