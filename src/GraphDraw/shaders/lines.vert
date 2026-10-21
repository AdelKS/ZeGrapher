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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
