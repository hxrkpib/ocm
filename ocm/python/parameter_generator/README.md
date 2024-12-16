# 使用
```bash
pyinstaller --onefile ocm-parmgen.py
安装后：ocm-parmgen <input_path> <output_path>
```
# 注意
- 输入输出均为文件夹，会递归处理所有yaml文件，生成到一个hpp文件中
- 如果一个yaml里只有一个key, key和文件名必须相同
- ymal需符合规范，禁止无key的list！！！
错误：
```yaml
- 1
- 2
```
正确：
```yaml
a:
  - 1
  - 2
```
