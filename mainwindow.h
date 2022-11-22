#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/Parser.h"
#include <QMainWindow>
#include "ui_mainwindow.h"

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

private:
    Ui::MainWindow *ui;
    Parser *mParser = 0;
};

#endif // MAINWINDOW_H
