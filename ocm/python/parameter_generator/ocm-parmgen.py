import yaml
import sys
import os
import re

# 用于存储已生成的类，避免重复生成
generated_classes = {}
# 用于存储每个YAML文件对应的顶级类全限定名
generated_top_classes = []

def capitalize(name):
    """将字段名转换为类名，首字母大写"""
    return ''.join(word.capitalize() for word in name.replace('-', '_').split('_'))

def camel_to_snake(name):
    """将驼峰命名转换为下划线分隔的全小写形式"""
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def determine_type(name, value, namespaces, output_path):
    """确定YAML值对应的C++类型，返回全限定名"""
    if isinstance(value, bool):
        return "bool"
    elif isinstance(value, int):
        return "double"
    elif isinstance(value, float):
        return "double"
    elif isinstance(value, str):
        return "std::string"
    elif isinstance(value, list):
        return get_vector_type(name, value, namespaces, output_path)
    elif isinstance(value, dict):
        class_name = capitalize(name)
        generate_class(class_name, value, namespaces, output_path)
        # 返回包含新增命名空间的全限定类名
        full_class_name = '::'.join(namespaces + [f"auto_{class_name}", class_name])
        return full_class_name
    elif value is None:
        return "std::string"  # 默认处理Null为string
    else:
        return "std::string"  # 默认类型

def get_vector_type(name, lst, namespaces, output_path):
    """确定列表中元素的C++类型"""
    if not lst:
        return "std::vector<std::string>"  # 默认类型

    first_elem = lst[0]
    elem_type = determine_type(name, first_elem, namespaces, output_path)

    # 检查所有元素类型是否一致
    for item in lst:
        current_type = determine_type(name, item, namespaces, output_path)
        if current_type != elem_type:
            return "std::vector<std::string>"  # 默认类型

    return f"std::vector<{elem_type}>"

def sanitize_field_name(name):
    """将字段名转换为有效的C++变量名，转换为小写并添加下划线"""
    # 替换非字母数字字符为下划线，并转换为小写
    sanitized = ''.join(['_' if not c.isalnum() else c for c in name]).lower()
    # 确保变量名不以数字开头
    if sanitized and sanitized[0].isdigit():
        sanitized = '_' + sanitized
    return sanitized + '_'

def create_variable_name(full_class_name):
    """
    根据全限定类名生成成员变量名：
    1. 移除所有 'auto_' 前缀
    2. 移除类名本身
    3. 将剩余部分的驼峰命名转换为下划线分隔的全小写形式
    4. 将 '::' 替换为 '_'
    5. 添加下划线结尾
    """
    parts = full_class_name.split("::")
    # 移除 'auto_' 前缀
    parts = [part[len('auto_'):] if part.startswith('auto_') else part for part in parts]
    # 移除最后一个类名
    if len(parts) > 1:
        parts = parts[:-1]
    else:
        parts = parts[:-1]  # 如果只有一个部分，也移除
    # 将剩余部分的驼峰命名转换为 snake_case
    parts_snake = [camel_to_snake(part) for part in parts]
    # 将剩余部分用 '_' 连接
    var_name = '_'.join(parts_snake).lower()
    # 确保变量名不为空
    if not var_name:
        var_name = 'config'
    return var_name + '_'  # 添加下划线结尾

def generate_class(class_name, data, namespaces, output_path):
    """生成C++类代码并写入文件"""
    # 构建全限定类名以避免命名冲突
    # 在当前命名空间中添加 'auto_ClassName' 命名空间
    auto_namespace = f"auto_{class_name}"
    full_class_name = '::'.join(namespaces + [auto_namespace, class_name])
    if full_class_name in generated_classes:
        return  # 已生成，跳过

    generated_classes[full_class_name] = True

    member_vars = []
    getter_methods = []

    for key, value in data.items():
        field_name = sanitize_field_name(key)
        field_type = determine_type(key, value, namespaces + [auto_namespace], output_path)
        member_vars.append(f"        {field_type} {field_name};")

        # 生成 getter 方法
        getter_name = capitalize(key)
        if field_type in ["bool", "double", "int", "std::string"] or field_type.startswith("std::vector<"):
            # 值类型返回
            getter_methods.append(f"        {field_type} {getter_name}() const {{")
            getter_methods.append(f"            return {field_name};")
            getter_methods.append(f"        }}\n")
        else:
            # 自定义类型返回常量引用
            getter_methods.append(f"        const {field_type}& {getter_name}() const {{")
            getter_methods.append(f"            return {field_name};")
            getter_methods.append(f"        }}\n")

    # 开始生成类代码
    class_code = []
    class_code.append(f"class {class_name} {{")
    class_code.append("public:")

    # 添加默认构造函数和析构函数
    class_code.append(f"    {class_name}() = default;")
    class_code.append(f"    ~{class_name}() = default;\n")

    # 删除拷贝构造函数和赋值操作符
    class_code.append(f"    {class_name}(const {class_name}&) = default;")
    class_code.append(f"    {class_name}& operator=(const {class_name}&) = default;\n")

    # 删除移动构造函数和赋值操作符
    class_code.append(f"    {class_name}({class_name}&&) = default;")
    class_code.append(f"    {class_name}& operator=({class_name}&&) = default;\n")

    # 添加 update_from_yaml 方法
    class_code.append("    void update_from_yaml(const YAML::Node& auto_yaml_node,bool check_key) {")
    for key, value in data.items():
        field_name = sanitize_field_name(key)
        field_type = determine_type(key, value, namespaces + [auto_namespace], output_path)

        if field_type.startswith("std::vector<"):
            elem_type = field_type[len("std::vector<"):-1]
            class_code.append(f"        if(auto_yaml_node[\"{key}\"]) {{")
            class_code.append(f"            {field_name}.clear();")
            class_code.append(f"            for(auto& item : auto_yaml_node[\"{key}\"]) {{")
            if elem_type in ["std::string", "bool", "int", "double"]:
                class_code.append(f"                {field_name}.push_back(item.as<{elem_type}>());")
            else:
                # elem_type 是一个自定义类，使用全限定名
                class_code.append(f"                {elem_type} elem;")
                class_code.append(f"                elem.update_from_yaml(item,check_key);")
                class_code.append(f"                {field_name}.push_back(elem);")
            class_code.append(f"            }}")
            class_code.append(f"        }}else {{if (check_key) throw std::runtime_error(\"[ConfigCollect] Do not find key: {key} \");}}")
        elif field_type in ["bool", "int", "double", "std::string"]:
            class_code.append(f"        if(auto_yaml_node[\"{key}\"]) {field_name} = auto_yaml_node[\"{key}\"].as<{field_type}>(); else if (check_key) throw std::runtime_error(\"[ConfigCollect] Do not find key: {key}\" );")
        else:
            # field_type 是一个自定义类，使用全限定名
            class_code.append(f"        if(auto_yaml_node[\"{key}\"]) {field_name}.update_from_yaml(auto_yaml_node[\"{key}\"],check_key);")

    class_code.append("    }\n")

    # 添加 getter methods
    for getter in getter_methods:
        class_code.append(getter)

    # 添加 print 方法，增加 indent_level 参数
    class_code.append("    void print(int indent_level = 0) const {")
    class_code.append("        std::string indent(indent_level * 4, ' ');")
    class_code.append(f"        std::cout << indent << \"{class_name}:\" << std::endl;")
    for key, value in data.items():
        field_name = sanitize_field_name(key)
        field_type = determine_type(key, value, namespaces + [auto_namespace], output_path)
        if field_type in ["bool", "int", "double", "std::string"]:
            class_code.append(f"        std::cout << indent << \"    {field_name}: \" << {field_name} << std::endl;")
        elif field_type.startswith("std::vector<"):
            class_code.append(f"        std::cout << indent << \"    {field_name}: [\" << std::endl;")
            class_code.append(f"        for(const auto& item : {field_name}) {{")
            elem_type = field_type[len("std::vector<"):-1]
            if elem_type in ["bool", "int", "double", "std::string"]:
                class_code.append(f"            std::cout << indent << \"        \" << item << std::endl;")
            else:
                class_code.append(f"            item.print(indent_level + 2);")
            class_code.append(f"        }}")
            class_code.append(f"        std::cout << indent << \"    ]\" << std::endl;")
        else:
            # 自定义类型，调用其 print 方法
            class_code.append(f"        std::cout << indent << \"    {field_name}:\" << std::endl;")
            class_code.append(f"        {field_name}.print(indent_level + 1);")
    class_code.append("    }\n")

    # 添加 private 访问权限
    class_code.append("private:")
    class_code.extend(member_vars)
    class_code.append("};")
    class_code.append("")  # 空行

    # 先生成子类
    for key, value in data.items():
        if isinstance(value, dict):
            sub_class_name = capitalize(key)
            # 添加 'auto_' 前缀并基于键名构建命名空间
            new_namespaces = namespaces + [auto_namespace]
            generate_class(sub_class_name, value, new_namespaces, output_path)
        elif isinstance(value, list) and len(value) > 0 and isinstance(value[0], dict):
            # 假设列表名为复数形式，类名为单数
            sub_class_name = capitalize(key.rstrip('s'))
            # 添加 'auto_' 前缀并基于键名构建命名空间
            new_namespaces = namespaces + [auto_namespace]
            generate_class(sub_class_name, value[0], new_namespaces, output_path)

    # 将生成的类写入文件，带有命名空间
    with open(os.path.join(output_path, "parameter.hpp"), "a") as f:
        # 写入命名空间开始
        for ns in namespaces:
            f.write(f"namespace {ns} {{\n")
        # 写入 'auto_ClassName' 命名空间开始
        f.write(f"namespace {auto_namespace} {{\n")
        # 写入类代码
        for line in class_code:
            f.write(line + "\n")
        # 写入 'auto_ClassName' 命名空间结束
        f.write(f"}} // namespace {auto_namespace}\n")
        # 写入其他命名空间结束
        for ns in reversed(namespaces):
            f.write(f"}} // namespace {ns}\n")
        f.write("\n")  # 空行

def print_generated_classes():
    """打印每个YAML文件对应的同名顶级类，包含所有命名空间和类名，使用 :: 分隔"""
    print("生成的C++顶级类及其命名空间结构:")
    for class_name in generated_top_classes:
        print(class_name)

def generate_config_collect_class(output_path):
    """在 parameter.hpp 中生成线程安全的懒汉式单例 ConfigCollect 类，包含所有顶级类的私有成员变量、公共 getter 方法以及 update_from_yaml 方法。
    为每个成员变量增加一个单独的 update_from_yaml 函数接口，并将其放入 private。
    增加一个公共函数，可以使用字符串输入（通过 if-else 语句）调用独立的 update_from_yaml。
    调用时的字符串为成员变量名，驼峰命名转换为下划线连接的全小写形式，确保不会出现连续的下划线。
    增加一个 update_from_yaml_all 函数，调用所有成员变量的更新函数。
    """
    if not generated_top_classes:
        print("没有生成任何顶级类，跳过 ConfigCollect 类的生成。")
        return

    with open(os.path.join(output_path, "parameter.hpp"), "a") as f:
        # 开始生成 ConfigCollect 类
        f.write("class ConfigCollect {\npublic:\n")
        
        # 添加获取单例实例的静态方法
        f.write("    // 获取单例实例\n")
        f.write("    static ConfigCollect& getInstance() {\n")
        f.write("        static ConfigCollect instance;\n")
        f.write("        return instance;\n")
        f.write("    }\n\n")
        
        # 删除拷贝构造函数和赋值操作符
        f.write("    // 删除拷贝构造函数和赋值操作符\n")
        f.write("    ConfigCollect(const ConfigCollect&) = delete;\n")
        f.write("    ConfigCollect& operator=(const ConfigCollect&) = delete;\n\n")
        
        # 删除移动构造函数和赋值操作符
        f.write("    // 删除移动构造函数和赋值操作符\n")
        f.write("    ConfigCollect(ConfigCollect&&) = delete;\n")
        f.write("    ConfigCollect& operator=(ConfigCollect&&) = delete;\n\n")
        
        # 添加公共的 update_from_yaml 函数，接收字符串参数
        f.write("    // 根据名称更新对应的配置\n")
        f.write("    void update_from_yaml(const std::string& name, const std::string& base_path,bool check_file,bool check_key) {\n")
        f.write("        if (name.empty()) {\n")
        f.write("            return;\n")
        f.write("        }\n")
        # 初始化是否匹配的标志
        f.write("        bool matched = false;\n")
        # 开始生成 if-else 语句，根据输入的字符串调用对应的私有函数
        for idx, full_class_name in enumerate(generated_top_classes):
            # 处理命名空间和变量名
            parts = full_class_name.split("::")
            # 提取顶层命名空间部分以生成变量名
            # 移除 'auto_' 前缀
            parts_no_auto = [part[len("auto_"):] if part.startswith("auto_") else part for part in parts]
            # 移除类名本身
            parts_no_auto = parts_no_auto[:-1]
            # 将剩余部分转换为 snake_case
            var_name_snake = '_'.join([camel_to_snake(part) for part in parts_no_auto])
            var_name_snake = re.sub('_+', '_', var_name_snake)
            # 生成 if-else 语句
            if idx == 0:
                f.write(f"        if (name == \"{var_name_snake}\") {{\n")
            else:
                f.write(f"        else if (name == \"{var_name_snake}\") {{\n")
            f.write(f"            update_from_yaml_{var_name_snake}(base_path,check_file,check_key);\n")
            f.write("            matched = true;\n")
            f.write("        }\n")
        # 添加未匹配时的处理
        f.write("        if (!matched) {\n")
        f.write("            // 未找到匹配的配置项\n")
        f.write("            std::cerr << \"[ConfigCollect] No matching configuration for '\" << name << \"'\" << std::endl;\n")
        f.write("        }\n")
        f.write("    }\n\n")

        # 添加 update_from_yaml_all 函数，调用所有成员变量的更新函数
        f.write("    // 更新所有配置\n")
        f.write("    void update_from_yaml_all(const std::string& base_path,bool check_file,bool check_key) {\n")
        for full_class_name in generated_top_classes:
            # 处理命名空间和变量名
            parts = full_class_name.split("::")
            # 提取顶层命名空间部分以生成函数名
            parts_no_auto = [part[len("auto_"):] if part.startswith("auto_") else part for part in parts]
            parts_no_auto = parts_no_auto[:-1]
            var_name_snake = '_'.join([camel_to_snake(part) for part in parts_no_auto])
            var_name_snake = re.sub('_+', '_', var_name_snake)
            # 调用更新函数
            f.write(f"        update_from_yaml_{var_name_snake}(base_path,check_file,check_key);\n")
        f.write("    }\n\n")

        # 添加 print 方法，增加 indent_level 参数
        f.write("    // 打印所有配置\n")
        f.write("    void print(int indent_level = 0) const {\n")
        f.write("        std::string indent(indent_level * 4, ' ');\n")
        for full_class_name in generated_top_classes:
            # 生成成员变量名
            parts = full_class_name.split("::")
            parts_no_auto = [part[len("auto_"):] if part.startswith("auto_") else part for part in parts]
            parts_no_auto = parts_no_auto[:-1]
            var_name_snake = '_'.join([camel_to_snake(part) for part in parts_no_auto])
            var_name_snake = re.sub('_+', '_', var_name_snake)
            f.write(f"        std::cout << indent << \"{var_name_snake}:\" << std::endl;\n")
            f.write(f"        {{\n")
            f.write(f"            std::shared_lock<std::shared_mutex> lock(m_{var_name_snake});\n")
            f.write(f"            {var_name_snake}.print(indent_level + 1);\n")
            f.write(f"        }}\n\n")
        f.write("    }\n\n")

        # 添加 getter 方法
        f.write("    // 获取各个配置的实例\n")
        for full_class_name in generated_top_classes:
            # 处理命名空间和变量名
            parts = full_class_name.split("::")
            parts_no_auto = [part[len("auto_"):] if part.startswith("auto_") else part for part in parts]
            parts_no_auto = parts_no_auto[:-1]
            var_name_snake = '_'.join([camel_to_snake(part) for part in parts_no_auto])
            var_name_snake = re.sub('_+', '_', var_name_snake)

            # 生成 getter 方法名
            getter_method_name = 'get_' + var_name_snake
            # 生成 getter 方法
            f.write(f"    {full_class_name}& {getter_method_name}() {{\n")
            f.write(f"        std::shared_lock<std::shared_mutex> lock(m_{var_name_snake});\n")
            f.write(f"        return {var_name_snake};\n")
            f.write("    }\n\n")

        # 添加私有成员变量和函数
        f.write("private:\n")
        # 添加私有构造函数
        f.write("    // 私有构造函数，防止外部实例化\n")
        f.write("    ConfigCollect() = default;\n\n")
        for full_class_name in generated_top_classes:
            # 生成成员变量名和对应的函数名
            parts = full_class_name.split("::")
            parts_no_auto = [part[len("auto_"):] if part.startswith("auto_") else part for part in parts]
            parts_no_auto = parts_no_auto[:-1]
            class_name = parts[-1]
            var_name_snake = '_'.join([camel_to_snake(part) for part in parts_no_auto])
            var_name_snake = re.sub('_+', '_', var_name_snake)

            # 生成函数名
            function_name_snake = var_name_snake

            # 添加私有成员变量
            f.write(f"    {full_class_name} {var_name_snake};\n")
            f.write(f"    mutable std::shared_mutex m_{var_name_snake};\n\n")

            # 构建文件路径
            dir_parts = parts_no_auto  # 目录路径去掉最后的类名
            yaml_class_name = class_name  # 最后的类名为 YAML 文件名

            # 将命名空间部分转换为 snake_case
            dir_parts_snake = [camel_to_snake(part) for part in dir_parts]

            # YAML 文件名为最后一个类名转换为 snake_case
            yaml_file_name = camel_to_snake(yaml_class_name)
            # 去除连续的下划线
            yaml_file_name = re.sub('_+', '_', yaml_file_name)

            # 构建目录路径，确保移除了最后一层目录
            if dir_parts_snake:
                dir_path = "/".join(dir_parts_snake)
                yaml_path = f"base_path + \"/{dir_path}.yaml\""
            else:
                yaml_path = f"base_path + \".yaml\""

            # 添加私有的 update_from_yaml 函数
            f.write(f"    // 更新 {full_class_name} 配置\n")
            f.write(f"    void update_from_yaml_{function_name_snake}(const std::string& base_path, bool check_file, bool check_key) {{\n")
            f.write("        YAML::Node auto_yaml_node;\n")
            f.write(f"        // 加载 YAML 文件\n")
            f.write(f"        std::ifstream file({yaml_path});\n")
            f.write(f"        if (file.good()) {{ \n ")
            f.write(f"            auto_yaml_node = YAML::LoadFile({yaml_path});\n")
            f.write("            {\n")
            f.write(f"                std::unique_lock<std::shared_mutex> lock(m_{var_name_snake});\n")
            f.write(f"                {var_name_snake}.update_from_yaml(auto_yaml_node,check_key);\n")
            f.write("            }\n")
            f.write("        } else {\n")
            f.write(f"            if (check_file) {{\n")
            f.write(f"                throw std::runtime_error(\"[ConfigCollect] Do not find YAML file: \" + {yaml_path});\n")
            f.write("            } else {\n")
            f.write(f"                std::cerr << \"[ConfigCollect] Do not find YAML file: \" << {yaml_path} << std::endl;\n")
            f.write("            }\n")
            f.write("        }\n")
            f.write("    }\n\n")
        f.write("};\n\n")
    print("已生成线程安全的懒汉式单例 ConfigCollect 类。")

def main(input_path, output_path):
    # 清空输出文件并写入头文件内容
    with open(os.path.join(output_path, "parameter.hpp"), "w") as f:
        f.write("/*\n")
        f.write(" * Automatically generate files, manual modification is strictly prohibited!\n")
        f.write(" */\n")
        f.write("#pragma once\n#include <fstream>\n#include <string>\n#include <vector>\n#include <iostream>\n#include <yaml-cpp/yaml.h>\n#include <shared_mutex>\n#include <mutex>\n\nnamespace ocm {\n")

    # 遍历输入路径
    for root, dirs, files in os.walk(input_path):
        for file in files:
            if file.endswith(('.yaml', '.yml')):
                yaml_file = os.path.join(root, file)
                # 计算相对于输入路径的相对路径
                rel_path = os.path.relpath(root, input_path)
                # 生成命名空间列表
                if rel_path == '.' or rel_path == '':
                    namespaces = []
                else:
                    # 将相对路径分割为各个部分，并转换为命名空间，同时添加 'auto_' 前缀
                    namespaces = ["auto_" + capitalize(part) for part in rel_path.replace('-', '_').split(os.sep)]
                # 读取YAML文件
                try:
                    with open(yaml_file, 'r') as f_yaml:
                        data = yaml.safe_load(f_yaml)
                        if data is None:
                            print(f"YAML 文件 {yaml_file} 是空的，跳过。")
                            continue
                except yaml.YAMLError as e:
                    print(f"解析 YAML 文件 {yaml_file} 时出错: {e}")
                    continue
                except Exception as e:
                    print(f"读取文件 {yaml_file} 时出错: {e}")
                    continue

                # # 检查 YAML 数据是否有单一顶层键
                # if isinstance(data, dict) and len(data) == 1:
                #     top_key = next(iter(data))
                #     class_name = capitalize(top_key)
                #     # 为顶层类不预先添加命名空间
                #     all_namespaces = namespaces
                #     class_data = data[top_key]
                # else:
                #     # 如果有多个顶层键，使用文件名作为类名
                base_name = os.path.splitext(file)[0]
                file_namespace = "auto_" + capitalize(base_name)+'_main'
                all_namespaces = namespaces
                class_name = capitalize(base_name)
                class_data = data

                # 生成类
                generate_class(class_name, class_data, all_namespaces, output_path)
                # 构建全限定类名，包括类名本身
                full_class_name = '::'.join(all_namespaces + [f"auto_{class_name}", class_name])
                generated_top_classes.append(full_class_name)

    # 打印生成的类
    print_generated_classes()
    # 生成 ConfigCollect 类
    generate_config_collect_class(output_path)
    with open(os.path.join(output_path, "parameter.hpp"), "a") as f:
        f.write("}\n")
    print("C++ 类已生成到 'parameter.hpp' 文件中。")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("用法: python generate_cpp_classes.py <input_directory> <output_directory>")
        sys.exit(1)
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    if not os.path.isdir(input_path):
        print(f"输入路径 '{input_path}' 不是一个有效的目录。")
        sys.exit(1)
    if not os.path.isdir(output_path):
        print(f"输出路径 '{output_path}' 不是一个有效的目录。")
        sys.exit(1)
    main(input_path, output_path)
