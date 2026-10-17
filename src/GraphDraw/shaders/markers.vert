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
