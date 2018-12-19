#include "Lexer.h"
#include <stdexcept>
#include <unordered_map>

using namespace cs241c;

namespace {
class Lexer {
  const std::unordered_map<std::string, Token::Type> Keywords{
      {"main", Token::Type::Main},
      {"function", Token::Type::Function},
      {"procedure", Token::Type::Procedure},
      {"call", Token::Type::Call},
      {"return", Token::Type::Return},
      {"var", Token::Type::Var},
      {"array", Token::Type::Array},
      {"let", Token::Type::Let},
      {"if", Token::Type::If},
      {"then", Token::Type::Then},
      {"else", Token::Type::Else},
      {"fi", Token::Type::Fi},
      {"while", Token::Type::While},
      {"do", Token::Type::Do},
      {"od", Token::Type::Od}};

  std::string Text;
  std::string::iterator Pos;
  bool Eof = false;

public:
  explicit Lexer(std::string Text)
      : Text(move(Text)), Pos(this->Text.begin()) {}

  Token next() {
    while (true) {
      while (!Eof && isspace(*Pos)) {
        nextChar();
      }

      if (Eof) {
        return Token::Type::Eof;
      }

      switch (*Pos) {
      case '=': {
        nextChar();
        if (*Pos == '=') {
          nextChar();
          return Token::Type::Eq;
        }
        return Token::Type::Unknown;
      }
      case '!': {
        nextChar();
        if (*Pos == '=') {
          nextChar();
          return Token::Type::Ne;
        }
        return Token::Type::Unknown;
      }
      case '<': {
        nextChar();
        if (*Pos == '=') {
          nextChar();
          return Token::Type::Le;
        } else if (*Pos == '-') {
          nextChar();
          return Token::Type::LArrow;
        }
        return Token::Type::Lt;
      }
      case '>': {
        nextChar();
        if (*Pos == '=') {
          nextChar();
          return Token::Type::Ge;
        }
        return Token::Type::Gt;
      }
      case '+': {
        nextChar();
        return Token::Type::Add;
      }
      case '-': {
        nextChar();
        return Token::Type::Sub;
      }
      case '*': {
        nextChar();
        return Token::Type::Mul;
      }
      case '/': {
        nextChar();
        if (*Pos == '/') {
          while (*Pos != '\n') {
            nextChar();
          }
        } else {
          return Token::Type::Div;
        }
      }
      case '(': {
        nextChar();
        return Token::Type::POpen;
      }
      case ')': {
        nextChar();
        return Token::Type::PClose;
      }
      case '[': {
        nextChar();
        return Token::Type::BOpen;
      }
      case ']': {
        nextChar();
        return Token::Type::BClose;
      }
      case '{': {
        nextChar();
        return Token::Type::COpen;
      }
      case '}': {
        nextChar();
        return Token::Type::CClose;
      }
      case '.': {
        nextChar();
        return Token::Type::Period;
      }
      case ',': {
        nextChar();
        return Token::Type::Comma;
      }
      case ';': {
        nextChar();
        return Token::Type::Semi;
      }
      case '#':
        while (*Pos != '\n') {
          nextChar();
        }
      default: {
        if (isalpha(*Pos)) {
          std::string Ident = lexIdentifier();
          auto Kw = Keywords.find(Ident);
          if (Kw == Keywords.end()) {
            return {Token::Type::Ident, Ident};
          } else {
            return Kw->second;
          }
        } else if (isdigit(*Pos)) {
          int32_t I = lexNumber();
          return {Token::Type::Number, I};
        }
        return Token::Type::Unknown;
      }
      }
    }
  }

private:
  void nextChar() {
    ++Pos;
    if (Pos == Text.end()) {
      Eof = true;
    }
  }

  std::string lexIdentifier() {
    std::string S;
    S += *Pos;
    nextChar();
    while (isalnum(*Pos)) {
      S += *Pos;
      nextChar();
    }
    return S;
  }

  int32_t lexNumber() {
    std::string S;
    S += *Pos;
    nextChar();
    while (isdigit(*Pos)) {
      S += *Pos;
      nextChar();
    }
    return stoi(S);
  }
};
} // namespace

std::vector<Token> cs241c::lex(const std::string &text) {
  Lexer L(text);
  std::vector<Token> Tokens;
  Token T = L.next();
  while (T.T != Token::Type::Eof) {
    if (T.T == Token::Type::Unknown) {
      throw std::runtime_error("Lexer: Unkown symbol.");
    }
    Tokens.push_back(T);
    T = L.next();
  }

  return Tokens;
}
