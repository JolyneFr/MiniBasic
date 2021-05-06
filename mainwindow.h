#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <fstream>
#include "program.h"
#include "programrunner.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Program *cur_prog;
    ProgramRunner prog_runner;
    int lineIndex = 1;

    bool HAS_HELP = false;

    void display_code_from_file(const std::string &filename);
    void save_file(const std::string &filename);
    void append_code();
    void sync_code_display();
    void read_from_begin();

private slots:
    void press_enter();
    void click_run();
    void click_help();
    void click_clear();
    void click_save_file();
    void click_load_file();
    void click_debug();
};
#endif // MAINWINDOW_H
