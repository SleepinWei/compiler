#include "mainwindow.h"
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::loadGrammar);
    QObject::connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::loadAsset);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadGrammar()
{
    if (mParser)
        delete mParser;
    mParser = new Parser;

    mParser->readGrammar("./asset/grammar.txt");
    mParser->printGrammar("./asset/printed_grammar.txt");
    mParser->calFirst();
    mParser->printFirst("./asset/first.txt");

    ui->listWidget->clear();
    ui->listWidget->addItem("printed_grammar");
    ui->listWidget->addItem("first");

    ui->textEdit->clear();
}

void MainWindow::loadAsset(QListWidgetItem *item)
{
    ui->textEdit->clear();
    QFile inputFile("./asset/" + item->text() + ".txt");
    if (!inputFile.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&inputFile);
    QString line = in.readAll();
    inputFile.close();

    ui->textEdit->setPlainText(line);
}
