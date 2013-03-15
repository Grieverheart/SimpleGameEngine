#ifndef __OBJ_PARSER_H
#define __OBJ_PARSER_H

#include <vector>
#include <string>
#include "vertex.h"

std::vector<Vertex> parseObj(const char* filename, const std::string& shading_model);

#endif