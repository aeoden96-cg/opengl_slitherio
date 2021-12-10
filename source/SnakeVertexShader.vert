#version 330 core

layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in float vertexColor;

// GOING TO Tessellation shader.
out float tessColor;

// GOING TO Tessellation shader.
out vec3 vCenter;
out float vRadius;


void main(){

    vCenter = vertexPosition_modelspace.xyz ;
    vRadius = vertexPosition_modelspace.w;

    tessColor = vertexColor;

    //gl_Position = vertexPosition_modelspace - vec4(1,1,0,0);

}

