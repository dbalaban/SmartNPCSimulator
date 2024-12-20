import os
import subprocess
import yaml
import glob
import sys

import matplotlib.pyplot as plt

if __name__ == "__main__" and __package__ is None:
  sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
  __package__ = "scripts"

from auxiliary import data_reader as dr
from auxiliary import data_plotting as dp

def run_trial(config, fconfig):
  # if fconfig exists, replace it
  if os.path.exists(fconfig):
    os.remove(fconfig)
  with open(fconfig, 'w') as file:
    yaml.dump(config, file)
  
  # list other yaml files in the config directory
  yaml_files = glob.glob('config/*.yaml')
  # filter out the Data.yaml file
  yaml_files = [f for f in yaml_files if 'Data.yaml' not in f]
  yaml_files.append(fconfig)

  # Run the GridWorldApp executable
  exe = "bin/GridWorldApp"
  args = ""
  for f in yaml_files:
    args += f"{f} "
  exe_string = f"{exe} {args}"
  print(f"Running {exe_string}")
  subprocess.run(exe_string, shell=True)

if __name__ == "__main__":
  # Define the content of the Data.yaml file
  trial_random_data_config = {
    "max_time": 10000,
    "actor_type": "random",
    "filename": "trial_random.dat",
    "directory": "data/raw/"
  }

  trial_crafted_data_config = {
    "max_time": 10000,
    "actor_type": "crafted",
    "filename": "trial_crafted.dat",
    "directory": "data/raw/"
  }

  trial_0001_data_config = {
    "max_time": 10000,
    "actor_type": "smart",
    "filename": "trial_0001.dat",
    "directory": "data/raw/"
  }

  # Define the path to the Data.yaml file
  config_dir = 'config_trial'
  # create directory if it does not exist
  os.makedirs(config_dir, exist_ok=True)
  data_yaml_path = os.path.join(config_dir, 'Data.yaml')

  # run_trial(trial_random_data_config, data_yaml_path)
  # reader = dr.DataReader('data/raw/trial_random.dat')
  # dr.rePickle(reader, 'data/pickled/trial_random.pkl')

  run_trial(trial_crafted_data_config, data_yaml_path)
  reader = dr.DataReader('data/raw/trial_crafted.dat')
  dr.rePickle(reader, 'data/pickled/trial_crafted.pkl')

  # run_trial(trial_0001_data_config, data_yaml_path)
  # reader = dr.DataReader('data/raw/trial_0001.dat')
  # dr.rePickle(reader, 'data/pickled/trial_0001.pkl')

  # Plot the health data
  fig, ax = plt.subplots()
  ax.set_xlabel('Time Elapsed (hr)')
  ax.set_ylabel('Health (HP)')
  dp.plot_health(ax, "data/pickled/trial_random.pkl", 'Random Actor')
  dp.plot_health(ax, "data/pickled/trial_crafted.pkl", 'Hand Crafted Actor')
  dp.plot_health(ax, "data/pickled/trial_0001.pkl", 'AI Actor')
  ax.legend()
  plt.savefig('data/plot/health.png')

  # Clean up the Data.yaml file
  os.remove(data_yaml_path)