import matplotlib.pyplot as plt
import numpy as np

import csv
from pathlib import Path


def read_csv_data(file_name):
    csv_data = [[] for i in range(6)]
    with open(Path(__file__).with_name(file_name), newline='') as csv_file:
        csv_reader = csv.reader(csv_file)
        for row in csv_reader:
            csv_data[0].append(float(row[0])) # x
            csv_data[1].append(float(row[1])) # num
            csv_data[2].append(float(row[2])) # mean
            csv_data[3].append(float(row[3])) # sigma
            csv_data[4].append(float(row[4])) # min
            csv_data[5].append(float(row[5])) # max

        for i, value_list in enumerate(csv_data):
            csv_data[i] = np.asarray(value_list)

    return csv_data


def chebyshev_bound(k, n):
    return ((n + 1) / n * ((n - 1) / k**2 + 1)) / (n + 1)


fig = plt.figure(figsize=(20, 8))
axes = fig.subplots(2, 3)

error_vs_dist = read_csv_data("error_vs_dist.csv")
axes[0, 0].plot(error_vs_dist[0], error_vs_dist[2], label="Mean")
axes[0, 0].plot(error_vs_dist[0], error_vs_dist[5], label="Max.")
axes[0, 0].set_xscale('log', base=10)
axes[0, 0].set_yscale('log', base=10)
axes[0, 0].set_xlabel("Hit Distance")
axes[0, 0].set_ylabel("Error")
axes[0, 0].legend()

error_vs_size = read_csv_data("error_vs_size.csv")
axes[1, 0].plot(error_vs_size[0], error_vs_size[2], label="Mean")
axes[1, 0].plot(error_vs_size[0], error_vs_size[5], label="Max.")
axes[1, 0].set_xscale('log', base=10)
axes[1, 0].set_yscale('log', base=10)
axes[1, 0].set_xlabel("Max. Extent")
axes[1, 0].set_ylabel("Error")
axes[1, 0].legend()

offset_vs_dist = read_csv_data("offset_vs_dist.csv")
k = 10
axes[0, 1].plot(error_vs_size[0], chebyshev_bound(k, error_vs_size[1]))
axes[0, 1].set_xscale('log', base=10)
axes[0, 1].set_yscale('log', base=10)
# axes[0, 1].set_ylim([1e-4, 1e-1])
axes[0, 1].set_xlabel("Hit Distance")
axes[0, 1].set_ylabel(f"Probability outside {k}*sigma")
axes[0, 1].legend()

axes[1, 1].plot(offset_vs_dist[0], offset_vs_dist[2], label="Mean")
axes[1, 1].plot(offset_vs_dist[0], offset_vs_dist[5], label="Max.")
axes[1, 1].plot(offset_vs_dist[0], offset_vs_dist[2] + k * offset_vs_dist[3], label=f"{k}*sigma")
axes[1, 1].set_xscale('log', base=10)
axes[1, 1].set_yscale('log', base=10)
axes[1, 1].set_xlabel("Hit Distance")
axes[1, 1].set_ylabel("Offset Distance")
axes[1, 1].legend()

# axes[1, 1].plot(error_vs_size[0], error_vs_size[5])
# axes[1, 1].set_xscale('log', base=10)
# axes[1, 1].set_yscale('log', base=10)
# axes[1, 1].set_xlabel("Max. Extent")
# axes[1, 1].set_ylabel("Max. Error")
# axes[1, 1].legend()

axes[0, 2].plot(error_vs_dist[0], error_vs_dist[1])
axes[0, 2].set_xscale('log', base=10)
axes[0, 2].set_xlabel("Hit Distance")
axes[0, 2].set_ylabel("Number of Intersections")

fig.suptitle("%d Intersections" % sum(offset_vs_dist[1]))

plt.show()
