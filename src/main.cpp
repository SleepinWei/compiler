
//#define QT_DEBUG
#ifdef QT_DEBUG
#include <QApplication>
#include "mainwindow.h"
#endif


//#include <QApplication>
//#include "mainwindow.h"
//#include <QCoreApplication>
#include"Parser.h"
#include"Lexer.h"
#include<iostream>
#include<fstream>

#include"Generator.h"

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
#else
    Lexer lex;
    //string s((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    //lex.Preprocess(s);
    lex.analyze("./asset/source.txt");
    lex.print("./asset/lex.txt");
    std::cout << "Lex Analysis Done" << '\n';

    Parser& parser = Parser::GetInstance();
    Generator& generator = Generator::GetInstance();

    GrammarInfo* info = parser.readGrammarY("./asset/c99.txt");
    parser.printGrammar(info,"./asset/printed_grammar.txt");

    parser.calFirstVN(info);
    parser.printVNFirst(info->firstVN,"./asset/firstVN.txt");
    std::cout << "First Done\n";

    DFA dfa = parser.genDFA(info);
    parser.printTable(dfa,"./asset/table.txt");
    std::cout << "Table Done\n";

    auto [tree, ir] = parser.analyze(lex.inputs,"./asset/output.txt",dfa);
    std::cout << "Analyzation Done" << '\n';

    generator.output(ir,"./asset/quads.txt");
    std::cout << "Generation Done\n";

    if(ir)
		delete ir; 
    if (tree)
        delete tree;

    return 0;
#endif
}
