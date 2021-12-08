#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace; 
layout(location = 1) in float vertexColor;

// Output data; will be interpolated for each fragment.
out float tessColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

// Output data for Tessellation shader.
out vec3 vCenter;
out float vRadius;


void main(){

    vCenter = vertexPosition_modelspace.xyz ;
    vRadius = vertexPosition_modelspace.w;

    tessColor = vertexColor;

    //gl_Position = vertexPosition_modelspace - vec4(1,1,0,0);

}

