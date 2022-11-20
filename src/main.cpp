//#include <QCoreApplication>
#include"Parser.h"
#include<iostream>

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    return a.exec();
    Parser parser;
    //parser.readGrammarYACC("./asset/grammar_yacc.txt");
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
    std::cout << "Table Done\n";


    return 0;
}
