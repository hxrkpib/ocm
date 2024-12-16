import posix_ipc
import mmap
    
class SharedMemorySemaphore:
    def __init__(self, name: str, initial_value: int):
        try:
            self.semaphore = posix_ipc.Semaphore("openrobot_ocm_" + name, posix_ipc.O_CREX, initial_value=initial_value)
            print(f"信号量{name}创建成功")
        except posix_ipc.ExistentialError:
            self.semaphore = posix_ipc.Semaphore("openrobot_ocm_" + name)
            print(f"信号量{name}已存在，已打开")
    def Increment(self):
        self.semaphore.release()
        
    def IncrementWhenZero(self):
        if self.semaphore.value == 0:
            self.semaphore.release()

    def IncrementValue(self, value: int):
        for _ in range(value):
            self.semaphore.release()

    def Decrement(self):
        self.semaphore.acquire()
        
    def TryDecrement(self):
        success = True
        try:
            self.semaphore.acquire(0)
            success = True
        except posix_ipc.BusyError:
            success = False
        return success
    
    def DecrementTimeout(self, timeout: int):
        success = True
        try:
            self.semaphore.acquire(timeout)
            success = True
        except posix_ipc.BusyError:
            success = False
        return success
    
    def Close(self):
        self.semaphore.close()

    def Destroy(self):
        self.semaphore.unlink()
        
class SharedMemory:
    def __init__(self, name: str, check_size: bool, size: int = 0):
        self.sem=SharedMemorySemaphore(name+"_shm", 1)
        self.name = "openrobot_ocm_" + name 
        self.check_size = check_size
        self.size = size
        try:
            self.shm = posix_ipc.SharedMemory(self.name, posix_ipc.O_CREX, size=self.size)
            print(f"共享内存{name}创建成功")
        except posix_ipc.ExistentialError:
            self.shm = posix_ipc.SharedMemory(self.name)
            print(f"共享内存{name}已存在，已打开")
            if self.check_size:
                if self.shm.size != self.size:
                    raise Exception("共享内存大小不一致")
            self.size = self.shm.size
        self.data = mmap.mmap(self.shm.fd, self.size)
    
    def WriteData(self, data):
        self.data.seek(0)
        self.data.write(data)
    
    def ReadData(self):
        self.data.seek(0)
        return self.data.read()

    def Lock(self):
        self.sem.Decrement()
        
    def UnLock(self):
        self.sem.Increment()
    
    def Close(self):
        self.shm.close()
    
    def Destroy(self):
        self.shm.unlink()
class SharedMemoryTopic:
    def __init__(self):
        self.sem = {}
        self.shm = {}
        
    def CheckSemExist(self, topic_name: str):
        if topic_name not in self.sem:
            self.sem[topic_name] = SharedMemorySemaphore(topic_name, 0)
            
    def CheckSHMExist(self, topic_name: str, check_size: bool, size: int = 0):
        if topic_name not in self.shm:
            self.shm[topic_name] = SharedMemory(topic_name, check_size, size)
    
    def PublishSem(self, topic_name: str):
        self.CheckSemExist(topic_name)
        self.sem[topic_name].IncrementWhenZero()
    
    def WriteDataToSHM(self, topic_name: str, data):
        buf=data.encode()
        datalen=len(buf)
        self.CheckSHMExist(topic_name, True, datalen)
        self.shm[topic_name].Lock()
        self.shm[topic_name].WriteData(buf)
        self.shm[topic_name].UnLock()
        self.PublishSem(topic_name)
    
    def Publish(self, topic_name: str, shm_name: str, data):
        self.WriteDataToSHM(shm_name, data)
        self.PublishSem(topic_name)
    def PublishList(self, topic_names: list[str], shm_name: str, data: list):
        self.WriteDataToSHM(shm_name, data)
        for topic_name in topic_names:
            self.PublishSem(topic_name)
            
    def Subscribe(self, topic_name: str, shm_name: str, callback, lcm_type):
        self.CheckSemExist(topic_name)
        self.sem[topic_name].Decrement()
        self.CheckSHMExist(shm_name, False)
        self.shm[shm_name].Lock()
        data=lcm_type.decode(self.shm[shm_name].ReadData())
        self.shm[shm_name].UnLock()
        callback(data)
        
    def SubscribeNoWait(self, topic_name: str, shm_name: str, callback,lcm_type):
        self.CheckSemExist(topic_name)
        if self.sem[topic_name].TryDecrement():
            self.CheckSHMExist(shm_name, False)
            self.shm[shm_name].Lock()
            data=lcm_type.decode(self.shm[shm_name].ReadData())
            self.shm[shm_name].UnLock()
            callback(data)

    def SubscribeTimeout(self, topic_name: str, shm_name: str, callback, lcm_type, timeout: int):
        self.CheckSemExist(topic_name)
        if self.sem[topic_name].DecrementTimeout(timeout):
            self.CheckSHMExist(shm_name, False)
            self.shm[shm_name].Lock()
            data=lcm_type.decode(self.shm[shm_name].ReadData())
            self.shm[shm_name].UnLock()
            callback(data)
            