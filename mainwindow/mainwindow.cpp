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

    ui->scrollArea_select_files->setStyleSheet(
        "QScrollArea {"
        "    border: 1px solid gray;"
        "}"
    );
}

void MainWindow::loadConnect() {
    connect(this, &MainWindow::signalMessageInfo, this, [=](QString info) {
        ui->textBrowser_log->setTextColor(Qt::black);
        ui->textBrowser_log->append(info);
        ui->textBrowser_log->append(" ");
    });

    connect(this, &MainWindow::signalMessageError, this, [=](QString info) {
        ui->textBrowser_log->setTextColor(Qt::red);
        ui->textBrowser_log->append(info);
        ui->textBrowser_log->append(" ");
    });

    connect(&m_pc, &PyenvConfigurator::signalMessageInfo, this, [=](QString info) {
        ui->textBrowser_log->setTextColor(Qt::black);
        ui->textBrowser_log->append(info);
        ui->textBrowser_log->append(" ");
    });

    connect(&m_pc, &PyenvConfigurator::signalMessageError, this, [=](QString info) {
        ui->textBrowser_log->setTextColor(Qt::red);
        ui->textBrowser_log->append(info);
        ui->textBrowser_log->append(" ");
    });

    connect(&m_mp, &MseedProcess::signalMessageInfo, this, [=](QString info) {
        ui->textBrowser_log->setTextColor(Qt::black);
        ui->textBrowser_log->append(info);
        ui->textBrowser_log->append(" ");
    });

    connect(&m_mp, &MseedProcess::signalMessageError, this, [=](QString info) {
        ui->textBrowser_log->setTextColor(Qt::red);
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
        ui->checkBox_virtual_env->setCheckState(Qt::Checked);
    }

    // 检查解释器
    updatePythonInterpreterUI();

    // 检查依赖
    updatePythonDependencyUI();
}

void MainWindow::exexPythonScript(int cmd) {
    if (!ui->input_mseed_path->text().isEmpty()) {
        std::vector<std::string> file_name_list;

        for (auto *i : m_file_list_checkbox) {
            if (i->checkState() == Qt::Checked) {
                file_name_list.emplace_back(i->text().toStdString());
            }
        }

        m_mp.setAlignmentCount(ui->spinBox_alignment_count->value());
        m_mp.setFrequencyRange(ui->dSpinBox_freq_min->value(), ui->dSpinBox_freq_max->value());
        m_mp.run(cmd, file_name_list);
    } else {
        emit signalMessageError("[PyProcess] Please select the files path first!");
    }
}

void MainWindow::slotFilesCheckBoxStateChanged() {
    bool allChecked = true, anyChecked = false;

    for (auto *checkbox : m_file_list_checkbox) {
        if (checkbox->isChecked()) {
            anyChecked = true;
        } else {
            allChecked = false;
        }
    }

    // 更新全选复选框的状态
    if (allChecked) {
        ui->checkBox_select_all_files->setCheckState(Qt::Checked);
    } else if (anyChecked) {
        ui->checkBox_select_all_files->setCheckState(Qt::PartiallyChecked);
    } else {
        ui->checkBox_select_all_files->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::updateWidgetSelectFiles(QStringList &file_list) {
    // 移除已有的部件
    if (ui->scrollArea_select_files ->widget()) {
        delete ui->scrollArea_select_files->widget();
    }

    // 创建容器部件和布局
    QWidget *container = new QWidget(ui->scrollArea_select_files);
    QVBoxLayout *layout = new QVBoxLayout(container);

    container->setMinimumHeight((int) file_list.size() * 35);
    container->setMaximumHeight((int) file_list.size() * 35);

    layout->setSpacing(0);
    layout->setContentsMargins(5, 0, 5, 0);

    container->setLayout(layout);

    // 准备动态内容
    m_file_list_checkbox.clear();

    for (int i = 0; i < (int) file_list.size(); i++) {
        QCheckBox *check_box = new QCheckBox(file_list[i]);
        check_box->setChecked(true);
        check_box->setStyleSheet("padding: 5px; font-size: 12px;");

        // 将 QCheckBox 添加到布局中
        layout->addWidget(check_box);
        m_file_list_checkbox.emplace_back(check_box);

        connect(check_box, &QCheckBox::stateChanged, this, [=](int) {
            slotFilesCheckBoxStateChanged();
        });

        // 添加分割线
        QFrame *line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setStyleSheet("color: gray;");  // 设置分割线颜色
        layout->addWidget(line);
    }

    ui->checkBox_select_all_files->setCheckState(Qt::Checked);

    // 调整容器尺寸并设置到 QScrollArea 中
    ui->scrollArea_select_files->setWidget(container);
    ui->scrollArea_select_files->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea_select_files->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void MainWindow::on_button_set_mseed_path_clicked() {
    QString folder_path = QFileDialog::getExistingDirectory(this, "Select directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!folder_path.isEmpty()) {
        m_mp.setFolderPath(folder_path.toStdString());
        ui->input_mseed_path->setText(folder_path);

        // get file list
        QDir dir(folder_path);

        QStringList filters;
        filters << "*.mseed";
        QStringList file_list = dir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);

        updateWidgetSelectFiles(file_list);
    }
}

void MainWindow::on_checkBox_virtual_env_stateChanged(int arg1) {
    m_mp.setPyVirtualFlag(arg1 != 0);
}

void MainWindow::on_button_virtual_env_choose_clicked() {
    QString folder_path = QFileDialog::getExistingDirectory(this, "Select directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!folder_path.isEmpty()) {
        m_pc.writeVirtualEnvConfig(folder_path);  // 写入虚拟环境配置
        m_pc.setPyVirtualPath(folder_path.toStdString());
        m_mp.setPyVirtualPath(folder_path.toStdString());
        QStringList path_split_list = folder_path.split("/");
        ui->label_python_env_value->setText(path_split_list.at(path_split_list.size() - 2));
        ui->checkBox_virtual_env->setCheckState(Qt::Checked);
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
    ui->checkBox_virtual_env->setCheckState(Qt::Unchecked);

    // 检查依赖
    updatePythonDependencyUI();
}

void MainWindow::on_button_show_pyplot_clicked() {
    exexPythonScript(0);
}

void MainWindow::on_button_save_pyplot_clicked() {
    exexPythonScript(1);
}

void MainWindow::on_button_output_excel_clicked() {
    exexPythonScript(2);
}

void MainWindow::on_checkBox_select_all_files_stateChanged(int arg1) {
    if (arg1 == 1) {
        for (auto *checkbox : m_file_list_checkbox) {
            if (checkbox->isChecked()) {
                return;
            }
        }
    }

    bool checked = (arg1 != 0);
    for (auto *checkbox : m_file_list_checkbox) {
        checkbox->setChecked(checked);
    }
}
