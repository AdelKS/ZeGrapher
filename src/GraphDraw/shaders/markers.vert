#version 440

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

layout(location = 0) out vec2 vCorner;

// Two triangles (CCW) tiling a quad; indexed by gl_VertexIndex % 6.
const vec2 kCorners[6] = vec2[6](
    vec2(-1., -1.), vec2(+1., -1.), vec2(+1., +1.),
    vec2(-1., -1.), vec2(+1., +1.), vec2(-1., +1.)
);

void main() {
    vec2 corner = kCorners[gl_VertexIndex % 6];
    vCorner     = corner;
    gl_Position = qt_Matrix * vec4(pos + corner * (glSize * 0.5), 0.0, 1.0);
}
