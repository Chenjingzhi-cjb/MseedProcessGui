#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include <vector>

#include "mseed_process.hpp"
#include "pyenv_configurator.hpp"


QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_button_set_spm_path_clicked();

    void on_button_run_clicked();

    void on_checkBox_virtual_env_clicked();

    void on_button_virtual_env_choose_clicked();

    void on_button_interpreter_install_clicked();

    void on_button_dependency_install_clicked();

    void on_button_virtual_env_clear_clicked();

    void on_button_output_excel_clicked();

signals:
    void signalMessage(QString);

private:
    void loadUiInit();

    void loadConnect();

    void updatePythonInterpreterUI();

    void updatePythonDependencyUI();

    void pythonEnvInit();

private:
    Ui::MainWindow *ui;

    MseedProcess m_mp;

    PyenvConfigurator m_pc;
};


#endif // MAINWINDOW_H
