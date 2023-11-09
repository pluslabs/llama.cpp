#pragma once

#ifndef WANDB_CPP
#define WANDB_CPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <json.hpp>

using json = nlohmann::json;

extern int wandb_init(std::string projectName, json paramJson);
extern int wandb_log(json logJson);
extern int wandb_finish();

#endif // WANDB_CPP