#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include "ui/AssetItem.h"
#include <QProcess>
#include <QMessageBox>

#include "src/Generator.h"
extern Generator generator;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_2->setEnabled(false);
    QObject::connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::loadGrammar);
    QObject::connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::genAndShowTree);
    QObject::connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::loadAsset);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString u8(const char* str) {
    return QString::fromUtf8(str);
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
    lexer->analyze("./asset/source.txt");
    lexer->print("./asset/lex.txt");
    std::cout << "Lex Analysis Done" << '\n';

    mParser->readGrammarY("./asset/c99.txt");
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

    mParser->analyze(lexer->inputs,"./asset/output.txt");
    mParser->printTree("./asset/tree.dot");

    generator.output("./asset/quads.txt");

    while(ui->listWidget->count()) {
        auto itm = ui->listWidget->item(0);
        ui->listWidget->removeItemWidget(itm);
        delete itm;
    }

    ui->listWidget->addItem(new AssetItem(u8("源程序\0"), u8("./asset/source.txt")));
    ui->listWidget->addItem(new AssetItem(u8("词法分析结果"), u8("./asset/lex.txt")));
    ui->listWidget->addItem(new AssetItem(u8("文法产生式\0"), u8("./asset/printed_grammar.txt")));
    ui->listWidget->addItem(new AssetItem(u8("FIRST集\0"), u8("./asset/firstVN.txt")));
//    ui->listWidget->addItem(new AssetItem(u8("项目集族"), u8("./asset/cluster.txt"), true));
    ui->listWidget->addItem(new AssetItem(u8("状态转移表"), u8("./asset/table.txt")));
    ui->listWidget->addItem(new AssetItem(u8("规约过程"), u8("./asset/output.txt")));
    ui->listWidget->addItem(new AssetItem(u8("语法树\0"), u8("./asset/tree.dot")));
    ui->listWidget->addItem(new AssetItem(u8("中间代码四元式\0"), u8("./asset/quads.txt")));

    ui->textEdit->clear();
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::loadAsset(QListWidgetItem *item)
{
    AssetItem *itm = (AssetItem*)item;
    if (itm->big) {
        if (QMessageBox::question(nullptr, itm->name + u8(" 文件巨大"), u8("打开需要很久，确定要查看吗？\0")) == QMessageBox::No)
            return;
//        QMessageBox msgbox;
//        msgbox.setText(itm->name + u8(" 文件巨大"));
//        msgbox.setInformativeText(u8("打开需要很久，确定要查看吗？\0"));
//        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//        msgbox.setStandardButtons(QMessageBox::No);
//        if (msgbox.exec() == QMessageBox::No)
//            return;
    }
    ui->textEdit->clear();
    QFile inputFile(itm->fpath);
    if (!inputFile.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&inputFile);
    QString line = in.readAll();
    inputFile.close();

    ui->textEdit->setPlainText(line);
}

void MainWindow::genAndShowTree()
{
    QString program = "./graphviz/dot.exe";
    QStringList arguments;
    arguments << "./asset/tree.dot" << "-T" << "png" << "-o" << "./asset/tree.png";
    QProcess process;
    process.start(program, arguments);
    if (process.waitForFinished()) {
        if (procImg)
            delete procImg;
        QString p2 = "cmd.exe";
        QStringList args2;
        args2 << "/c" << "start" << "./asset/tree.png";
        procImg = new QProcess;
        procImg->start(p2, args2);
        procImg->waitForFinished();
    } else {
        std::cout << "generate tree.png failed\n";
    }
}
