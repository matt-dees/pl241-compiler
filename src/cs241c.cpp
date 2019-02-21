#include "CommonSubexElimPass.h"
#include "ConstExprEvalPass.h"
#include "DeadCodeEliminationPass.h"
#include "Filesystem.h"
#include "IntegrityCheckPass.h"
#include "Lexer.h"
#include "Parser.h"
#include "Phi2VarPass.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

using namespace cs241c;
using namespace std;

static void printUsage(const string_view &Executable) { cout << "Usage: " << Executable << " [--vcg] <source>\n"; }

int main(int ArgC, char **ArgV) {
  string_view Executable = ArgV[0];
  vector<string_view> Args(ArgV + 1, ArgV + ArgC);

  if (Args.size() == 0) {
    printUsage(Executable);
    return 0;
  }

  bool GenerateVcg = find(Args.begin(), Args.end(), "--vcg") != Args.end();

  string_view &InputFile = Args.back();
  ifstream FS(InputFile.data());
  string Text(istreambuf_iterator<char>{FS}, {});

  auto Tokens = lex(Text);
  auto AST = parse(Tokens);
  auto IR = AST.genIr();

  IntegrityCheckPass ICP;
  ICP.run(*IR);

  ConstExprEvalPass CEE;
  CEE.run(*IR);

  ICP.run(*IR);

  CommonSubexElimPass CSE;
  CSE.PrintDebug = true;
  CSE.run(*IR);

  ICP.run(*IR);

  DeadCodeEliminationPass DCEP;
  DCEP.run(*IR);

  ICP.run(*IR);

  IR->allocateRegisters();

  if (GenerateVcg) {
    for (auto &F : IR->functions()) {
      string IGOutput{string(InputFile) + "." + F->toString() + ".ig.vcg"};
      removeFile(IGOutput);
      AnnotatedIG AIG(F->interferenceGraph(), F->registerColoring());
      AIG.writeToFile(IGOutput);
    }
  }

  Phi2VarPass P2V;
  P2V.run(*IR);

  ICP.run(*IR);

  if (GenerateVcg) {
    string VcgOutput{string(InputFile) + ".vcg"};
    removeFile(VcgOutput);
    IR->writeToFile(VcgOutput);
  }

  return 0;
}
