/****************************************************************************
**  Copyright (c) 2021, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "treecreator.h"

#include <cmath>
#include <unordered_map>


std::unique_ptr<FastTree> TreeCreator::getTreeFromExpr(std::string_view expr)
{
    std::vector<ExpressionNode> decomposition = check_decompose(expr);
    return createFastTree(decomposition.begin(), decomposition.end());
}

std::vector<ExpressionNode> TreeCreator::check_decompose(const std::string_view formula)
{
    std::vector<ExpressionNode> decomposition;

    if(formula.empty())
        throw make_pair(std::string("Empty expression"), formula);

    std::vector<std::string_view> split;

    std::string_view formula_copy(formula);

    while(not formula_copy.empty())
    {
        auto pos = formula_copy.find_first_of("()+-*/^", 7);

        if(pos != std::string_view::npos)
        {
            split.push_back(formula_copy.substr(0, pos));
            split.push_back(formula_copy.substr(pos+1, 1));
            formula_copy.remove_prefix(pos+1);
        }
        else split.push_back(std::move(formula_copy));
    }

    auto handle_number = [&](std::string_view view, bool& success)
    {
        try
        {
            success = true;
            return std::stod(std::string(view));
        }
        catch(...)
        {
            success = false;
            return std::nan("");
        }
    };

    bool digit = true, openingParenthesis = true, numberSign = true, varOrFunc = true, canEnd = false,
         ope = false, closingParenthesis = false;

    short pth = 0;

    static const std::unordered_map<std::string, NodeType> operators =
    {
        {"+", NodeType::PLUS},
        {"-", NodeType::MINUS},
        {"*", NodeType::MULTIPLY},
        {"/", NodeType::DIVIDE},
        {"^", NodeType::POWER}
    };

    const size_t size = split.size();
    for(size_t i = 0 ; i < size ; i++)
    {
        // the current substring starts with a digit, so it's a number
        if(std::isdigit(split[i][0]))
        {
            if(not digit)
                throw make_pair(std::string("Unexpected number at this position"), split[i]);

            double val = 0;
            bool success = false;

            // test for 1.23E+45, the split should be something like ["1.23E", "+", "45"]
            // this should be tried first
            if(i+2 < size and
               (split[i+1] == "+" or split[i+1] == "-") and
               std::isdigit(split[i+2][0]))
            {
                val = handle_number(std::string_view(split[i].begin(), split[i+2].end()), success);
            }

            if(not success)
            {
                val = handle_number(split[i], success);
            }

            if(success)
            {
                decomposition.push_back(ExpressionNode{.type = NodeType::NUMBER, .desc = ExpressionNode::NodeValue {.value = val}});
                digit = false, openingParenthesis = false, numberSign = false, varOrFunc = false, canEnd = true,
                ope = true, closingParenthesis = true;
            }
            else throw make_pair(std::string("Could not parse number"), split[i]);
        }

        else if(operators.contains(std::string(split[i])))
        {
            if(not ope)
                throw make_pair(std::string("Unexpected operator"), split[i]);

            decomposition.push_back(ExpressionNode{.type = operators.at(std::string(split[i]))});
        }
        else if(split[i] == "(")
        {
            if(not openingParenthesis)
                throw make_pair(std::string("Unexpected opening parenthesis"), split[i]);
            pth++;

            decomposition.push_back(ExpressionNode{.type = NodeType::OPEN_PTH});

            numberSign = digit = varOrFunc = openingParenthesis = true;
            ope = closingParenthesis = canEnd = false;
        }
        else if(split[i] == ")")
        {
            if(not (closingParenthesis && pth > 0))
                throw make_pair(std::string("Unexpected closing parenthesis"), split[i]);

            pth--;

            decomposition.push_back(ExpressionNode{.type = NodeType::CLOSED_PTH});

            ope = closingParenthesis = canEnd = true;
            digit = numberSign = openingParenthesis = varOrFunc = false;

        }
    }

    if(pth != 0 || not canEnd)
        throw make_pair(std::string("Forgotten closing parentheses"), formula);

    return decomposition;
}


std::unique_ptr<FastTree> TreeCreator::createFastTree(
        std::vector<ExpressionNode>::const_iterator begin,
        std::vector<ExpressionNode>::const_iterator end
        )
{
    std::unique_ptr<FastTree> tree = std::make_unique<FastTree>();

    short pths = 0, closingPthPos = 0, openingPthPos = 0;
    bool debutPthFerme = false;

    if(begin + 1 == end)
    {
        const ExpressionNode &node = *begin;
        if(node.type == NodeType::NUMBER)
        {
            tree->type = NUMBER;
            tree->value = decompValues[start];
        }
        else tree->type = decompTypes[start];

        return tree;
    }

    for(char op = 0; op < 5; op++)
    {
        for(short i = start ; i >= end ; i--)
        {
            if(decompPriorites[i] == PTHF)
            {
                if(!debutPthFerme)
                {
                    debutPthFerme = true;
                    closingPthPos = i - 1;
                }
                pths--;
            }
            else if(decompPriorites[i] == PTHO)
            {
                pths++;
                if(pths == 0)
                {
                    openingPthPos = i + 1;
                    if(op == PTHO)
                    {
                        return createFastTree(closingPthPos, openingPthPos);
                    }
                }
            }
            else if(pths == 0 && decompPriorites[i] == op)
            {
                tree->type = decompTypes[i];
                tree->right = createFastTree(start, i + 1);
                if(op != FUNC)
                    tree->left = createFastTree(i - 1, end);
                return tree;
            }
        }
    }
    return tree;
}
