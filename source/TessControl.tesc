#version 430 core

//patch out float tcuRadius,tcvRadius,tcuDetail,tcvDetail;
// tcuRadius=R, tcuDetail=podjela za phi, tcvRadius=r, tcvDetail=podjela za theta

patch out float tcuRadius,tcuDetail,tcvDetail;
// tcuRadius=R, tcuDetail=podjela za phi,tcvDetail=podjela za theta

layout (vertices = 1)  out;

void main( )
{
    vec4 pom=gl_in[0].gl_Position;

    tcuRadius = pom.z;
    tcuDetail = 2f;
    tcvDetail = 2f;


    gl_TessLevelOuter[0] = tcvDetail;
    gl_TessLevelOuter[1] = tcuDetail;
    gl_TessLevelOuter[2] = tcvDetail;
    gl_TessLevelOuter[3] = tcuDetail;

    gl_TessLevelInner[0]  = tcuDetail;
    gl_TessLevelInner[1]  = tcvDetail;
}
