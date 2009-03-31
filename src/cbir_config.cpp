
#include "cbir_config.h"

void operator >> (const YAML::Node& node, cbir_component& cbir_config) {
  node["dimension"] >> cbir_config.dim;
  const YAML::Node& files = node["feature-files"];
  for(unsigned i = 0; i < files.size(); i++) {
    std::string filename;
    files[i] >> filename;
    cbir_config.feature_files.push_back(filename);
  }

  const YAML::Node& filter_files = node["filter-files"];
  for(unsigned i = 0; i < filter_files.size(); i++) {
    std::string filename;
    filter_files[i] >> filename;
    std::cout << filename << std::endl;
    cbir_config.filter_files.push_back(filename);
  }

  const YAML::Node& sample_files = node["sample-files"];
  for(unsigned i = 0; i < sample_files.size(); i++) {
    std::string filename;
    sample_files[i] >> filename;
    std::cout << filename << std::endl;
    cbir_config.sample_files.push_back(filename);
  }


}

void operator >> (const YAML::Node& node, http_server& server_config) {
  node["port"] >> server_config.port;
}

bool load_config(char* filename, http_server& server_config, cbir_component& cbir_config) {

  std::ifstream fin(filename);

  try {
    YAML::Parser parser(fin);
    YAML::Node doc;
    parser.GetNextDocument(doc);

    doc["http-server"] >> server_config;
    doc["cbir-component"] >> cbir_config;

    std::cout << "port: " << server_config.port << std::endl;
    std::cout << "dim: " << cbir_config.dim << std::endl;

    for (int i = 0; i < cbir_config.feature_files.size(); i++) {
      std::cout << "feature file: " << cbir_config.feature_files[i] << std::endl;
    }

  } catch(YAML::Exception& e) {
    std::cout << "Error at line " << e.line+1 << ", col " << e.column+1 << ": " << e.msg << "\n";
    return false;
  }

  return true;

}

