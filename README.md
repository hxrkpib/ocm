# OCM - One Communication Middleware

## 1. 项目介绍 
OCM 是一个用于机器人通信的中间件，提供了一个通用的通信、任务执行框架，支持多种通信协议和数据格式。  

## 2. 项目结构

### 2.1 通信

#### 2.1.1 进程内通信 
- `ocm/atomic_ptr.hpp`：原子指针，提供线程安全的指针操作。
- `ocm/writer_reader_lock.hpp`：读写锁，提供读写锁操作。
- 参照`examples/intra-process`：进程内通信示例。

#### 2.1.2 进程间通信
- `ocm/shared_memory_topic.hpp`：共享内存话题，提供共享内存发布订阅功能。
- `ocm/python/shared_memory_topic`：共享内存话题Python实现。
- 参照`examples/inter-process`：进程间通信示例。

#### 2.1.3 设备间通信
- [LCM](https://lcm-proj.github.io/lcm/)  
- 参照`examples/inter-device`：设备间通信示例。

#### 2.1.4 序列化
- [LCM](https://lcm-proj.github.io/lcm/)  

### 2.2 任务执行

#### 2.2.1 节点管理
- `node/node.hpp`：节点，最小执行单元。
- `node/node_map.hpp`：节点管理，提供节点管理功能。
- 参照`examples/node`：节点示例。

#### 2.2.2 任务管理
- `task/task_base.hpp`：任务基类，提供定时器、线程管理功能。
- 参照`examples/task`：任务示例。

#### 2.2.3 调度器
- `executer/executer.hpp`：调度器，提供任务调度功能。
- 参照`examples/executer`：调度器示例。

## 2.3 日志
- `log_anywhere/log_anywhere.hpp`：提供进程内共享的异步日志功能。
- [spdlog](https://github.com/gabime/spdlog)  
- 参照`examples/log_anywhere`：日志示例。

## 2.4 数据调试
- `debug_anywhere/debug_anywhere.hpp`：提供进程内共享的调试数据异步发布功能。
- 参照`examples/debug_anywhere`：数据调试示例。

## 2.5 参数
- 参照`parameter_generator/ocm-parmgen.py`：提供参数的C++数据类自动生成工具。

## 3. 依赖
- [LCM](https://lcm-proj.github.io/lcm/)
- [spdlog](https://github.com/gabime/spdlog)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [boost](https://www.boost.org/)
- 可使用[OpenRobotOS/third_party](https://github.com/OpenRobotOS/third_party.git)安装

## 4. 环境要求
- gcc >= 13
- cmake >= 3.10

## 5. 编译安装
```bash
mkdir build
cd build
cmake .. # 支持ROS2消息类型  -DSUPPORT_ROS2=ON -DROS_DISTRO=$ROS_DISTRO
sudo make install -j # 默认安装到/opt/openrobotlib/ocm，默认依赖位置/opt/openrobotlib/third_party
# 可选python共享内存话题安装
pip install posix_ipc
cd ocm/python/shared_memory_topic
pip install .
```
