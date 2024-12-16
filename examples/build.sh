#!/bin/bash

# 设置 ROOT_FOLDER 为脚本所在目录
ROOT_FOLDER="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# 用于记录编译成功的文件夹
compiled_folders=()

# 遍历根目录下的所有子文件夹
for folder in "$ROOT_FOLDER"/*; do
    if [ -d "$folder" ]; then
        # 如果文件夹中有 CMakeLists.txt 文件
        if [ -f "$folder/CMakeLists.txt" ]; then
            # 检查是否存在 build 目录
            BUILD_DIR="$folder/build"
            if [ -d "$BUILD_DIR" ]; then
                # 如果 build 目录存在，清空其中内容
                echo "Clearing build directory: $BUILD_DIR"
                rm -rf "$BUILD_DIR"/*
            else
                # 如果 build 目录不存在，创建它
                echo "Creating build directory: $BUILD_DIR"
                mkdir "$BUILD_DIR"
            fi

            # 进入 build 目录并执行 cmake 和 make
            cd "$BUILD_DIR"
            echo "Running cmake .. and make -j in $BUILD_DIR"
            cmake ..
            if make -j; then
                # 记录成功编译的文件夹
                compiled_folders+=("$folder")
            else
                echo "Build failed in $folder"
            fi
            cd "$ROOT_FOLDER"  # 回到根目录
        fi
    fi
done

# 输出成功编译的文件夹
if [ ${#compiled_folders[@]} -gt 0 ]; then
    echo "Successfully compiled the following folders:"
    for compiled_folder in "${compiled_folders[@]}"; do
        echo "$compiled_folder"
    done
else
    echo "No folders were successfully compiled."
fi
