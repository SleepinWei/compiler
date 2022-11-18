//#include <QCoreApplication>
#include"Parser.h"

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    return a.exec();
    Parser parser;
    parser.readGrammar("./asset/grammar.txt");
    parser.printGrammar("./asset/printed_grammar.txt");
    return 0;
}
