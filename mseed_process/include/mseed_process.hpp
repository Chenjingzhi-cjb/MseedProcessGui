#ifndef MSEED_PROCESS_HPP
#define MSEED_PROCESS_HPP

#include <QString>
#include <QProcess>
#include <QCoreApplication>

#include <string>


class MseedProcess : public QObject {
    Q_OBJECT

public:
    MseedProcess(QObject *parent = nullptr)
        : QObject(parent),
          m_py_virtual_flag(false),
          m_alignment_count(40000),
          m_freq_min(0),
          m_freq_max(500) {}

    ~MseedProcess() = default;

public:
    int run(int exec_type, std::vector<std::string> file_name_list) {
        if (file_name_list.empty()) return -1;

        // 创建一个 QProcess 对象
        QProcess py_process;

        QString exec_str;
        if (exec_type == 0) exec_str = "Plot1";
        else if (exec_type == 1) exec_str = "Plot2";
        else if (exec_type == 2) exec_str = "Excel";

        // 设置要执行的命令和参数
        QString py_interpreter = "python";
        if (m_py_virtual_flag) {
            if (m_py_virtual_path.empty()) {
                emit signalMessageError("[PyEnv] Python virual path is empty!");
                return -1;
            }

            py_interpreter = QString::fromStdString(m_py_virtual_path + "/Scripts/python");  // 拼接 Python 解释器路径
        }
        QStringList py_script_arguments;
        py_script_arguments << (QCoreApplication::applicationDirPath() + "/PyScript/main.py")
                            << "-F" << QString::fromStdString(m_folder_path)
                            << "-A" << QString::number(m_alignment_count)
                            << "-E" << exec_str
                            << "-FMin" << QString::number(m_freq_min)
                            << "-FMax" << QString::number(m_freq_max);

        py_script_arguments << "-fs";
        for (auto &fn : file_name_list) {
            py_script_arguments << QString::fromStdString(fn);
        }

        emit signalMessageInfo(QString::fromStdString("[PyProcess] Startup script ......"));

        // 启动 Python 脚本
        py_process.start(py_interpreter, py_script_arguments);

        // 等待进程完成
        if (!py_process.waitForStarted()) {
            emit signalMessageError("[PyProcess] Failed to start Python process!");
            return -1;
        }

        // 等待 Python 脚本执行完成
        if (!py_process.waitForFinished(-1)) {
            emit signalMessageError("[PyProcess] Python process failed to finish!");
            return -1;
        }

        // 检查 Python 脚本的退出状态
        int exit_code = py_process.exitCode();
        if (exit_code != 0) {
            emit signalMessageError(QString::fromStdString("[PyProcess] Python process finished with exit code: " + std::to_string(exit_code)));
            return -1;
        }

        if (exec_type == 0)
            emit signalMessageInfo("[PyProcess] The charts are generated.");
        else if (exec_type == 1)
            emit signalMessageInfo("[PyProcess] The charts have been saved in the source file directory.");
        else if (exec_type == 2)
            emit signalMessageInfo("[PyProcess] The data has been exported to \"result.xlsx\" in the source file directory.");

        return 0;
    }

    void setPyVirtualFlag(bool flag) {
        m_py_virtual_flag = flag;
    }

    void setPyVirtualPath(std::string path) {
        m_py_virtual_path = path;
    }

    void setFolderPath(std::string path) {
        m_folder_path = path;
    }

    std::string getFolderPath() {
        return m_folder_path;
    }

    void setAlignmentCount(int count) {
        m_alignment_count = count;
    }

    void setFrequencyRange(double min, double max) {
        m_freq_min = min;
        m_freq_max = max;
    }

signals:
    void signalMessageInfo(QString);

    void signalMessageError(QString);

private:
    bool m_py_virtual_flag;
    std::string m_py_virtual_path;

    std::string m_folder_path;
    int m_alignment_count;
    double m_freq_min;
    double m_freq_max;
};


#endif // MSEED_PROCESS_HPP
