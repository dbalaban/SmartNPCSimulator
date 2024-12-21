import pickle
import numpy as np
import argparse
import os
import glob

import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm

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
def plot_health(ax, fpkl, label, color=None):
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
  ax.plot(x, y, label=label, color=color)

def integrate(param_name, fpkl):
  (colmap,colapp,data) = load_data(fpkl)
  # find the index for "Time Elapsed" column header
  timeIdx = inverse_dict(colmap, "Time Elapsed")
  assert not timeIdx is None
  # find the index for "Character 0 Health" column header
  paramIdx = inverse_dict(colmap, param_name)
  assert not paramIdx is None 

  x_start = colapp[timeIdx]
  y_start = colapp[paramIdx]
  assert y_start >= x_start
  idx_start = y_start-x_start
  x = data[timeIdx][idx_start:]
  y = data[paramIdx]

  # trim any none values, delete corresponding x values
  NoneIdx = np.array([i for i, val in enumerate(x) if val is None or np.isnan(val)], dtype=int)
  x = np.delete(x, NoneIdx)
  y = np.delete(y, NoneIdx)
  NoneIdx = np.array([i for i, val in enumerate(y) if val is None or np.isnan(val)], dtype=int)
  x = np.delete(x, NoneIdx)
  y = np.delete(y, NoneIdx)

  return np.trapz(y, x)

def get_timespan(fpkl, param_name):
  (colmap,colapp,data) = load_data(fpkl)
  # find the index for "Time Elapsed" column header
  timeIdx = inverse_dict(colmap, "Time Elapsed")
  assert not timeIdx is None
  # find the index for "Character 0 Health" column header
  paramIdx = inverse_dict(colmap, param_name)
  assert not paramIdx is None 

  x_start = colapp[timeIdx]
  y_start = colapp[paramIdx]
  assert y_start >= x_start
  idx_start = y_start-x_start
  x = data[timeIdx][idx_start:]
  return x[-1]-x[0]

def plot_health_params(param_name, min_val, max_val, isLog=False):
  fig, ax = plt.subplots()
  ax.set_xlabel('Time Elapsed (hr)')
  ax.set_ylabel('Health (HP)')

  pkl_files = glob.glob("data/pickled/trial_*.pkl")
  # filter files with non numeric characters after trial_
  pkl_files = [f for f in pkl_files if f.split('_')[-1].split('.')[0].isnumeric()]
  pkl_files.sort()
  pkl_files = pkl_files[1:] # remove trial_0000.pkl
  nvalues = len(pkl_files)

  if isLog:
    param_values = np.logspace(np.log10(min_val), np.log10(max_val), num=nvalues)
    norm = LogNorm(vmin=min_val, vmax=max_val)
  else:
    param_values = np.linspace(min_val, max_val, num=nvalues)
    norm = plt.Normalize(vmin=min_val, vmax=max_val)

  color_values = np.linspace(0, 1, num=nvalues)
  integrated_health = []
  timespans = []
  # color by param_values, add colorbar, plot health over time
  for i in range(nvalues):
    color = plt.cm.viridis(color_values[i])
    plot_health(ax, pkl_files[i], f'{param_name}={param_values[i]}', color)
    integrated_health.append(integrate("Character 0 Health", pkl_files[i]))
    timespans.append(get_timespan(pkl_files[i], "Character 0 Health"))
  
  # add colorbar with ticks and labels
  sm = plt.cm.ScalarMappable(cmap=plt.cm.viridis, norm=norm)
  sm.set_array([])

  cbar = plt.colorbar(sm, ax=ax, label=param_name)
  if isLog:
      cbar.set_ticks(np.logspace(np.log10(min_val), np.log10(max_val), num=10))
  else:
      cbar.set_ticks(np.linspace(min_val, max_val, num=10))
  cbar.set_ticklabels([f'{val:.2e}' for val in cbar.get_ticks()])

  plt.savefig(f"data/plots/health_{param_name}.png")

  # plot integrated health vs param_values
  plt.figure()
  plt.scatter(param_values, integrated_health)
  plt.xlabel(param_name)
  plt.ylabel('Integrated Health')
  if isLog:
    plt.xscale('log')
  plt.title(f'Integrated Health vs {param_name}')
  plt.savefig(f"data/plots/integrated_health_{param_name}.png")

  # plot timespan vs param_values
  plt.figure()
  plt.scatter(param_values, timespans)
  plt.xlabel(param_name)
  plt.ylabel('Timespan (hr)')
  if isLog:
    plt.xscale('log')
  plt.title(f'Timespan vs {param_name}')
  plt.savefig(f"data/plots/timespan_{param_name}.png")

def plot_baseline_health():
  fig, ax = plt.subplots()

  ax.set_xlabel('Time Elapsed (hr)')
  ax.set_ylabel('Health (HP)')

  plot_health(ax, "data/pickled/trial_random.pkl", 'Random Actor')
  plot_health(ax, "data/pickled/trial_0001.pkl", 'AI Actor')

  ax.legend()
  # save the plot
  plt.savefig("data/plots/health_plot.png")

def main():
  plot_health_params('learning_rate_critic', 1e-6, 1e6, isLog=True)

if __name__ == "__main__":
  main()