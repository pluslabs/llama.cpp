import wandb
import json
import sys

def initialize_wandb(project_name, run_name, config):
    config_json = json.loads(config)
    global wandb_run
    wandb_run = wandb.init(project=project_name, name=run_name, config=config_json)

def log_data(data):
    # Print data for debugging in color blue with prefix "wandb: "
    print("\033[94mwandb: \033[0m" + data)
    if wandb_run is None:
        print("Error: wandb_run is not initialized")
        return
    wandb_run.log(json.loads(data))

def finish_wandb_run():
    wandb.finish()


# Listen for commands from the C++ program as main routine
if __name__ == "__main__":
    for line in sys.stdin:
        # Read line from stdin with first word being the function name and the rest being the arguments
        function_name, *args = line.split()

        # Print the function name for debugging in color blue and with prefix "wandb: "
        print("\033[94mwandb: \033[0m" + function_name)

        # Call the corresponding function with the provided arguments
        if function_name == 'initialize_wandb':
            initialize_wandb(*args)
        elif function_name == 'log_data':
            log_data(*args)
        elif function_name == 'finish_wandb_run':
            finish_wandb_run()
