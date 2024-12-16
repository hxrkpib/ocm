import shared_memory_topic
import MyData

def callback(data):
    print(data.count)
    for i in range(10):
        print(data.values[i], end=" ")
    print()

if __name__ == "__main__":
    topic=shared_memory_topic.SharedMemoryTopic()
    while True:
        topic.Subscribe("topic1", "shm1", callback, MyData.MyData)
            