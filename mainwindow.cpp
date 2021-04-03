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
    connect(ui->ciw_input, &QLineEdit::returnPressed, this, &MainWindow::press_enter);
    connect(ui->action_save, &QAction::triggered, this, &MainWindow::click_save_file);
    connect(ui->action_load, &QAction::triggered, this, &MainWindow::click_load_file);
    //connect(ui->delete_button, &QPushButton::clicked, this, &MainWindow::click_delete);
    cur_prog = new Program();
    prog_runner.setDisplay(ui->code_display, ui->result_display, ui->ss_display, ui->error_label);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::press_enter()
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
        if (!tokens.empty()) prog_runner.readStatement(tokens);
        lineIndex++;
    }
}

void MainWindow::click_clear()
{
    cur_prog->clear();
    ui->code_display->clear();
    ui->result_display->clear();
    ui->ss_display->clear();
    prog_runner.clear();
}

void MainWindow::click_save_file()
{
    QString filename = QFileDialog::getSaveFileName(this,
                       tr("Save your Idiot Basic Program！"),
                       "../MiniBasic/Code/proj.txt", tr("text file(*txt)"));
    prog_runner.saveCode(filename.toStdString());
    qDebug() <<"saved filename: " << filename;
}

void MainWindow::save_file(const std::string &filename)
{
    prog_runner.saveCode(filename);
    qDebug() <<"saved filename: " << QString::fromStdString(filename);
}

void MainWindow::click_load_file()
{
    click_clear();
    cur_prog->clear();
    QString filename = QFileDialog::getOpenFileName(this,
                       tr("Open your Terrible Basic Program！"),
                       "../MiniBasic/Code/", tr("text file(*txt)"));
    cur_prog->load(filename.toStdString());
    qDebug() <<"loaded filename: " << filename;

    read_from_begin();
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
    QString qtext = ui->ciw_input->text().trimmed();
    string cur_text = qtext.toStdString();
    if (qtext == "") return; // ignore empty input
    if (qtext == "LOAD") {
        click_load_file(); // handle LOAD statement
        return;
    }
    if (qtext == "CLEAR") {
        click_clear();
        return;
    }
    if (qtext == "QUIT") {
        this->close();
    }
    QVector<Token> tokens = getTokens(cur_text);
    prog_runner.readStatement(tokens);

    lineIndex++;
}
