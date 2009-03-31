
#ifndef CBIR_CONFIG

#define CBIR_CONFIG

#include "yaml.h"
#include <fstream>
#include <iostream>

struct http_server {
  int port;
};

struct cbir_component {
  int dim;
  std::vector <std::string> feature_files;
  std::vector <std::string> filter_files;
  std::vector <std::string> sample_files;
};

void operator >> (const YAML::Node& node, cbir_component& cbir_config);
void operator >> (const YAML::Node& node, http_server& server_config);

bool load_config(char* filename, http_server& server_config, cbir_component& cbir_config);

#endif

