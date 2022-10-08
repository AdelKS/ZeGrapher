/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#ifndef TREECREATOR_H
#define TREECREATOR_H

#include <string>
#include <vector>
#include <string_view>

#include "structures.h"
#include "mathworld.h"
#include "calculusdefines.h"

/* TODO: update approach as the following:
    - Parse: aka cut each atom in a formula
    - Evaluate type of atom: separator, number
    - Treat number in a special as to make 1.2E+33 as one atom
    - Check for validity
    - Enable setting custom names for functions and variables
    - Performance improvement: flatten trees
*/

enum struct NodeType
{
    UNDEFINED,

    NUMBER,
    VAR,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,

    POWER,
    FUNC,

    OPEN_PTH,
    CLOSED_PTH
};

struct ExpressionNode
{
    union NodeValue
    {
        double value;
        unsigned long index;
    };

    NodeType type = NodeType::UNDEFINED;
    NodeValue desc = NodeValue {.index = 0};
};

struct FastTree
{
    ExpressionNode root;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
};

class TreeCreator
{
public:
    TreeCreator(MathWorld *world, MathWorld *sub_world = nullptr);

    std::unique_ptr<FastTree> getTreeFromExpr(std::string_view expr);

protected:
    std::vector<std::string_view> split(std::string_view expr);

    std::vector<ExpressionNode> check_decompose(std::string_view formula);
    std::unique_ptr<FastTree> createFastTree(
        std::vector<ExpressionNode>::const_iterator begin,
        std::vector<ExpressionNode>::const_iterator end
    );

    std::vector<ExpressionNode> decomposition;

};

#endif // TREECREATOR_H
