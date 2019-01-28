#include "Filesystem.h"
#include "Lexer.h"
#include "Parser.h"
#include <algorithm>
#include <catch.hpp>
#include <fstream>

using namespace cs241c;

TEST_CASE("Compile and run test programs") {
  auto Programs = listFiles(CS241C_TEST_PROGRAMS_DIR);
  for (auto &P : Programs) {
    if (P.rfind(".txt") != P.length() - 4) {
      continue;
    }
    DYNAMIC_SECTION("Compile " << P) {
      std::ifstream FS(std::string(CS241C_TEST_PROGRAMS_DIR) + "/" + P);
      std::string Text(std::istreambuf_iterator<char>{FS}, {});
      std::vector<Token> Tokens = lex(Text);
      Computation Comp = parse(Tokens);
      CHECK(true);
    }
  }
}
