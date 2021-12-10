#version 430 core

//uniform float  uScale;
uniform mat4 MVP;

layout( quads, equal_spacing, ccw)  in;

patch in float tcRadius;
patch in vec3 tcCenter;
patch in float tcColor;

out float fragColor;

const float PI = 3.14159265;

void main( )
{
    fragColor = tcColor;
    vec3 p = gl_in[0].gl_Position.xyz;

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    u= PI * ( u - .5 );
    v = 2. * PI * ( v - .5 );

    //float costheta = cos(phi);
    //xyz *= ( uScale * tcRadius );
    //xyz += tcCenter;

    vec3 xyz = vec3( v*cos(u)*tcRadius, v*sin(u)*tcRadius,0);

    xyz += tcCenter;
    gl_Position = MVP * vec4( xyz,1. );
}
