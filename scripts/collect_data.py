import os
import subprocess
import yaml
import glob
import sys

import matplotlib.pyplot as plt

if __name__ == "__main__":
  sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
  __package__ = "scripts"

from auxiliary import data_reader as dr
from auxiliary import data_plotting as dp

# generator for altering a parameter in a config in range
def get_configs(fconfig, param_name, start, end, num_steps, isLog=False):
  # read the existing yaml file from the config directory
  config_path = os.path.join('config', fconfig)
  with open(config_path, 'r') as file:
    config = yaml.load(file, Loader=yaml.FullLoader)
  
  isConfigData = fconfig == 'Data.yaml'
  if not isConfigData:
    # read the default data config
    data_config_path = os.path.join('config', 'Data.yaml')
    with open(data_config_path, 'r') as file:
      data_config = yaml.load(file, Loader=yaml.FullLoader)
  else: # point to the same config
    data_config = config

  # generate the new configs
  for i in range(num_steps):
    if isLog:
      new_val = start * (end/start)**(i/(num_steps-1))
    else:
      new_val = start + i*(end-start)/(num_steps-1)
    new_config = config.copy()
    new_config[param_name] = new_val
    data_config["filename"] = f"trial_{1+i:04d}.dat"
    if not isConfigData:
      yield [(fconfig, new_config), ('Data.yaml', data_config)]
    else:
      yield [(fconfig, new_config)]

# configs is a list of tuples, 
# the first element is the filename, eg. Data.yaml
# the second element is the yaml dictionary
def run_trial(configs):
  # copy the yaml files from config/ to config_trial/
  yaml_files = glob.glob('config/*.yaml')
  for f in yaml_files:
    os.system(f"cp {f} config_trial/")

  # overwrite yaml files with the new configs
  for filename, config in configs:
    with open(f'config_trial/{filename}', 'w') as file:
      yaml.dump(config, file)

  args = glob.glob('config_trial/*.yaml')

  # Run the GridWorldApp executable
  exe = "bin/GridWorldApp"
  args = ""
  for f in yaml_files:
    args += f"{f} "
  exe_string = f"{exe} {args}"
  print(f"Running {exe_string}")
  subprocess.run(exe_string, shell=True)

def run_trials(gen_configs):
  count = 0
  for config in gen_configs:
    count += 1
    run_trial(config)
    # read the data from data/raw/trial_{count}.dat
    data = dr.read_data(f"data/raw/trial_{count:04d}.dat")
    # save the data to data/pickled/trial_{count}.pkl
    dr.save_data(data, f"data/pickled/trial_{count:04d}.pkl")

if __name__ == "__main__":
  generator = get_configs('SmartActor.yaml', 'learning_rate_actor', 1e-6, 1e6, 20, isLog=True)

  os.makedirs('config_trial', exist_ok=True)

  run_trials(generator)

  # Plot the health data
  fig, ax = plt.subplots()
  ax.set_xlabel('Time Elapsed (hr)')
  ax.set_ylabel('Health (HP)')
  dp.plot_health(ax, "data/pickled/trial_random.pkl", 'Random Actor')
  dp.plot_health(ax, "data/pickled/trial_crafted.pkl", 'Hand Crafted Actor')
  dp.plot_health(ax, "data/pickled/trial_0001.pkl", 'AI Actor')
  ax.legend()
  plt.savefig('data/plots/health.png')

  # Clean up the config_trial directory
  os.system("rm -rf config_trial")