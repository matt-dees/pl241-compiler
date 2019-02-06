CS241 Half-Time Project Report
==============================

*Team members*: Matt Dees, Fabian Parzefall

We have completed the first step and most of the second step of the project thus far.
This means we have a parser that generates a workable, semi-optimized intermediate representation in SSA form.

We verified the correctness of our Phi placement using test case 024 as it seemed to involve the most complicated nesting of while and if statements.
In order to generate correct Phi instructions we built a dominator tree and used the dominance frontier to guide the Phi placement.
This eliminated the need to constantly iterate over the CFG to "patch" up nested Phi instructions.

We have also completed the copy propagation and dead code elimination optimizations on our IR.
We are visualizing the intermediate representation using the VCG file format as recommended in the project
documentation.
We spent some time getting xvcg to work on Ubuntu to visualize the VCG file, but upon learning about more modern tools (such as yComb) we decided to use those instead.

Our testing has consisted of glancing over the CFG so far, as we plan on constructing an automated unittest suite using the DLX virtual machine.
Since we made the decision to write our code in C++, we ported the emulator over to C++ to easien the integration into our testing framework.

We plan to finish the Common Subexpression Elimination optimization this week, leaving five weeks for register allocation, code generation, instruction scheduling, and testing.
