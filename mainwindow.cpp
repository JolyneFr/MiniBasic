#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->load_button, &QPushButton::clicked, this, &MainWindow::on_click_load);
    connect(ui->run_button, &QPushButton::clicked, this, &MainWindow::on_click_run);
    connect(ui->clear_button, &QPushButton::clicked, this, &MainWindow::on_click_clear);
    connect(ui->ciw_input, &QLineEdit::returnPressed, this, &MainWindow::on_click_load);

    cur_prog = new Program();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_click_load()
{
    append_code();
    ui->ciw_input->clear();
}

void MainWindow::on_click_run()
{

}

void MainWindow::on_click_clear()
{
    cur_prog->clear();
    ui->code_display->clear();
}

void MainWindow::refresh_display()
{

}

void MainWindow::append_code()
{
    string cur_text = (ui->ciw_input->text()).toStdString();
    if (cur_text == "") return;
    cur_prog->append(cur_text);
    ui->code_display->insertPlainText(QString::fromStdString(cur_text) + '\n');
}
