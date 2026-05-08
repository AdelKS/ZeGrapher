#version 440

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
  mat4  qt_Matrix;
  float qt_Opacity;
  vec4  markerColor;
  float glSize;
  float radius;
  float strokeWidth;
  int   markerType;
};

// SDF helpers — all in centered, pixel-space sprite coordinates.
//   r = visible half-extent (positive inside, negative outside is what the
//   coverage step expects; SDFs return *negative* inside).

float sdRhombus(vec2 p, float r) {
  return abs(p.x) + abs(p.y) - r;
}

float sdDisc(vec2 p, float r) {
  return length(p) - r;
}

float sdSquare(vec2 p, float r) {
  return max(abs(p.x), abs(p.y)) - r;
}

// Equilateral triangle, apex at the sprite top. 'r' is the circumradius.
// Adapted from iquilezles.org/articles/distfunctions2d (y-up); we flip
// p.y first so apex aligns with gl_PointCoord.y = 0.
float sdTriangle(vec2 p, float r) {
  p.y = -p.y;
  const float k = sqrt(3.0);
  p.x = abs(p.x) - r;
  p.y = p.y + r / k;
  if (p.x + k * p.y > 0.0)
    p = vec2(p.x - k * p.y, -k * p.x - p.y) * 0.5;
  p.x -= clamp(p.x, -2.0 * r, 0.0);
  return -length(p) * sign(p.y);
}

// Two perpendicular rectangles unioned: arms of half-length 'r' and
// half-thickness 'h'. Used for the Cross point style.
float sdCross(vec2 p, float r, float h) {
  vec2 q = abs(p);
  float dh = max(q.x - r, q.y - h);  // horizontal arm
  float dv = max(q.y - r, q.x - h);  // vertical arm
  return min(dh, dv);
}

void main() {
  // gl_PointCoord runs (0,0) top-left to (1,1) bottom-right; scaling by
  // glSize gives centered pixel offsets so the SDFs use the same units
  // as radius/strokeWidth.
  vec2 p = (gl_PointCoord - 0.5) * glSize;

  float sdf;
  switch (markerType) {
    case 1: sdf = sdRhombus (p, radius);         break;
    case 2: sdf = sdDisc  (p, radius);         break;
    case 3: sdf = sdSquare  (p, radius);         break;
    case 4: sdf = sdTriangle(p, radius);         break;
    case 5: sdf = sdCross   (p, radius, strokeWidth);  break;
    default: discard;
  }

  float aa = length(vec2(dFdx(sdf), dFdy(sdf)));
  float coverage = clamp(0.5 - sdf / max(aa, 1e-6), 0.0, 1.0);

  float alpha = coverage * markerColor.a * qt_Opacity;
  fragColor = vec4(markerColor.rgb * alpha, alpha);
}
