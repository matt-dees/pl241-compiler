#include "NameGen.h"

using namespace cs241c;
using namespace std;

int NameGen::BasicBlockCounter;
int NameGen::InstructionCounter;

string NameGen::genBasicBlockName() { return string("BB_") + to_string(BasicBlockCounter++); }

int NameGen::genInstructionId() { return ++InstructionCounter; }
