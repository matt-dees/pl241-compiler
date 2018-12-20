#include "Parser.h"
#include <algorithm>
#include <cassert>
#include <functional>
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
      Funcs.push_back(std::move(Func));
    }

    consume(TT::COpen);
    auto StmtSeq = pStmtSeq();
    consume(TT::CClose);
    consume(TT::Period);

    Funcs.push_back({Func::Type::Procedure, "main", {}, {}, move(StmtSeq)});

    return {move(Globals), move(Funcs)};
  }

  std::vector<std::unique_ptr<Decl>> pDecl() {
    auto DeclGen = pDeclType();

    std::vector<std::unique_ptr<Decl>> Decls;

    std::string Ident = consumeIdent();
    auto Decl = DeclGen(Ident);
    Decls.push_back(move(Decl));

    while (match(TT::Comma)) {
      consume(TT::Comma);
      Ident = consumeIdent();
      Decl = DeclGen(Ident);
      Decls.push_back(move(Decl));
    }

    consume(TT::Semi);

    return Decls;
  }

  Func pFunc() {
    Func::Type FT = Lookahead->T == TT::Function ? Func::Type::Function
                                                 : Func::Type::Procedure;

    consume({TT::Function, TT::Procedure});
    auto Ident = consumeIdent();

    std::vector<std::string> Params;
    if (match(TT::POpen)) {
      Params = pParams();
    }

    consume(TT::Semi);
    auto [Vars, StmtSeq] = pBody();
    consume(TT::Semi);

    return {FT, Ident, Params, move(Vars), move(StmtSeq)};
  }

  std::vector<std::unique_ptr<Stmt>> pStmtSeq() {
    std::vector<std::unique_ptr<Stmt>> StmtSeq;

    StmtSeq.push_back(pStmt());
    while (match(TT::Semi)) {
      consume(TT::Semi);
      StmtSeq.push_back(pStmt());
    }

    return StmtSeq;
  }

  std::function<std::unique_ptr<Decl>(const std::string &)> pDeclType() {
    if (match(TT::Var)) {
      consume(TT::Var);
      return [](const std::string &Ident) {
        return std::make_unique<Decl>(IntDecl(Ident));
      };
    } else {
      consume(TT::Array);

      std::vector<int32_t> Dim;

      consume(TT::BOpen);
      Dim.push_back(consumeNumber());
      consume(TT::BClose);

      while (match(TT::BOpen)) {
        consume(TT::BOpen);
        Dim.push_back(consumeNumber());
        consume(TT::BClose);
      }

      return [Dim](const std::string &Ident) {
        return std::make_unique<Decl>(ArrayDecl(Ident, Dim));
      };
    }
  }

  std::vector<std::string> pParams() {
    consume(TT::POpen);

    std::vector<std::string> Params;

    if (match(TT::Ident)) {
      Params.push_back(consumeIdent());
    }

    while (match(TT::Comma)) {
      consume(TT::Comma);
      Params.push_back(consumeIdent());
    }

    consume(TT::PClose);

    return Params;
  }

  std::tuple<std::vector<std::unique_ptr<Decl>>,
             std::vector<std::unique_ptr<Stmt>>>
  pBody() {
    std::vector<std::unique_ptr<Decl>> Locals;
    while (match({TT::Var, TT::Array})) {
      auto Decl = pDecl();
      Locals.insert(Locals.end(), make_move_iterator(Decl.begin()),
                    make_move_iterator(Decl.end()));
    }

    consume(TT::COpen);
    if (!match({TT::Let, TT::Call, TT::If, TT::While, TT::Return})) {
      throw std::runtime_error("Parser: Expected statement.");
    }
    auto StmtSeq = pStmtSeq();
    consume(TT::CClose);

    return {move(Locals), move(StmtSeq)};
  }

  std::unique_ptr<Stmt> pStmt() {
    switch (Lookahead->T) {
    case TT::Let:
      return pAssignment();
    case TT::Call:
      return std::make_unique<FunctionCallStmt>(std::move(*pFuncCall()));
    case TT::If:
      return pIfStmt();
    case TT::While:
      return pWhileStmt();
    case TT::Return:
      return pReturnStmt();
    default:
      throw std::runtime_error("Parser: Expected statement.");
    }
  }

  std::unique_ptr<Assignment> pAssignment() {
    consume(TT::Let);
    auto Lhs = pDesignator();
    consume(TT::LArrow);
    auto Rhs = pExpr();
    return std::make_unique<Assignment>(move(Lhs), move(Rhs));
  }

  std::unique_ptr<IfStmt> pIfStmt() {
    consume(TT::If);
    auto Cond = pRelation();
    consume(TT::Then);
    auto ThenSeq = pStmtSeq();

    std::vector<std::unique_ptr<Stmt>> ElseSeq;
    if (match(TT::Else)) {
      consume(TT::Else);
      ElseSeq = pStmtSeq();
    }

    consume(TT::Fi);

    return std::make_unique<IfStmt>(std::move(Cond), move(ThenSeq),
                                    move(ElseSeq));
  }

  std::unique_ptr<WhileStmt> pWhileStmt() {
    consume(TT::While);
    auto Cond = pRelation();
    consume(TT::Do);
    auto Body = pStmtSeq();
    consume(TT::Od);
    return std::make_unique<WhileStmt>(std::move(Cond), move(Body));
  }

  std::unique_ptr<ReturnStmt> pReturnStmt() {
    consume(TT::Return);
    if (match({TT::Ident, TT::Number, TT::COpen, TT::Call})) {
      return std::make_unique<ReturnStmt>(pExpr());
    } else {
      return std::make_unique<ReturnStmt>(nullptr);
    }
  }

  std::unique_ptr<Designator> pDesignator() {
    std::string Ident = consumeIdent();

    if (!match(TT::BOpen)) {
      return std::make_unique<VarDesignator>(Ident);
    }

    std::vector<std::unique_ptr<Expr>> Indices;
    do {
      consume(TT::BOpen);
      Indices.push_back(pExpr());
      consume(TT::BClose);
    } while (match(TT::BOpen));

    return std::make_unique<ArrayDesignator>(Ident, move(Indices));
  }

  std::unique_ptr<Expr> pExpr() {
    auto Expr = pTerm();

    while (match({TT::Add, TT::Sub})) {
      MathExpr::Operation Op;
      switch (Lookahead->T) {
      case TT::Add:
        Op = MathExpr::Operation::Add;
        break;
      case TT::Sub:
        Op = MathExpr::Operation::Sub;
        break;
      default:
        assert(false);
      }
      consume({TT::Add, TT::Sub});

      auto Right = pTerm();

      Expr = std::make_unique<MathExpr>(Op, move(Expr), move(Right));
    }

    return Expr;
  }

  std::unique_ptr<Expr> pTerm() {
    auto Expr = pFactor();

    while (match({TT::Mul, TT::Div})) {
      MathExpr::Operation Op;
      switch (Lookahead->T) {
      case TT::Mul:
        Op = MathExpr::Operation::Mul;
        break;
      case TT::Div:
        Op = MathExpr::Operation::Div;
        break;
      default:
        assert(false);
      }
      consume({TT::Mul, TT::Div});

      auto Right = pFactor();

      Expr = std::make_unique<MathExpr>(Op, move(Expr), move(Right));
    }

    return Expr;
  }

  std::unique_ptr<Expr> pFactor() {
    switch (Lookahead->T) {
    case TT::Ident:
      return pDesignator();
    case TT::Number:
      return std::make_unique<ConstantExpr>(consumeNumber());
    case TT::POpen: {
      consume(TT::POpen);
      auto Expr = pExpr();
      consume(TT::PClose);
      return Expr;
    }
    case TT::Call:
      return pFuncCall();
    default:
      assert(false);
    }
  }

  std::unique_ptr<FunctionCall> pFuncCall() {
    consume(TT::Call);
    std::string Ident = consumeIdent();

    std::vector<std::unique_ptr<Expr>> Args;
    if (match(TT::POpen)) {
      consume(TT::POpen);
      if (match({TT::Ident, TT::Number, TT::COpen, TT::Call})) {
        Args.push_back(pExpr());
        while (match(TT::Comma)) {
          consume(TT::Comma);
          Args.push_back(pExpr());
        }
      }
      consume(TT::PClose);
    }

    return std::make_unique<FunctionCall>(Ident, move(Args));
  }

  Relation pRelation() {
    auto Left = pExpr();

    Relation::Type T;
    switch (Lookahead->T) {
    case TT::Eq:
      T = Relation::Type::Eq;
      break;
    case TT::Ne:
      T = Relation::Type::Ne;
      break;
    case TT::Lt:
      T = Relation::Type::Lt;
      break;
    case TT::Le:
      T = Relation::Type::Le;
      break;
    case TT::Gt:
      T = Relation::Type::Gt;
      break;
    case TT::Ge:
      T = Relation::Type::Ge;
      break;
    default:
      throw error(TT::Eq);
    }
    consume({TT::Eq, TT::Ne, TT::Lt, TT::Le, TT::Gt, TT::Ge});

    auto Right = pExpr();

    return {T, move(Left), move(Right)};
  }

  std::string consumeIdent() {
    Token T = *Lookahead;
    consume(TT::Ident);
    return *T.S;
  }

  int32_t consumeNumber() {
    Token T = *Lookahead;
    consume(TT::Number);
    return *T.I;
  }

  void consume(TT T) { consume(std::vector<TT>{T}); }

  void consume(const std::vector<TT> &Ts) {
    if (!match(Ts)) {
      throw error(Ts.front());
    }
    advance();
  }

  void advance() {
    if (Lookahead->T != TT::Eof)
      ++Lookahead;
  }

  bool match(TT T) { return match(std::vector<TT>{T}); }

  bool match(const std::vector<TT> &Ts) {
    return std::find(Ts.begin(), Ts.end(), Lookahead->T) != Ts.end();
  }

  std::runtime_error error(TT TExpect) {
    return std::runtime_error(
        std::string("Parser: Expected token: ") +
        TokenNames[static_cast<size_t>(TExpect)] +
        ", actual token: " + TokenNames[static_cast<size_t>(Lookahead->T)]);
  }
};
} // namespace

Computation cs241c::parse(const std::vector<Token> &Tokens) {
  Parser P(&Tokens);
  return P.get();
}
