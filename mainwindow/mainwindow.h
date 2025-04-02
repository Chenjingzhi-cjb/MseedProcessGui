#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QStringList>

#include <string>
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
    void updateWidgetSelectFiles(QStringList &file_list);

    void on_button_set_mseed_path_clicked();

    void on_checkBox_virtual_env_stateChanged(int arg1);

    void on_button_virtual_env_choose_clicked();

    void on_button_interpreter_install_clicked();

    void on_button_dependency_install_clicked();

    void on_button_virtual_env_clear_clicked();

    void on_button_show_pyplot_clicked();

    void on_button_save_pyplot_clicked();

    void on_button_output_excel_clicked();

signals:
    void signalMessageInfo(QString);

    void signalMessageError(QString);

private:
    void loadUiInit();

    void loadConnect();

    void updatePythonInterpreterUI();

    void updatePythonDependencyUI();

    void pythonEnvInit();

    void exexPythonScript(int cmd);

private:
    Ui::MainWindow *ui;

    std::vector<QCheckBox *> m_file_list_checkbox;

    MseedProcess m_mp;

    PyenvConfigurator m_pc;
};


#endif // MAINWINDOW_H
