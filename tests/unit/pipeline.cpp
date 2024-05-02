#include "hpc_aio/common/data_structure.h"

int read_data(hpc_aio::FileRequest* request) {
  printf("Read Data");
  return 0;
}

int cache_data(hpc_aio::FileRequest* request) {
  printf("Writting to Local SSD");
  return 0;
}
int batch_data(hpc_aio::FileRequest* request) {
  printf("Writting to Local SSD");
  return 0;
}

int main(const int argc, char* argv[]) {
  std::vector<std::string> filenames = {"file1.dat", "file2.dat"};
  auto graph = hpc_aio::FileReader(filenames);
  graph.Add(hpc_aio::NodeType::IO, "read_data", read_data);
  graph.Add(hpc_aio::NodeType::IO, "cache_data", cache_data);
  graph.Add(hpc_aio::NodeType::MERGE, "batch_data", batch_data);
  auto completed = std::set<uint16_t>();
  graph.Run(completed);
  return 0;
}