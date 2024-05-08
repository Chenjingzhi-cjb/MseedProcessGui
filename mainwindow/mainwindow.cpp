#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
          ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setWindowIcon(QIcon(":/ico/resource/icon.ico"));

    loadConnect();

    loadUiInit();

    pythonEnvInit();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadUiInit() {
    ui->textBrowser_log->append(" ");
    ui->input_alignment_count->setText("32500");  // 默认
}

void MainWindow::loadConnect() {
    connect(&m_pc, &PyenvConfigurator::signalMessage, this, [=](QString info) {
        ui->textBrowser_log->append(info);
        ui->textBrowser_log->append(" ");
    });
    connect(&m_mp, &MseedProcess::signalMessage, this, [=](QString info) {
        ui->textBrowser_log->append(info);
        ui->textBrowser_log->append(" ");
    });
}

void MainWindow::updatePythonInterpreterUI() {
    if (m_pc.checkInterpreter()) {
        ui->button_interpreter_install->setEnabled(false);
        ui->label_interpreter_check->setPixmap(QPixmap(":/py_check/resource/right.png"));
    } else {
        ui->button_interpreter_install->setEnabled(true);
        ui->label_interpreter_check->setPixmap(QPixmap(":/py_check/resource/error.png"));
    }
}

void MainWindow::updatePythonDependencyUI() {
    if (m_pc.checkDependency()) {
        ui->button_dependency_install->setEnabled(false);
        ui->label_dependency_check->setPixmap(QPixmap(":/py_check/resource/right.png"));
    } else {
        ui->button_dependency_install->setEnabled(true);
        ui->label_dependency_check->setPixmap(QPixmap(":/py_check/resource/error.png"));
    }
}

void MainWindow::pythonEnvInit() {
    // 检查虚拟环境配置
    QString py_virtual_path = m_pc.readVirtualEnvConfig();  // 读取虚拟环境配置
    if (!py_virtual_path.isEmpty()) {
        m_mp.setPyVirtualPath(py_virtual_path.toStdString());
        m_pc.setPyVirtualPath(py_virtual_path.toStdString());
        QStringList path_split_list = py_virtual_path.split("/");
        ui->label_python_env_value->setText(path_split_list.at(path_split_list.size() - 2));
        ui->checkBox_virtual_env->setChecked(true);
    }

    // 检查解释器
    updatePythonInterpreterUI();

    // 检查依赖
    updatePythonDependencyUI();
}

void MainWindow::on_button_set_spm_path_clicked() {
    QString folder_path = QFileDialog::getExistingDirectory(this, "Select directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!folder_path.isEmpty()) {
        m_mp.setFolderPath(folder_path.toStdString());
        ui->input_spm_path->setText(folder_path);
    }
}

void MainWindow::on_button_run_clicked() {
    if (!ui->input_spm_path->text().isEmpty()) {
        m_mp.setAlignmentCount(ui->input_alignment_count->text().toInt());
        m_mp.run(0);
    }
}

void MainWindow::on_checkBox_virtual_env_clicked() {
    m_mp.setPyVirtualFlag(ui->checkBox_virtual_env->isChecked());
}

void MainWindow::on_button_virtual_env_choose_clicked() {
    QString folder_path = QFileDialog::getExistingDirectory(this, "Select directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!folder_path.isEmpty()) {
        m_pc.writeVirtualEnvConfig(folder_path);  // 写入虚拟环境配置
        m_pc.setPyVirtualPath(folder_path.toStdString());
        m_mp.setPyVirtualPath(folder_path.toStdString());
        QStringList path_split_list = folder_path.split("/");
        ui->label_python_env_value->setText(path_split_list.at(path_split_list.size() - 2));
        ui->checkBox_virtual_env->setChecked(true);
    }

    // 检查依赖
    updatePythonDependencyUI();
}

void MainWindow::on_button_interpreter_install_clicked() {
    m_pc.installInterpreter();

    // 检查解释器
    updatePythonInterpreterUI();
}

void MainWindow::on_button_dependency_install_clicked() {
    m_pc.installDependency();

    // 检查依赖
    updatePythonDependencyUI();
}

void MainWindow::on_button_virtual_env_clear_clicked() {
    m_pc.writeVirtualEnvConfig("");
    m_mp.setPyVirtualPath("");
    m_pc.setPyVirtualPath("");
    ui->label_python_env_value->setText("-");
    ui->checkBox_virtual_env->setChecked(false);

    // 检查依赖
    updatePythonDependencyUI();
}

void MainWindow::on_button_output_excel_clicked() {
    if (!ui->input_spm_path->text().isEmpty()) {
        m_mp.setAlignmentCount(ui->input_alignment_count->text().toInt());
        m_mp.run(1);
    }
}
