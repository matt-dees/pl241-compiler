#include "DeadCodeEliminationPass.h"
#include "Filesystem.h"
#include "Lexer.h"
#include "Parser.h"
#include "VcgGen.h"
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

  DeadCodeEliminationPass DCEP;
  DCEP.run(*IR);

  if (GenerateVcg) {
    VcgGen VcgGenerator = VcgGen(IR.get());
    string VcgOutput{string(InputFile) + ".vcg"};
    removeFile(VcgOutput);
    VcgGenerator.generate(VcgOutput);
  }

  return 0;
}
