#include <iostream>
#include <thread>
#include "ocm/atomic_ptr.hpp"

// 示例函数，使用AtomicPtr修改数据
void UpdateData(ocm::AtomicPtr<int>& atomicPtr) {
  // 以线程安全的方式修改数据
  atomicPtr = 10;

  // 获取当前值并输出
  std::shared_ptr<const int> value = atomicPtr.GetPtr();
  std::cout << "Updated value: " << *value << std::endl;
}

int main() {
  // 创建一个初始化为值5的AtomicPtr对象
  ocm::AtomicPtr<int> atomicInt(5);  // 初始值为5

  // 使用多个线程来修改和读取原子数据
  std::thread t1(UpdateData, std::ref(atomicInt));  // 创建线程1
  std::thread t2(UpdateData, std::ref(atomicInt));  // 创建线程2

  // 等待两个线程完成
  t1.join();
  t2.join();

  return 0;  // 程序结束
}
