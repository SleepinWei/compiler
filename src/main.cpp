#include <QApplication>
#include "mainwindow.h"
#include"Parser.h"
#include"Lexer.h"
#include<iostream>
#include<fstream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
