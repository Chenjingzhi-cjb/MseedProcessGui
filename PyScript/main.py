import os

import argparse
import numpy as np
import matplotlib.pyplot as plt
import xlsxwriter
from obspy.core import read


def format_conversion(folder_path):
    count = 0
    for filename in os.listdir(folder_path):
        if filename.endswith(".mseed"):
            file_path = os.path.join(folder_path, filename)

            st = read(file_path)
            trace_num = len(st)

            if trace_num == 1:
                st.write(file_path + '_TSPAIR', format='TSPAIR')
                count = count + 1
                print('%04d --------> %s' % (count, filename))
            else:
                print('ERROR:the number of the trace is not one ??? %s' % filename)


def data_alignment(folder_path, count):
    for filename in os.listdir(folder_path):
        if filename.endswith(".mseed_TSPAIR"):
            file_path = os.path.join(folder_path, filename)

            with open(file_path, 'r', encoding='utf-8') as file:
                lines = file.readlines()

            if len(lines) > count:
                lines = lines[:count]

            with open(file_path, 'w', encoding='utf-8') as file:
                file.writelines(lines)


def data_conversion_base(filename, folder_path):
    file_path = os.path.join(folder_path, filename)

    with open(file_path, 'r') as f:
        raw_datas = f.readlines()
        datas = [int(i.split(' ')[-1]) for i in raw_datas[1:]]

    # 输入信号
    x = np.array(datas)

    # 获取采样率（Fs）的值
    fs = 1000  # 假设采样率为 1000 Hz

    # 计算频率步长
    freq_step = fs / len(x)

    # 创建频率轴
    freq_axis = np.arange(0, fs / 2, freq_step)

    # 零填充输入信号以匹配频率轴的长度
    padded_x = np.pad(x, (0, len(freq_axis) * 2 - len(x)), mode='constant')

    # 计算傅里叶变换
    fx = np.fft.fft(padded_x)

    # 取前半部分复数的绝对值
    half_abs_fx = abs(fx[:len(freq_axis)])

    return freq_axis, half_abs_fx


def data_conversion_C(folder_path):
    for index, filename in enumerate(os.listdir(folder_path)):
        if filename.endswith(".mseed_TSPAIR"):
            freq_axis, half_abs_fx = data_conversion_base(filename, folder_path)

            # 绘制散点图并将图像存储到列表中
            plt.figure(index)
            plt.plot(freq_axis[1:], half_abs_fx[1:], linestyle='-')
            plt.xlabel('Frequency (Hz)')
            plt.ylabel('Magnitude')
            plt.title('FFT Spectrum for {}'.format(filename))  # 使用文件名作为标题

    # 显示所有图像
    plt.show()


def data_conversion_S(folder_path):
    workbook = xlsxwriter.Workbook(folder_path + '\\' + "result.xlsx")
    worksheet = workbook.add_worksheet()

    col = 0
    for filename in os.listdir(folder_path):
        if filename.endswith(".mseed_TSPAIR"):
            freq_axis, half_abs_fx = data_conversion_base(filename, folder_path)

            row = 0
            for i in range(len(freq_axis)):
                # 往表格写入内容
                worksheet.write(row, col, freq_axis[i])
                worksheet.write(row, col + 1, half_abs_fx[i])
                row += 1
            col += 2

    # 保存
    workbook.close()


def main(folder_path, alignment_count, exec_type):
    format_conversion(folder_path)
    data_alignment(folder_path, alignment_count)

    if exec_type == 'C':
        data_conversion_C(folder_path)
    elif exec_type == 'S':
        data_conversion_S(folder_path)

    print("Finished!")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Parse arguments for main.py')
    parser.add_argument('-F', help='Folder Path')
    parser.add_argument('-A', type=int, help='Alignment Count')
    parser.add_argument('-E', choices=['C', 'S'], help='Exec Type')

    args = parser.parse_args()

    main(args.F, args.A, args.E)
