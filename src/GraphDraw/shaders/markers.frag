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

// from https://iquilezles.org/articles/distfunctions2d/
float sdRhombus(vec2 p, vec2 b)
{
  b.y = -b.y;
  p = abs(p);
  float h = clamp( (dot(b,p)+b.y*b.y)/dot(b,b), 0.0, 1.0 );
  p -= b*vec2(h,h-1.0);
  return length(p)*sign(p.x);
}

float sdDisc(vec2 p, float r) {
  return length(p) - r;
}

// from https://iquilezles.org/articles/distfunctions2d/
float sdBox(vec2 p, vec2 b )
{
  vec2 d = abs(p)-b;
  return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
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

float sdRoundedCross(vec2 p, float w, in float r)
{
  float coef = sqrt(2.0)/2.0;
  p = vec2(p.x * coef - p.y * coef, p.x * coef + p.y * coef);
  p = abs(p);
  return length(p-min(p.x+p.y,w)*0.5) - r;
}

void main() {
  // gl_PointCoord runs (0,0) top-left to (1,1) bottom-right; scaling by
  // glSize gives centered pixel offsets so the SDFs use the same units
  // as radius/strokeWidth.
  vec2 p = (gl_PointCoord - 0.5) * glSize;

  float sdf;
  if      (markerType == 1) sdf = sdRhombus(p, vec2(radius, radius)) - strokeWidth/2.0;
  else if (markerType == 2) sdf = sdDisc(p, radius) - strokeWidth/2.0;
  else if (markerType == 3) sdf = sdBox(p, vec2(radius, radius)) - strokeWidth/2.0;
  else if (markerType == 4) sdf = sdTriangle(p, radius) - strokeWidth/2.0;
  else if (markerType == 5) sdf = sdRoundedCross(p, radius + strokeWidth, strokeWidth/2.0);
  else discard;

  float aa = length(vec2(dFdx(sdf), dFdy(sdf)));
  float coverage = clamp(0.5 - sdf / max(aa, 1e-6), 0.0, 1.0);

  float alpha = coverage * markerColor.a * qt_Opacity;
  fragColor = vec4(markerColor.rgb * alpha, alpha);
}
