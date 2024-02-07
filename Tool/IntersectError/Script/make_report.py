import matplotlib.pyplot as plt

import csv
from pathlib import Path


def read_csv_data(file_name):
    csv_data = [[] for i in range(5)]
    with open(Path(__file__).with_name(file_name), newline='') as csv_file:
        csv_reader = csv.reader(csv_file)
        for row in csv_reader:
            csv_data[0].append(float(row[0])) # x
            csv_data[1].append(float(row[1])) # num
            csv_data[2].append(float(row[2])) # mean
            csv_data[3].append(float(row[3])) # min
            csv_data[4].append(float(row[4])) # max

    return csv_data


fig = plt.figure(figsize=(12, 8))
axes = fig.subplots(2, 2)

triangle_error_vs_dist_data = read_csv_data("triangle_error_vs_dist.csv")
axes[0, 0].set_title("%d Triangle Intersects" % sum(triangle_error_vs_dist_data[1]))
axes[0, 0].plot(triangle_error_vs_dist_data[0], triangle_error_vs_dist_data[2])
axes[0, 0].set_xscale('log', base=10)
axes[0, 0].set_yscale('log', base=10)
axes[0, 0].set_xlabel("Hit Distance")
axes[0, 0].set_ylabel("Error")

triangle_error_vs_size_data = read_csv_data("triangle_error_vs_size.csv")
axes[0, 1].set_title("%d Triangle Intersects" % sum(triangle_error_vs_size_data[1]))
axes[0, 1].plot(triangle_error_vs_size_data[0], triangle_error_vs_size_data[2])
axes[0, 1].set_xscale('log', base=10)
axes[0, 1].set_yscale('log', base=10)
axes[0, 1].set_xlabel("Max. Extent")
axes[0, 1].set_ylabel("Error")

plt.show()
