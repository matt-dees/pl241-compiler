#include "Lexer.h"
#include "Parser.h"
#include "VcgGen.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

using namespace cs241c;

static void printUsage(const std::string_view &Executable) {
  std::cout << "Usage: " << Executable << " [--vcg] <source>\n";
}

int main(int ArgC, char **ArgV) {
  std::string_view Executable = ArgV[0];
  std::vector<std::string_view> Args(ArgV + 1, ArgV + ArgC);

  if (Args.size() == 0) {
    printUsage(Executable);
    return 0;
  }

  bool GenerateVcg = std::find(Args.begin(), Args.end(), "--vcg") != Args.end();

  std::string_view &InputFile = Args.back();
  std::ifstream FS(InputFile.data());
  std::string Text(std::istreambuf_iterator<char>{FS}, {});

  auto Tokens = lex(Text);
  auto AST = parse(Tokens);
  auto IR = AST.genIr();

  if (GenerateVcg) {
    VcgGen VcgGenerator = VcgGen(IR.get());
    std::string VcgOutput{std::string(InputFile) + ".vcg"};
    std::filesystem::remove(VcgOutput);
    VcgGenerator.generate(VcgOutput);
  }

  return 0;
}
