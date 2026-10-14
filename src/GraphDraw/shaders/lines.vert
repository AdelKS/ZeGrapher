#version 440

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 local;

layout(std140, binding = 0) uniform buf {
    mat4  qt_Matrix;
    float qt_Opacity;
    vec4  lineColor;
    float halfWidth;
    float alphaScale;
    float pxPerUnit;
    int   dashCount;
    float dashPeriod;
    vec4  dashes[2];
};

layout(location = 0) out vec4 vLocal;

void main() {
    vLocal      = local;
    gl_Position = qt_Matrix * vec4(pos, 0.0, 1.0);
}
