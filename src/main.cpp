#define _CRT_SECURE_NO_WARNINGS
#include"Parser.h"
#include"Lexer.h"
#include"CodeGen.h"
#include<iostream>
#include<fstream>
#include<string>

#include"Generator.h"
#include"Node.h"

//#define GEN_GRAMMAR

int main(int argc, char *argv[])
{
    bool genGrammar = false;
    string filename; 
    for (int i = 1; i < argc; i++) {
        // 解析选项
        if (strcmp(argv[i], "--generate")==0) {
            genGrammar = true;
        }    
    }
    Lexer lex;
    lex.analyze("./asset/source.c");
    lex.print("./asset/lex.txt");
    std::cout << "Lex Analysis Done" << '\n';

    Parser& parser = Parser::GetInstance();
    Generator& generator = Generator::GetInstance();

    GrammarInfo* info = parser.readGrammarY("./asset/c99_backup.txt");

    parser.calFirstVN(info);
    std::cout << "First Done\n";

    DFA dfa; 
    if (genGrammar) {
        dfa = parser.genDFA(info);
        parser.printTable(info, dfa, "./asset/table.txt");
        std::cout << "Table Done\n";
    }
    else {
        dfa = parser.loadTable(info, "./asset/table.txt");
        std::cout << "Read Done\n";
    }

    SyntaxTree* tree = nullptr;
    IR* ir = nullptr;
    // 重定向
    freopen("./asset/log.txt", "w", stderr);
    freopen("./asset/info.txt", "w", stdout);
    //
    auto&& result = parser.analyze(lex.inputs,"./asset/output.txt",dfa);
    tree = std::get<0>(result);
    ir = std::get<1>(result);

    if (tree != nullptr && ir != nullptr) {

        parser.printTree(tree, "./asset/tree.dot");
        std::cout << "Analyzation Done" << '\n';

        generator.postProcess(ir);
        generator.output(ir, "./asset/quads.txt");
        std::cout << "Generation Done\n";

        DestCode code;
        code.fout = ofstream("./asset/codegen.s");
        CodeGen::codeGen(ir, code);
        std::cout << "CodeGen Done\n";

        delete ir;
        delete tree;
    }

    fclose(stderr);
    fclose(stdout);
    return 0;
}
