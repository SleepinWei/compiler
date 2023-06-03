# Yet another compiler

This is a course project for Compilers' Principles, Tongji University.

We aim to make a compiler that translates ansi c codes into x86 assembly.

## Structure

We use lex specification and grammar from this website. [ANSI C Yacc Grammar](https://www.lysator.liu.se/c/ANSI-C-grammar-y.html)

The syntax analysis uses LR1 method.

The semantics analysis follows syntax directed translation and is done during the process of syntax analysis. The intermediate representation we choose is quaternion. 

The code generation process generates x86 assembly code in GAS syntax(AT&T). 
We use gcc-12.2.0-x86 to assemble the generated code.