import pickle
import numpy as np
import argparse
import os

import matplotlib.pyplot as plt

def load_data(file_path):
  with open(file_path, 'rb') as file:
    data = pickle.load(file)
  return data

def plot_trendline(x, y, label):
  plt.scatter(x, y, label=label)
  z = np.polyfit(x, y, 1)
  p = np.poly1d(z)
  plt.plot(x, p(x), linestyle='--', label=f'{label} Trendline')

def save_plot(x, y, label, output_dir):
  plt.figure()
  plot_trendline(x, y, label)
  plt.xlabel('X-axis Label')
  plt.ylabel('Y-axis Label')
  plt.title(f'Trendline for {label}')
  plt.legend()
  plt.savefig(os.path.join(output_dir, f'{label}_trendline.png'))
  plt.close()

# given a .pkl data file and pyplot, add the health data with a given label
def plot_health(ax, fpkl, label):
  (colmap,colapp,data) = load_data(fpkl)
  x = data[0]
  y = data[2]
  ax.scatter(x, y, label=label)

if __name__ == "__main__":
  # create figure
  fig, ax = plt.subplots()

  ax.set_xlabel('Time Elapsed (hr)')
  ax.set_ylabel('Health (HP)')

  plot_health(ax, "data/pickled/trial_random.pkl", 'Random Actor')

  ax.legend()
  # save the plot
  plt.savefig("data/plots/health_plot.png")