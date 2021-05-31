#version 330 core

in float vcol;
in vec2 UV;

out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
    vec3 ocol = texture( myTextureSampler, UV ).rgb;
    ocol = ocol * vcol;
    color = vec4(ocol,1);
}