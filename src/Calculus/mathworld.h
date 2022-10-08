#pragma once

#include "calculusdefines.h"
#include "treecreator.h"

#include <string>
#include <vector>
#include <unordered_map>

enum ObjectType {FUNCTION, SEQUENCE, PARAMETRIC_EQ, VARIABLE};

class Function;


using ObjectID = unsigned int;

class MathWorld
{
public:
    MathWorld();

protected:
    std::vector<Function> functions;
    std::unordered_map<std::string, ObjectType> objectTypes;
    std::unordered_map<std::string, size_t> objectIDs;
};

