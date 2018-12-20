#ifndef CS241C_FRONTEND_PARSER_H
#define CS241C_FRONTEND_PARSER_H

#include "Lexer.h"
#include "ast/Computation.h"
#include <string>

namespace cs241c {
Computation parse(const std::vector<Token> &Tokens);
}

#endif
