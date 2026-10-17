/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#version 440

// vLocal, in drawing units:
//   x: position along a capsule from its start point, unused in a strip
//   y: signed distance from the center line
//   z: length of the capsule, negative in a strip
//   w: distance travelled along the line, for the dashes
layout(location = 0) in  vec4 vLocal;
layout(location = 0) out vec4 fragColor;

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

// distance along the line from 's' to the nearest dash
float dashDistance(float s)
{
  float t = mod(s, dashPeriod);
  float best = dashPeriod;
  float start = 0.0;
  for (int i = 0; i < dashCount; i += 2)
  {
    float end = start + dashes[i / 4][i % 4];
    // checking the same dash one period before and after handles the wraparound
    for (int k = -1; k <= 1; k++)
    {
      float a = start + float(k) * dashPeriod, b = end + float(k) * dashPeriod;
      best = min(best, max(max(a - t, t - b), 0.0));
    }
    if (i + 1 < dashCount)
      start = end + dashes[(i + 1) / 4][(i + 1) % 4];
  }
  return best;
}

void main() {
  float along = vLocal.z < 0.0 ? 0.0 : max(max(-vLocal.x, vLocal.x - vLocal.z), 0.0);
  if (dashCount > 0)
    along = max(along, dashDistance(vLocal.w));

  float d = length(vec2(along, vLocal.y));
  float coverage = clamp((halfWidth - d) * pxPerUnit + 0.5, 0.0, 1.0) * alphaScale;

  float alpha = coverage * lineColor.a * qt_Opacity;
  fragColor = vec4(lineColor.rgb * alpha, alpha);
}
