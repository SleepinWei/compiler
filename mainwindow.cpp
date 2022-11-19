#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include "ui/AssetItem.h"

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

    while(ui->listWidget->count()) {
        auto itm = ui->listWidget->item(0);
        ui->listWidget->removeItemWidget(itm);
        delete itm;
    }
    ui->listWidget->addItem(new AssetItem("0.文法产生式", "./asset/printed_grammar.txt"));
    ui->listWidget->addItem(new AssetItem("0.FIRST集", "./asset/first.txt"));
    ui->listWidget->addItem(new AssetItem("0.源程序", "./source.txt"));
    ui->listWidget->addItem(new AssetItem("1.词法分析结果", "./out.txt"));

    ui->textEdit->clear();
}

void MainWindow::loadAsset(QListWidgetItem *item)
{
    AssetItem *itm = (AssetItem*)item;
    ui->textEdit->clear();
    QFile inputFile(itm->fpath);
    if (!inputFile.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&inputFile);
    QString line = in.readAll();
    inputFile.close();

    ui->textEdit->setPlainText(line);
}
