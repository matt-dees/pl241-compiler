#include "DeadCodeEliminationPass.h"
#include "Filesystem.h"
#include "IntegrityCheckPass.h"
#include "Lexer.h"
#include "Parser.h"
#include <algorithm>
#include <catch.hpp>
#include <fstream>

using namespace cs241c;
using namespace std;

TEST_CASE("Compile and run test programs") {
  auto Programs = listFiles(CS241C_TEST_PROGRAMS_DIR);
  for (auto &P : Programs) {
    if (P.rfind(".txt") != P.length() - 4) {
      continue;
    }
    DYNAMIC_SECTION("Compile " << P) {
      ifstream FS(string(CS241C_TEST_PROGRAMS_DIR) + "/" + P);
      string Text(istreambuf_iterator<char>{FS}, {});

      vector<Token> Tokens = lex(Text);
      Computation AST = parse(Tokens);
      auto IR = AST.genIr();

      DeadCodeEliminationPass DCEP;
      DCEP.run(*IR);

      IntegrityCheckPass ICP;
      ICP.run(*IR);

      CHECK(true);
    }
  }
}
