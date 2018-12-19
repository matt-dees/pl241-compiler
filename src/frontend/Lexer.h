#ifndef CS241C_TOKEN_H
#define CS241C_TOKEN_H

#include <optional>
#include <string>
#include <vector>

namespace cs241c {
struct Token {
  enum class Type;

  Type T;
  std::optional<int32_t> I;
  std::optional<std::string> S;

  Token(Type T) : T(T), I{}, S{} {};
  Token(Type T, int32_t I) : T(T), I{I}, S{} {};
  Token(Type T, std::string S) : T(T), I{}, S{move(S)} {};

  enum class Type {
    Eof,

    Main,
    Function,
    Procedure,
    Call,
    Return,

    Var,
    Array,
    Let,

    If,
    Then,
    Else,
    Fi,

    While,
    Do,
    Od,

    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,

    Add,
    Sub,
    Mul,
    Div,

    POpen,
    PClose,
    BOpen,
    BClose,
    COpen,
    CClose,

    LArrow,

    Period,
    Comma,
    Semi,

    Ident,
    Number,

    Unknown
  };
};

std::vector<Token> lex(const std::string &text);
} // namespace cs241c

#endif
