#ifndef HPC_AIO_COMMON_HPC_AIO_DATA_STRUCTURE_H
#define HPC_AIO_COMMON_HPC_AIO_DATA_STRUCTURE_H
#include <cstring>
#include <functional>
#include <hpc_aio/hpc_aio_config.hpp>
#include <list>
#include <memory>
#include <set>
#include <string>

#define NO_OP_FUNCTION "NOOP"

namespace hpc_aio {
struct Request {
  /* data */
  void* data;
};

struct FileRequest : public Request {
  int fd;
  std::string fname;
};

typedef int (*NodeFunction)(Request*);
typedef int (*FileNodeFunction)(FileRequest*);

enum NodeType { NOOP = 0, IO = 1, PREPROCESSING = 2, ROOT = 3, MERGE = 4 };

int read_file(Request* request) {
  printf("Read file %s\n", (char*)request->data);
  return 0;
}

struct Node {
  NodeType type;
  std::string function_name;  // FIXME(hari): replace with CharStruct
  Request* request;
  Node() : type(NodeType::NOOP), function_name(), request(nullptr) {}
  Node(NodeType _type, std::string _function_name, Request* _request)
      : type(_type), function_name(_function_name), request(_request) {}
  Node(const Node& other)
      : type(other.type),
        function_name(other.function_name),
        request(other.request) {} /* copy constructor*/
  Node(Node&& other)
      : type(other.type),
        function_name(other.function_name),
        request(other.request) {} /* move constructor*/
};

template <typename R, typename F>
class Graph {
 protected:
  std::vector<std::vector<uint16_t>> graph;
  std::vector<std::vector<uint16_t>> dep_graph;
  std::vector<Node> nodes;
  uint16_t leaf_start_index;
  Node* root_node;
  Node* current_nodes;
  uint32_t node_length;

 public:
  Graph() {
    graph.push_back(std::vector<uint16_t>());
    nodes.push_back(Node(NodeType::ROOT, NO_OP_FUNCTION, nullptr));
    dep_graph.push_back(std::vector<uint16_t>());
    leaf_start_index = 0;
  }

  Graph* Add(NodeType _type, const char* _function_name, F function,
             R* request = nullptr) {
    if (_type != NodeType::MERGE) {
      auto size_before = graph.size();
      auto new_leaf_start_index = graph.size();
      for (uint16_t i = leaf_start_index; i < size_before; ++i) {
        graph[i].push_back(new_leaf_start_index);
        graph.push_back(std::vector<uint16_t>());
        dep_graph.push_back(std::vector<uint16_t>());
        dep_graph[new_leaf_start_index].push_back(i);
        nodes.push_back(Node(_type, _function_name, request));
        new_leaf_start_index = graph.size();
      }
      leaf_start_index = size_before;
    } else {
      auto new_leaf_start_index = graph.size();
      dep_graph.push_back(std::vector<uint16_t>());
      for (uint16_t i = leaf_start_index; i < graph.size(); ++i) {
        graph[i].push_back(new_leaf_start_index);
        dep_graph[new_leaf_start_index].push_back(i);
      }
      graph.push_back(std::vector<uint16_t>());
      nodes.push_back(Node(_type, _function_name, request));
      leaf_start_index = new_leaf_start_index;
    }

    return this;
  }

  int Run(std::set<uint16_t>& completed, uint16_t current = 0) {
    for (auto dependent : dep_graph[current]) {
      if (completed.find(dependent) == completed.end()) {
        printf("Dependency not met for node %d with function %s\n", current,
               nodes[current].function_name.c_str());
        return 0;
      }
    }
    printf("Running node %d with function %s\n", current,
           nodes[current].function_name.c_str());
    completed.insert(current);
    for (auto child : graph[current]) {
      Run(completed, child);
    }
    return 0;
  }
};
class FileReader : public Graph<FileRequest, FileNodeFunction> {
 public:
  FileReader(std::vector<std::string> files) : Graph() {
    auto new_leaf_start_index = graph.size();
    uint16_t current_index = graph.size();
    auto files_index = std::vector<uint16_t>();
    for (auto file : files) {
      graph.push_back(std::vector<uint16_t>());
      dep_graph.push_back(std::vector<uint16_t>());
      files_index.push_back(current_index++);
      auto request = new FileRequest();
      request->fname = file;
      nodes.push_back(Node(NodeType::IO, "read_file", request));
    }
    for (uint16_t i = leaf_start_index; i < new_leaf_start_index; ++i) {
      graph[i] = files_index;
      for (auto findex : files_index) {
        dep_graph[findex].push_back(i);
      }
    }
    leaf_start_index = new_leaf_start_index;
  }
  FileReader* Add(NodeType _type, const char* _function_name,
                  FileNodeFunction function, FileRequest* request = nullptr) {
    Graph::Add(_type, _function_name, function, request);
    return this;
  }
};
}  // namespace hpc_aio
#endif  // HPC_AIO_COMMON_HPC_AIO_DATA_STRUCTURE_H