#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/Parser.h"
#include "src/Lexer.h"
#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void loadGrammar();
    void loadAsset(QListWidgetItem *item);
    void genAndShowTree();

private:
    Ui::MainWindow *ui;
    Parser *mParser = 0;
    Lexer *lexer = 0;
    QProcess *procImg = 0;
};

#endif // MAINWINDOW_H
