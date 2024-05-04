#ifndef PYENV_CONFIGURATOR_HPP
#define PYENV_CONFIGURATOR_HPP

#include <QString>
#include <QProcess>
#include <QCoreApplication>
#include <QTextStream>
#include <QFile>

#include <string>


class PyenvConfigurator : public QObject {
    Q_OBJECT

public:
    PyenvConfigurator(QObject *parent = nullptr)
        : QObject(parent),
          m_virtual_env_config_path(QCoreApplication::applicationDirPath() + "/py_virtual_path.txt") {}

    ~PyenvConfigurator() = default;

public:
    bool checkInterpreter() {
        // 创建一个 QProcess 对象
        QProcess process;

        // 设置要执行的命令和参数
        QString interpreter = "python";
        QStringList arguments;
        arguments << "-V";

        // 检查 Python
        process.start(interpreter, arguments);

        // 等待进程执行
        if (!process.waitForStarted()) {
            emit signalMessage("Failed to start check Python!");
            return false;
        }

        // 等待进程完成
        if (!process.waitForFinished(-1)) {
            emit signalMessage("Check Python failed to finish!");
            return false;
        }

        // 检查进程的退出状态
        int exit_code = process.exitCode();
        emit signalMessage(QString::fromStdString("Check Python finished with exit code: " + std::to_string(exit_code)));

        if (exit_code != 0) return false;

        return true;
    }

    bool checkDependency() {
        bool error = false;

        // 设置要执行的命令和参数
        QString interpreter = "pip";
        if (!m_py_virtual_path.empty()) {
            interpreter = QString::fromStdString(m_py_virtual_path + "/Scripts/pip");
        }

        std::vector<QStringList> arguments_list(4);
        arguments_list[0] << "show" << "numpy";
        arguments_list[1] << "show" << "matplotlib";
        arguments_list[2] << "show" << "obspy";
        arguments_list[3] << "show" << "xlsxwriter";

        for (auto &arguments : arguments_list) {
            // 创建一个 QProcess 对象
            QProcess process;

            // 检查 Python
            process.start(interpreter, arguments);

            // 等待进程执行
            if (!process.waitForStarted()) {
                emit signalMessage(QString::fromStdString("Failed to start check " + arguments.last().toStdString() + "!"));
                return false;
            }

            // 等待进程完成
            if (!process.waitForFinished(-1)) {
                emit signalMessage(QString::fromStdString("Check " + arguments.last().toStdString() + " failed to finish!"));
                return false;
            }

            // 检查进程的退出状态
            int exit_code = process.exitCode();
            emit signalMessage(QString::fromStdString("Check " + arguments.last().toStdString() + " finished with exit code: " + std::to_string(exit_code)));

            if (exit_code != 0) error = true;
        }

        if (error) return false;

        return true;
    }

    bool installInterpreter() {
        // 创建一个 QProcess 对象
        QProcess process;

        // 设置要执行的命令和参数
        QString interpreter = "PyEnv/python-3.9.13-amd64.exe";
        QStringList arguments;

        // 安装 Python
        process.start(interpreter, arguments);

        // 等待进程执行
        if (!process.waitForStarted()) {
            emit signalMessage("Failed to start install Python!");
            return false;
        }

        // 等待进程完成
        if (!process.waitForFinished(-1)) {
            emit signalMessage("Install Python failed to finish!");
            return false;
        }

        // 检查进程的退出状态
        int exit_code = process.exitCode();
        emit signalMessage(QString::fromStdString("Install Python finished with exit code: " + std::to_string(exit_code)));

        return true;
    }

    bool installDependency() {
        // 设置要执行的命令和参数
        QString interpreter = "pip";

        std::vector<QStringList> arguments_list(4);
        arguments_list[0] << "install" << "--no-index" << "--find-links=./PyEnv/path" << "numpy";
        arguments_list[1] << "install" << "--no-index" << "--find-links=./PyEnv/path" << "matplotlib";
        arguments_list[2] << "install" << "--no-index" << "--find-links=./PyEnv/path" << "obspy";
        arguments_list[3] << "install" << "--no-index" << "--find-links=./PyEnv/path" << "xlsxwriter";

        for (auto &arguments : arguments_list) {
            // 创建一个 QProcess 对象
            QProcess process;

            // 安装依赖
            process.start(interpreter, arguments);

            // 等待进程执行
            if (!process.waitForStarted()) {
                emit signalMessage(QString::fromStdString("Failed to start install " + arguments.last().toStdString() + "!"));
                return false;
            }

            // 等待进程完成
            if (!process.waitForFinished(-1)) {
                emit signalMessage(QString::fromStdString("Install " + arguments.last().toStdString() + " failed to finish!"));
                return false;
            }

            // 检查进程的退出状态
            int exit_code = process.exitCode();
            emit signalMessage(QString::fromStdString("Install " + arguments.last().toStdString() + " finished with exit code: " + std::to_string(exit_code)));
        }

        return true;
    }

    QString readVirtualEnvConfig() {
        QFile py_virtual_path_file(m_virtual_env_config_path);
        if (py_virtual_path_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream py_virtual_path_in(&py_virtual_path_file);
            QString py_virtual_path = py_virtual_path_in.readLine();
            py_virtual_path_file.close();

            return py_virtual_path;
        }

        return "";
    }

    void writeVirtualEnvConfig(QString path) {
        QFile py_virtual_path_file(m_virtual_env_config_path);
        if (py_virtual_path_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream py_virtual_path_out(&py_virtual_path_file);
            py_virtual_path_out << path;
            py_virtual_path_file.close();
        }
    }

    void setPyVirtualPath(std::string path) {
        m_py_virtual_path = path;
    }

signals:
    void signalMessage(QString);

private:
    const QString m_virtual_env_config_path;

    std::string m_py_virtual_path;
};


#endif // PYENV_CONFIGURATOR_HPP
