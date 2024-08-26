#version 450

layout(location = 0) in vec3 fragColorFromVert;

layout(location = 0) out vec4 outColor;

void main(){
    outColor = vec4(fragColorFromVert, 1.0);
}