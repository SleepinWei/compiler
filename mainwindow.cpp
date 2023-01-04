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
    if (lexer)
        delete lexer;
    lexer = new Lexer;

    // Lexer
    ifstream t("./source.txt");
    string s((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    lexer->Preprocess(s);
//    lexer->analyze();
    lexer->print("./asset/lex.txt");
    std::cout << "Lex Analysis Done" << '\n';
    mParser->readGrammar("./grammar.txt");
    mParser->printGrammar("./asset/printed_grammar.txt");

    mParser->calFirstVN();
    mParser->printVNFirst("./asset/firstVN.txt");
    std::cout << "First Done\n";

    mParser->constructCluster();
    mParser->printCluster("./asset/cluster.txt");
    std::cout << "Cluster Done\n";

    mParser->constructTable();
    mParser->printTable("./asset/table.txt");
    std::cout << "Table Done\n";

    mParser->analyze(lexer->inputs, "./asset/Regulation_process.txt");

    while(ui->listWidget->count()) {
        auto itm = ui->listWidget->item(0);
        ui->listWidget->removeItemWidget(itm);
        delete itm;
    }
//    ui->listWidget->addItem(new AssetItem("源程序", "./source.txt"));
//    ui->listWidget->addItem(new AssetItem("词法分析结果", "./asset/lex.txt"));
//    ui->listWidget->addItem(new AssetItem("文法产生式", "./asset/printed_grammar.txt"));
//    ui->listWidget->addItem(new AssetItem("FIRST集", "./asset/firstVN.txt"));
//    ui->listWidget->addItem(new AssetItem("项目集族", "./asset/cluster.txt"));
    ui->listWidget->addItem(new AssetItem("状态转移表", "./asset/table.txt"));
    ui->listWidget->addItem(new AssetItem("规约过程", "./asset/Regulation_process.txt"));

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
