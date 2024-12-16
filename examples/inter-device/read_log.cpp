#include <stdio.h>

#include <lcm/lcm-cpp.hpp>

#include "exlcm/example_t.hpp"

int main(int argc, char **argv) {
  // 检查是否提供了日志文件路径
  if (argc < 2) {
    fprintf(stderr, "usage: read_log <logfile>\n");
    return 1;
  }

  // 打开日志文件
  lcm::LogFile log(argv[1], "r");
  if (!log.good()) {
    perror("LogFile");
    fprintf(stderr, "couldn't open log file %s\n", argv[1]);
    return 1;
  }

  while (1) {
    // 读取一个日志事件
    const lcm::LogEvent *event = log.readNextEvent();
    if (!event) break;  // 如果没有更多事件，退出循环

    // 只处理来自EXAMPLE频道的消息
    if (event->channel != "EXAMPLE") continue;

    // 尝试解码消息
    exlcm::example_t msg;
    if (msg.decode(event->data, 0, event->datalen) != event->datalen) continue;  // 如果解码失败，跳过此事件

    // 解码成功！打印消息内容
    printf("Message:\n");
    printf("  timestamp   = %lld\n", (long long)msg.timestamp);                                                                    // 打印时间戳
    printf("  position    = (%f, %f, %f)\n", msg.position[0], msg.position[1], msg.position[2]);                                   // 打印位置
    printf("  orientation = (%f, %f, %f, %f)\n", msg.orientation[0], msg.orientation[1], msg.orientation[2], msg.orientation[3]);  // 打印朝向
    printf("  ranges:");
    for (int i = 0; i < msg.num_ranges; i++) printf(" %d", msg.ranges[i]);  // 打印距离信息
    printf("\n");
    printf("  name        = '%s'\n", msg.name.c_str());  // 打印名称
    printf("  enabled     = %d\n", msg.enabled);         // 打印启用状态
  }

  // 当log变量超出作用域时，日志文件会自动关闭

  printf("done\n");
  return 0;  // 程序结束
}
