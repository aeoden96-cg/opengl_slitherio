#version 330 core

// interpolirana vrijednost vrijednosti koja je poslana iz vertex shadera
//in vec4 fragmentColor;
in float fragColor;
// Ouput data
out vec4 color;

void main()
{

	color = vec4(1-fragColor /20,0,0,0);

}
