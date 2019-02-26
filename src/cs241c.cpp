#include "CommonSubexElimPass.h"
#include "ConstExprEvalPass.h"
#include "DLXGen.h"
#include "DeadCodeEliminationPass.h"
#include "Filesystem.h"
#include "IntegrityCheckPass.h"
#include "Lexer.h"
#include "Mem2VarPass.h"
#include "Parser.h"
#include "Phi2VarPass.h"
#include "SSAPass.h"
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
  FunctionAnalyzer FA;

  auto Tokens = lex(Text);
  auto AST = parse(Tokens);
  auto IR = AST.genIr();

  FA.runDominanceAnalytics(IR.get());

  Mem2VarPass M2V(FA);
  M2V.run(*IR);

  SSAPass SSAP(FA);
  SSAP.run(*IR);

  IntegrityCheckPass ICP(FA);
  ICP.run(*IR);

  ConstExprEvalPass CEE(FA);
  CEE.run(*IR);

  ICP.run(*IR);

  CommonSubexElimPass CSE(FA);
  CSE.PrintDebug = true;
  CSE.run(*IR);

  ICP.run(*IR);

  DeadCodeEliminationPass DCEP(FA);
  DCEP.run(*IR);

  ICP.run(*IR);

  FA.runRegisterAllocation(IR.get());

  if (GenerateVcg) {
    for (auto &F : IR->functions()) {
      string IGOutput{string(InputFile) + "." + F->toString() + ".ig.vcg"};
      removeFile(IGOutput);
      AnnotatedIG AIG(FA.interferenceGraph(F.get()), FA.coloring(F.get()));
      AIG.writeToFile(IGOutput);
    }
  }

  Phi2VarPass P2V(FA);
  P2V.run(*IR);

  ICP.run(*IR);

  if (GenerateVcg) {
    string VcgOutput{string(InputFile) + ".vcg"};
    removeFile(VcgOutput);
    IR->writeToFile(VcgOutput);
  }

  auto Object = genDlx(*IR, FA);
  string ObjectFilename = string(InputFile) + ".dlxo";
  ofstream ObjectFile(ObjectFilename, ofstream::binary);
  ObjectFile.write(reinterpret_cast<char *>(Object.data()), Object.size());

  return 0;
}
