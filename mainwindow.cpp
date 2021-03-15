#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ciw_input->setFocus();

    connect(ui->load_button, &QPushButton::clicked, this, &MainWindow::click_load_file);
    connect(ui->run_button, &QPushButton::clicked, this, &MainWindow::click_run);
    connect(ui->clear_button, &QPushButton::clicked, this, &MainWindow::click_clear);
    connect(ui->ciw_input, &QLineEdit::returnPressed, this, &MainWindow::click_enter);
    connect(ui->action_save, &QAction::triggered, this, &MainWindow::click_save_file);
    connect(ui->action_load, &QAction::triggered, this, &MainWindow::click_load_file);
    //connect(ui->delete_button, &QPushButton::clicked, this, &MainWindow::click_delete);
    cur_prog = new Program();
    prog_runner.setDisplay(ui->code_display, ui->result_display);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::click_enter()
{
    append_code();
    ui->ciw_input->clear();
}

void MainWindow::click_run()
{
    QVector<Token> tokens = getTokens("RUN");
    prog_runner.readStatement(tokens);

    lineIndex++;
}

void MainWindow::read_from_begin()
{
    lineIndex = 1;
    while (lineIndex <= cur_prog->length()) {
        string curStmt = cur_prog->getStatement(lineIndex);
        QVector<Token> tokens = getTokens(curStmt);
        prog_runner.readStatement(tokens);
        lineIndex++;
    }
}

void MainWindow::click_clear()
{
    cur_prog->clear();
    ui->code_display->clear();
    prog_runner.clear();
}

void MainWindow::click_save_file()
{
    QString filename = QFileDialog::getSaveFileName(this,
                       tr("Save your Idiot Basic Program！"),
                       "../MiniBasic/Code/proj.txt", tr("text file(*txt)"));
    cur_prog->save(filename.toStdString());
    qDebug() <<"saved filename: " << filename;
}

void MainWindow::save_file(const std::string &filename)
{
    cur_prog->save(filename);
    qDebug() <<"saved filename: " << QString::fromStdString(filename);
}

void MainWindow::click_load_file()
{
    cur_prog->clear();
    QString filename = QFileDialog::getOpenFileName(this,
                       tr("Open your Terrible Basic Program！"),
                       "../MiniBasic/Code/", tr("text file(*txt)"));
    cur_prog->load(filename.toStdString());
    qDebug() <<"loaded filename: " << filename;

    read_from_begin();
}

void MainWindow::click_delete()
{
    int start = ui->del_from->value();
    int end = ui->del_to->value();
    if (start > end) {
        qDebug() << "Invalid range";
    } else if (start <= 0 || end > cur_prog->length())
        qDebug() << "Line number out of range";
    cur_prog->remove(start, end);
    sync_code_display();
}



void MainWindow::display_code_from_file(const std::string &filename)
{
    std::fstream this_file;
    this_file.open(filename, std::ios::in);
    std::string text;
    while (std::getline(this_file, text)) {
        ui->code_display->append(QString::fromStdString(text));
    }
    this_file.close();
    ui->code_display->insertPlainText("\n");
}

void MainWindow::sync_code_display()
{

}

void MainWindow::append_code()
{
    string cur_text = (ui->ciw_input->text()).toStdString();
    if (cur_text == "") return;
    cur_prog->append(cur_text);
    //ui->code_display->insertPlainText(QString::fromStdString(cur_text) + '\n');

    QVector<Token> tokens = getTokens(cur_text);
    prog_runner.readStatement(tokens);

    lineIndex++;
}
