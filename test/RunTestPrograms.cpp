#include "Lexer.h"
#include <catch.hpp>
#include <filesystem>
#include <fstream>

using namespace cs241c;

TEST_CASE("Compile and run test programs") {
  auto Programs = std::filesystem::directory_iterator(CS241C_TEST_PROGRAMS_DIR);
  for (auto &P : Programs) {
    DYNAMIC_SECTION("Compile " << P.path().filename()) {
      std::ifstream FS(P.path().string());
      std::string Text(std::istreambuf_iterator<char>{FS}, {});
      std::vector<Token> Tokens = lex(Text);
      CHECK_FALSE(Tokens.empty());
    }
  }
}
