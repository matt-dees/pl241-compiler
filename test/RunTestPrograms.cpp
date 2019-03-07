#include "CommonSubexElimPass.h"
#include "ConstExprEvalPass.h"
#include "DLXGen.h"
#include "DeadCodeEliminationPass.h"
#include "Filesystem.h"
#include "IntegrityCheckPass.h"
#include "Lexer.h"
#include "Mem2VarPass.h"
#include "ParamPass.h"
#include "Parser.h"
#include "Phi2VarPass.h"
#include "SSAPass.h"
#include "SpillPass.h"
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
      FunctionAnalyzer FA;

      FA.runDominanceAnalytics(IR.get());

      Mem2VarPass M2V(FA);
      M2V.run(*IR);

      SSAPass SSAP(FA);
      SSAP.run(*IR);

      ConstExprEvalPass CEE(FA);
      CEE.run(*IR);

      CommonSubexElimPass CSE(FA);
      CSE.run(*IR);

      DeadCodeEliminationPass DCEP(FA);
      DCEP.run(*IR);
      FA.runDominanceAnalytics(IR.get());

      ParamPass PP(FA);
      PP.run(*IR);

      FA.runRegisterAllocation(IR.get());

      SpillPass SP(FA);
      bool Spilled = true;
      while (Spilled) {
        SP.run(*IR);
        Spilled = SP.SpilledValues;
        FA.runRegisterAllocation(IR.get());
      }

      Phi2VarPass P2V(FA);
      P2V.run(*IR);

      IntegrityCheckPass ICP(FA);
      ICP.run(*IR);

      genDlx(*IR, FA);

      CHECK(true);
    }
  }
}
