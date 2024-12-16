#include "node/node_map.hpp"
#include <iostream>
#include "node/node.hpp"

using namespace ocm;

// 定义一个继承自NodeBase的NodeA类
class NodeA : public NodeBase {
 public:
  // 构造函数，初始化节点名称
  NodeA(const std::string& node_name) : NodeBase(node_name) {}

  // 重写Construct方法，构造节点
  void Construct() override { std::cout << "NodeA Construct" << std::endl; }

  // 重写Init方法，初始化节点
  void Init() override { std::cout << "NodeA Init" << std::endl; }

  // 重写Execute方法，执行节点
  void Execute() override { std::cout << "NodeA Run" << std::endl; }

  // 重写Output方法，输出节点信息
  void Output() override { std::cout << "NodeA Output" << std::endl; }

  // 重写TryEnter方法，尝试进入节点
  bool TryEnter() override {
    std::cout << "NodeA TryEnter" << std::endl;
    return true;
  }

  // 重写TryExit方法，尝试退出节点
  bool TryExit() override {
    std::cout << "NodeA TryExit" << std::endl;
    return true;
  }

  // 重写AfterExit方法，退出后处理
  void AfterExit() override { std::cout << "NodeA AfterExit" << std::endl; }
};

int main() {
  // 创建节点映射对象，并添加节点
  std::shared_ptr<NodeMap> node_map = std::make_shared<NodeMap>();
  node_map->AddNode("NodeA", std::make_shared<NodeA>("NodeA"));  // 添加NodeA节点

  // 调用节点的初始化、执行和输出方法
  node_map->GetNodePtr("NodeA")->Init();     // 初始化节点
  node_map->GetNodePtr("NodeA")->Execute();  // 执行节点
  node_map->GetNodePtr("NodeA")->Output();   // 输出节点信息

  // 输出节点的当前状态
  std::cout << "NodeA state: " << static_cast<int>(node_map->GetNodePtr("NodeA")->GetState()) << std::endl;

  // 调用节点的进入检查和退出检查方法
  node_map->GetNodePtr("NodeA")->TryEnter();
  node_map->GetNodePtr("NodeA")->TryExit();
  node_map->GetNodePtr("NodeA")->AfterExit();
  return 0;  // 程序结束
}
