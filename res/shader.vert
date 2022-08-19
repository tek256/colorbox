#version 330
layout(location = 0) in vec4 in_vert;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main(){
	gl_Position = proj * view * model * vec4(in_vert.xy, 0.f, 1.f);
}
