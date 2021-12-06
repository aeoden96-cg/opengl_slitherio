#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace; 
//layout(location = 1) in vec4 vertexColor;

// Output data; will be interpolated for each fragment.
//out vec4 fragmentColor;

// Values that stay constant for the whole mesh.
//uniform mat4 MVP;

void main(){

    gl_Position = vertexPosition_modelspace;

    // U fragmentColor samo zapisi primljeni vertexColor; interpoliranu vrijednost te varijable cemo
    // potom primiti u fragmentshaderu pod istim imenom (tamo je moramo tako deklarirati)
    //fragmentColor = vertexColor;
}

