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

# return None if not found
def inverse_dict(d, value):
  for k, v in d.items():
    if v == value:
      return k
  return None

# given a .pkl data file and pyplot, add the health data with a given label
def plot_health(ax, fpkl, label):
  (colmap,colapp,data) = load_data(fpkl)
  # find the index for "Time Elapsed" column header
  timeIdx = inverse_dict(colmap, "Time Elapsed")
  assert not timeIdx is None
  # find the index for "Character 0 Health" column header
  healthIdx = inverse_dict(colmap, "Character 0 Health")
  assert not healthIdx is None 

  x_start = colapp[timeIdx]
  y_start = colapp[healthIdx]
  assert y_start >= x_start
  idx_start = y_start-x_start
  x = data[timeIdx][idx_start:]
  y = data[healthIdx]
  ax.scatter(x, y, label=label)

def main():
  # create figure
  fig, ax = plt.subplots()

  ax.set_xlabel('Time Elapsed (hr)')
  ax.set_ylabel('Health (HP)')

  plot_health(ax, "data/pickled/trial_random.pkl", 'Random Actor')
  plot_health(ax, "data/pickled/trial_0001.pkl", 'AI Actor')

  ax.legend()
  # save the plot
  plt.savefig("data/plots/health_plot.png")

if __name__ == "__main__":
  main()