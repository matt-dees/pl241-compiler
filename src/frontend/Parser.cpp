#include "Parser.h"
#include <algorithm>
#include <stdexcept>

using namespace cs241c;

namespace {
using TT = Token::Type;

const char *TokenNames[] = {
    "Eof",   "Main",   "Function", "Procedure", "Call",   "Return", "Var",
    "Array", "Let",    "If",       "Then",      "Else",   "Fi",     "While",
    "Do",    "Od",     "Eq",       "Ne",        "Lt",     "Le",     "Gt",
    "Ge",    "Add",    "Sub",      "Mul",       "Div",    "POpen",  "PClose",
    "BOpen", "BClose", "COpen",    "CClose",    "LArrow", "Period", "Comma",
    "Semi",  "Ident",  "Number",   "Unknown"};

class Parser {
  std::vector<Token>::const_iterator Lookahead;

public:
  explicit Parser(const std::vector<Token> *Tokens)
      : Lookahead(Tokens->begin()) {}

  Computation get() { return pComputation(); }

private:
  Computation pComputation() {
    consume(TT::Main);

    std::vector<std::unique_ptr<Decl>> Globals;
    std::vector<Func> Funcs;

    while (match({TT::Var, TT::Array})) {
      auto Decl = pDecl();
      Globals.insert(Globals.end(), make_move_iterator(Decl.begin()),
                     make_move_iterator(Decl.end()));
    }

    while (match({TT::Function, TT::Procedure})) {
      auto Func = pFunc();
      Funcs.insert(Funcs.end(), make_move_iterator(Func.begin()),
                   make_move_iterator(Func.end()));
    }

    consume(TT::COpen);
    auto StmtSeq = pStmtSeq();
    consume(TT::CClose);
    consume(TT::Period);

    Funcs.push_back({Func::Type::Procedure, "main", {}, {}, move(StmtSeq)});

    return {move(Globals), move(Funcs)};
  }

  std::vector<std::unique_ptr<Decl>> pDecl() { return {}; }
  std::vector<Func> pFunc() { return {}; }
  std::vector<std::unique_ptr<Stmt>> pStmtSeq() { return {}; }

  void advance() {
    if (Lookahead->T != TT::Eof)
      ++Lookahead;
  }

  bool match(TT T) { return match(std::vector<TT>{T}); }

  bool match(const std::vector<TT> &Ts) {
    return std::find(Ts.begin(), Ts.end(), Lookahead->T) != Ts.end();
  }

  void consume(TT T) {
    if (Lookahead->T != T) {
      error(T);
    }
  }

  void error(TT TExpect) {
    throw std::runtime_error(
        std::string("Expected token: ") +
        TokenNames[static_cast<size_t>(TExpect)] +
        ", actual token: " + TokenNames[static_cast<size_t>(Lookahead->T)]);
  }
};
} // namespace

Computation parse(const std::vector<Token> &Tokens) {
  Parser P(&Tokens);
  return P.get();
}
