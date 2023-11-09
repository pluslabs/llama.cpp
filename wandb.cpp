#include "wandb.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <json.hpp>
#include <thread> // for sleep

using json = nlohmann::json;

FILE* pipe;

/**
 * Background thread function that monitors the output of the wandb Python process.
*/
void background_monitor_python_proc() {
   // Create a buffer to read the output
    char buffer[128];

    // Read and continuously log the child process's output
    while (true) {
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            // Log the output to stdout with color green and prefix "wandb: "
            std::cout << "\033[1;32mwandb: \033[0m" << buffer << std::endl;
            
        } else {
            // If fgets returns nullptr, the child process has closed its stdout
            break;
        }
    }
}

/**
 * Initializes the wandb process with the given run name and parameter JSON.
 * @param runName The name of the wandb run.
 * @param configJson The JSON object containing the hyper parameters to be logged.
 * @return 0 if the initialization was successful, 1 otherwise.
 */
int wandb_init(std::string runName, json configJson) {
    std::string command = "python wandb_interface.py";

    // Create a pipe to communicate with the Python process
    pipe = popen(command.c_str(), "w");

    
    if (!pipe) {
        std::cerr << "Error launching Python wandb process" << std::endl;
        return 1;
    }

    // Stringify configJson and escape characters for command line
    std::string configJsonString = configJson.dump();
    std::string::size_type pos = 0;
    while ((pos = configJsonString.find("\\", pos)) != std::string::npos) {
        configJsonString.replace(pos, 1, "\\\\");
        pos += 2;
    }

    // Pipe in the command to initialize wandb with the configJsonString
    fprintf(pipe, "initialize_wandb magic-llama-cpp %s %s\n", runName.c_str(), configJsonString.c_str());

    // Create a background thread for background_monitor_python_proc
    std::thread backgroundThread(background_monitor_python_proc);

    // Wait for the background thread to finish (optional)
    backgroundThread.join();
    
    return 0;
}

/**
 * Runs wandb log with the given json data.
 * @param logJson The json data to log.
 * @return 0 if successful, 1 otherwise.
 */
int wandb_log(json logJson) {
    if (!pipe) {
        std::cerr << "No running wandb pipe" << std::endl;
        return 1;
    }

    // Stringify paramJson and escape characters for command line
    std::string paramJsonString = logJson.dump();
    std::string::size_type pos = 0;
    while ((pos = paramJsonString.find("\\", pos)) != std::string::npos) {
        paramJsonString.replace(pos, 1, "\\\\");
        pos += 2;
    }

    // Pipe in the command to call log_data wandb with the paramJsonString
    fprintf(pipe, "log_data %s\n", paramJsonString.c_str());

    // Create a background thread for background_monitor_python_proc
    std::thread backgroundThread(background_monitor_python_proc);

    // Wait for the background thread to finish (optional)
    backgroundThread.join();
    return 0;
}

/**
 * Runs wandb finish.
 * @return 0 if successful, 1 otherwise.
*/
int wandb_finish() {
    if (!pipe) {
        std::cerr << "No running wandb pipe" << std::endl;
        return 1;
    }

    fprintf(pipe, "finish\n");

    // Create a background thread for background_monitor_python_proc
    std::thread backgroundThread(background_monitor_python_proc);

    // Wait for the background thread to finish (optional)
    backgroundThread.join();

    // Close the pipe when done
    pclose(pipe);

    return 0;
}