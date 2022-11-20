#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Parser parser;
    parser.readGrammarYACC("./asset/grammar_yacc.txt");
	parser.printGrammar("./asset/printed_grammar.txt");

    parser.calFirstVN();
	parser.printVNFirst("./asset/first.txt");

    parser.constructCluster();
    parser.printCluster("./asset/cluster.txt");
    if (0) {
        parser.readGrammar("./asset/grammar.txt");
        //parser.calFirst();
        parser.calFirstVN();
        parser.printVNFirst("./asset/firstVN.txt");
        parser.constructCluster();
        parser.printCluster("./asset/cluster.txt");
    }
    return a.exec();
}
