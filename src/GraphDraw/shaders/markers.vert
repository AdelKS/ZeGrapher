#version 440

// One vertex per data point. Style is uniform per draw — sourced from the
// material's std140 block alongside qt_Matrix / qt_Opacity, not from
// per-vertex attributes.
layout(location = 0) in vec2 pos;

layout(std140, binding = 0) uniform buf {
    mat4  qt_Matrix;
    float qt_Opacity;
    vec4  markerColor;
    float glSize;
    float radius;
    float strokeWidth;
    int   markerType;
};

void main() {
    gl_Position  = qt_Matrix * vec4(pos, 0.0, 1.0);
    gl_PointSize = glSize;
}
