#include "Lexer.h"
#include <stdexcept>
#include <unordered_map>

using namespace cs241c;
using TT = Token::Type;

namespace {
class Lexer {
  const std::unordered_map<std::string, TT> Keywords{
      {"main", TT::Main},
      {"function", TT::Function},
      {"procedure", TT::Procedure},
      {"call", TT::Call},
      {"return", TT::Return},
      {"var", TT::Var},
      {"array", TT::Array},
      {"let", TT::Let},
      {"if", TT::If},
      {"then", TT::Then},
      {"else", TT::Else},
      {"fi", TT::Fi},
      {"while", TT::While},
      {"do", TT::Do},
      {"od", TT::Od}};

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
        return TT::Eof;
      }

      switch (*Pos) {
      case '=': {
        nextChar();
        if (*Pos == '=') {
          nextChar();
          return TT::Eq;
        }
        return TT::Unknown;
      }
      case '!': {
        nextChar();
        if (*Pos == '=') {
          nextChar();
          return TT::Ne;
        }
        return TT::Unknown;
      }
      case '<': {
        nextChar();
        if (*Pos == '=') {
          nextChar();
          return TT::Le;
        } else if (*Pos == '-') {
          nextChar();
          return TT::LArrow;
        }
        return TT::Lt;
      }
      case '>': {
        nextChar();
        if (*Pos == '=') {
          nextChar();
          return TT::Ge;
        }
        return TT::Gt;
      }
      case '+': {
        nextChar();
        return TT::Add;
      }
      case '-': {
        nextChar();
        return TT::Sub;
      }
      case '*': {
        nextChar();
        return TT::Mul;
      }
      case '/': {
        nextChar();
        if (*Pos == '/') {
          while (*Pos != '\n') {
            nextChar();
          }
          break;
        } else {
          return TT::Div;
        }
      }
      case '(': {
        nextChar();
        return TT::POpen;
      }
      case ')': {
        nextChar();
        return TT::PClose;
      }
      case '[': {
        nextChar();
        return TT::BOpen;
      }
      case ']': {
        nextChar();
        return TT::BClose;
      }
      case '{': {
        nextChar();
        return TT::COpen;
      }
      case '}': {
        nextChar();
        return TT::CClose;
      }
      case '.': {
        nextChar();
        return TT::Period;
      }
      case ',': {
        nextChar();
        return TT::Comma;
      }
      case ';': {
        nextChar();
        return TT::Semi;
      }
      case '#':
        while (*Pos != '\n') {
          nextChar();
        }
        break;
      default: {
        if (isalpha(*Pos)) {
          std::string Ident = lexIdentifier();
          auto Kw = Keywords.find(Ident);
          if (Kw == Keywords.end()) {
            return {TT::Ident, Ident};
          } else {
            return Kw->second;
          }
        } else if (isdigit(*Pos)) {
          int32_t I = lexNumber();
          return {TT::Number, I};
        }
        return TT::Unknown;
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
  do {
    if (T.T == TT::Unknown) {
      throw std::runtime_error("Lexer: Unkown symbol.");
    }
    Tokens.push_back(T);
    T = L.next();
  } while (T.T != TT::Eof);

  return Tokens;
}
