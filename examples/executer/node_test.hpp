#include <iostream>
#include "node/node.hpp"

class NodeA : public ocm::NodeBase {
 public:
  NodeA(const std::string& node_name) : ocm::NodeBase(node_name) {}
  void Construct() override { std::cout << "NodeA Construct" << std::endl; }
  void Init() override { std::cout << "NodeA Init" << std::endl; }
  void Execute() override { std::cout << "NodeA Run" << std::endl; }
  void Output() override { std::cout << "NodeA Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeA TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeA TryExit" << std::endl;
    return true;
  }
  void AfterExit() override { std::cout << "NodeA AfterExit" << std::endl; }
};

class NodeB : public ocm::NodeBase {
 public:
  NodeB(const std::string& node_name) : ocm::NodeBase(node_name) {}
  void Construct() override { std::cout << "NodeB Construct" << std::endl; }
  void Init() override { std::cout << "NodeB Init" << std::endl; }
  void Execute() override { std::cout << "NodeB Run" << std::endl; }
  void Output() override { std::cout << "NodeB Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeB TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeB TryExit" << std::endl;
    return true;
  }
  void AfterExit() override { std::cout << "NodeB AfterExit" << std::endl; }
};

class NodeC : public ocm::NodeBase {
 public:
  NodeC(const std::string& node_name) : ocm::NodeBase(node_name) {}
  void Construct() override { std::cout << "NodeC Construct" << std::endl; }
  void Init() override { std::cout << "NodeC Init" << std::endl; }
  void Execute() override { std::cout << "NodeC Run" << std::endl; }
  void Output() override { std::cout << "NodeC Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeC TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeC TryExit" << std::endl;
    return true;
  }
  void AfterExit() override { std::cout << "NodeC AfterExit" << std::endl; }
};

class NodeD : public ocm::NodeBase {
 public:
  NodeD(const std::string& node_name) : ocm::NodeBase(node_name) {}
  void Construct() override { std::cout << "NodeD Construct" << std::endl; }
  void Init() override { std::cout << "NodeD Init" << std::endl; }
  void Execute() override { std::cout << "NodeD Run" << std::endl; }
  void Output() override { std::cout << "NodeD Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeD TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeD TryExit" << std::endl;
    return true;
  }
  void AfterExit() override { std::cout << "NodeD AfterExit" << std::endl; }
};

class NodeE : public ocm::NodeBase {
 public:
  NodeE(const std::string& node_name) : ocm::NodeBase(node_name) {}
  void Construct() override { std::cout << "NodeE Construct" << std::endl; }
  void Init() override { std::cout << "NodeE Init" << std::endl; }
  void Execute() override { std::cout << "NodeE Run" << std::endl; }
  void Output() override { std::cout << "NodeE Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeE TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeE TryExit" << std::endl;
    return true;
  }
  void AfterExit() override { std::cout << "NodeE AfterExit" << std::endl; }
};
