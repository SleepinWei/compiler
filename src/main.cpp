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

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
#else
    Lexer lex;
    ifstream t("./asset/source.txt");
    string s((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    lex.Preprocess(s);
    lex.analyze();
    lex.print("./asset/lex.txt");
    std::cout << "Lex Analysis Done" << '\n';

    Parser parser;
    parser.readGrammar("./asset/grammar.txt");
    parser.printGrammar("./asset/printed_grammar.txt");

    parser.calFirstVN();
    parser.printVNFirst("./asset/firstVN.txt");
    std::cout << "First Done\n";

    parser.constructCluster();
    parser.printCluster("./asset/cluster.txt");
    std::cout << "Cluster Done\n";

    parser.constructTable();
    parser.printTable("./asset/table.txt");
    //parser.constructDFA("./asset/dfa.dot");
    std::cout << "Table Done\n";

    parser.analyze(lex.inputs,"./output.txt");
    parser.printTree("./asset/tree.dot");

    return 0;
#endif
}
