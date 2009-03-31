
#include "embeded_web_server.h"
#include "ann_service.h"
#include "cbir_config.h"

int main(int argc, char **argv) {

  if (argc < 2) {
    cout << "Usage: cbir_server [config_file]" << endl;
    exit(1);
  }

  http_server server_config;
  cbir_component cbir_config;

  if (load_config(argv[1], server_config, cbir_config)) {  
    main_loop_for_web_server(server_config, cbir_config);
  }

  return 0;

}

