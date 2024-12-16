# setup.py

from setuptools import setup, find_packages

setup(
    name='shared_memory_topic',  # 包名
    version='1.0',  # 版本号
    author='hxrkpib',
    author_email='hxrkpib@foxmail.com',
    description='共享内存话题, 用于进程间通信',
    url='', 
    packages=find_packages(),
    install_requires=[
        'posix_ipc'
    ],
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License', 
        'Operating System :: OS Independent',
    ],
    python_requires='>=3.6',
)
