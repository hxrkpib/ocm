import shared_memory_topic
import MyData
if __name__ == "__main__":
    topic=shared_memory_topic.SharedMemoryTopic()
    mydata=MyData.MyData()
    mydata.count=1
    mydata.values=[1,2,3,4,5,6,7,8,9,10]
    topic.Publish("topic1", "shm1", mydata)
            
