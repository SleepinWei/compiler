//#include <QCoreApplication>
#include"Parser.h"

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    return a.exec();
    Parser parser;
    parser.readGrammar("./asset/grammar.txt");
    parser.printGrammar("./asset/printed_grammar.txt");
    //parser.calFirst();
    //parser.printFirst("./asset/first.txt");
    parser.calFirstVN();
    parser.printVNFirst("./asset/firstVN.txt");
    parser.constructCluster();
    parser.printCluster("./asset/cluster.txt");
    return 0;
}
