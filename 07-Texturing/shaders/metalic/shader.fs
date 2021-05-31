#version 330 core

in float vcol;
in vec2 UV;

out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
    vec3 vcol_prim = vec3(1)*vcol;
    vec3 ocol = texture( myTextureSampler, UV ).rgb;
    float I = length(vcol_prim) + length(ocol)*(1.0 - length(vcol_prim));
    ocol = ( (vcol_prim)*length((vcol_prim)) + ocol*length(ocol) * (1.0 - length((vcol_prim))) ) / I;
    color = vec4(ocol,1);
}