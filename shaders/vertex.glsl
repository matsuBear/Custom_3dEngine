#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
layout (location =0) in vec4 pos;
layout (location=1) in vec4 normal;
out vec4 vPos;
out vec4 vNormal;

void main(){
    gl_Position= uProjection*uView*uModel*pos;
    vPos=pos;
    vNormal=vec4(normal.xyz,0);
}