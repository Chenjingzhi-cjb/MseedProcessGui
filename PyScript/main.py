import os

import argparse
import numpy as np
import matplotlib.pyplot as plt
import xlsxwriter
from obspy.core import read


def data_conversion_base(filename, folder_path, alignment_count):
    file_path = os.path.join(folder_path, filename)

    stream = read(file_path)
    trace = stream[0]

    # 输入信号
    x = np.array(trace.data)
    if len(x) > alignment_count:
        x = x[:alignment_count]

    # 获取采样率（Fs）的值
    fs = trace.stats.sampling_rate

    # 创建频率轴
    freq_axis = np.fft.rfftfreq(len(x), d=1/fs)

    # 计算傅里叶变换
    fx = np.fft.rfft(x)

    # 取前半部分复数的绝对值
    half_abs_fx = abs(fx)

    return freq_axis, half_abs_fx


def data_conv_pyplot(folder_path, alignment_count, cmd):
    for index, filename in enumerate(os.listdir(folder_path)):
        if filename.endswith(".mseed"):
            freq_axis, half_abs_fx = data_conversion_base(filename, folder_path, alignment_count)

            # 绘制散点图并将图像存储到列表中
            plt.figure(index)
            plt.plot(freq_axis[1:], half_abs_fx[1:], linestyle='-')
            plt.xlabel('Frequency (Hz)')
            plt.ylabel('Magnitude')
            plt.title('FFT Spectrum for {}'.format(filename))  # 使用文件名作为标题

            if cmd == "save":
                # 保存图像
                plt.savefig(f"{folder_path}/{filename.removesuffix('.mseed')}.png")

    if cmd == "show":
        # 显示所有图像
        plt.show()


def num_to_excel_col(n):
    """将 0-based 列索引转换为 Excel 列名（A, B, C, ..., Z, AA, AB, ...）"""
    col_name = ""
    while n >= 0:
        col_name = chr(n % 26 + ord('A')) + col_name
        n = n // 26 - 1
    return col_name


def data_conv_excel(folder_path, alignment_count):
    workbook = xlsxwriter.Workbook(folder_path + '\\' + "result.xlsx")
    worksheet = workbook.add_worksheet()

    chart = workbook.add_chart({"type": "scatter", 'subtype': 'smooth'})

    col = 0
    for filename in os.listdir(folder_path):
        if filename.endswith(".mseed"):
            # 计算频域数据
            freq_axis, half_abs_fx = data_conversion_base(filename, folder_path, alignment_count)

            # 写入频域数据
            worksheet.write(0, col, filename.removesuffix(".mseed"))

            row = 1  # 去直流
            for i in range(1, len(freq_axis)):
                worksheet.write(row, col, freq_axis[i])
                worksheet.write(row, col + 1, half_abs_fx[i])

                row += 1

            # 添加数据到图表
            x_col = num_to_excel_col(col)
            y_col = num_to_excel_col(col + 1)

            chart.add_series({
                "name": f"={worksheet.name}!${x_col}$1",  # 组名称
                "categories": f"={worksheet.name}!${x_col}$2:${x_col}${len(freq_axis)}",  # X 轴数据
                "values": f"={worksheet.name}!${y_col}$2:${y_col}${len(freq_axis)}",  # Y 轴数据
                "line": {"width": 1.5, "smooth": True},  # 平滑曲线
            })

            col += 2

    # 设置图表属性
    chart.set_title({"name": "Spectral Analysis"})
    chart.set_x_axis({
        "name": "Frequency (Hz)",
        "min": 0,
        "major_gridlines": {"visible": True, "line": {"width": 0.5, "dash_type": "dash"}},  # 启用主网格线
    })
    chart.set_y_axis({
        "name": "Amplitude",
        "min": 0,
        "major_gridlines": {"visible": True, "line": {"width": 0.5, "dash_type": "dash"}},  # 启用主网格线
    })
    chart.set_size({"width": 1000, "height": 520})

    # 插入图表
    worksheet.insert_chart("B2", chart)

    # 保存
    workbook.close()


def main(folder_path, alignment_count, exec_type):
    if exec_type == 'Plot1':
        data_conv_pyplot(folder_path, alignment_count, "show")
    elif exec_type == 'Plot2':
        data_conv_pyplot(folder_path, alignment_count, "save")
    elif exec_type == 'Excel':
        data_conv_excel(folder_path, alignment_count)

    print("Finished!")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Parse arguments for main.py')
    parser.add_argument('-F', help='Folder Path')
    parser.add_argument('-A', type=int, help='Alignment Count')
    parser.add_argument('-E', choices=['Plot1', 'Plot2', 'Excel'], help='Exec Type')

    args = parser.parse_args()

    main(args.F, args.A, args.E)
