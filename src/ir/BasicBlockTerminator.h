#ifndef CS241C_IR_BASICBLOCKTERMINATOR_H
#define CS241C_IR_BASICBLOCKTERMINATOR_H

namespace cs241c {
class BasicBlockTerminator {};

class BranchTerminator : public BasicBlockTerminator {};

class ConditionalBranchTerminator : public BasicBlockTerminator {};

class ReturnTerminator : public BasicBlockTerminator {};

class EndTerminator : public BasicBlockTerminator {};
} // namespace cs241c

#endif
