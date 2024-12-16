#include <iostream>
#include <ocm/write_read_lock_data.hpp>
#include <thread>

// 示例函数，使用读锁读取数据
void ReadData(ocm::RWLockData<int>& rwData) {
  rwData.LockRead();                                             // 获取读锁
  std::cout << "Read value: " << *rwData.GetPtr() << std::endl;  // 输出读取到的值
  rwData.UnlockRead();                                           // 释放读锁
}

// 示例函数，使用写锁修改数据
void WriteData(ocm::RWLockData<int>& rwData, int newValue) {
  rwData.LockWrite();                                                  // 获取写锁
  *rwData.GetPtr() = newValue;                                         // 修改数据
  std::cout << "Updated value to: " << *rwData.GetPtr() << std::endl;  // 输出更新后的值
  rwData.UnlockWrite();                                                // 释放写锁
}

int main() {
  ocm::RWLockData<int> rwData(10);  // 初始化数据为10

  // 使用多个线程来读取和写入共享数据
  std::thread t1(ReadData, std::ref(rwData));       // 线程1：读取数据
  std::thread t2(WriteData, std::ref(rwData), 20);  // 线程2：更新数据为20
  std::thread t3(ReadData, std::ref(rwData));       // 线程3：读取数据

  // 等待所有线程完成
  t1.join();
  t2.join();
  t3.join();

  return 0;  // 程序结束
}
