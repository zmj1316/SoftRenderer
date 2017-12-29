# 区间扫描线软件渲染器文档


## 项目编译和代码

项目采用 git 作为 scm ， branch 为 other （因为基于之前的项目修改而来） 

项目使用了 C++ 17 部分 feature，建议采用 Visual Studio 2017 编译；

采用 Visual Studio 2015\2013 编译可能会缺失部分功能，不影响课程相关展示效果；

默认编译开启 AVX2 优化，如果旧机器无法运行需要修改编译选项；
默认 windows sdk 版本为 8.1, 如编译遇到相关错误提示需要修改为本机对应的版本；
编译工具集为v140，对应 vs2015，编译报错请修改成本机对应的版本

本项目配置了 visual studio online 在线编译，如有需要, 请将 visual studio online 账号 email to zmj1316@gmail.com 并注明，我会开通该账号的访问权限；



## 操作

* 拖拽窗口边缘调整窗口大小
* WASD 移动摄像机
* 上下左右方向键 移动camera目标
* QE 控制摄像机前后
* Z 切换到 z buffer 模式渲染带透视矫正插值后的结果（光滑的normal效果）

## 优化

已经使用 profiler 对项目进行热点优化，包括精简数据结构，使用 multimap 等容器加速排序；主要耗时在 aet 的排序上

由于对 stl 容器的使用导致大量内存分配占用性能， 所以引入了 tcmalloc 进行优化

## 性能指标

i5-4570 3.6Ghz 处理器， 在视野内绘制 2w+ （77979 个 index， 合计 25993）个三角面片， 开启背面剔除，不计算光照， 稳定 94 帧，对整个面片计算漫反射，稳定 80 帧。



## 项目结构

### 本人写的

* main.cpp:     程序入口
* MYUT.hpp\cpp: win32 窗口辅助功能，包括窗口创建、缩放、键盘等事件处理
* RendererDevice.hpp\cpp:   绘制设备类，提供在屏幕上绘制彩色点的接口
* Renderer.hpp: 固定管线GPU的模拟，这里用到了其中的 Vertex Shader 阶段，其光栅化阶段采用逐三角形 Z-Buffer 绘制
* ScanlineRenderer.hpp: 继承了 Renderer 类，替换了其中的光栅化阶段，用区间扫描线代替，项目主要逻辑在此
* helpers.hpp:  辅助功能代码

### 第三方库

* mmdformat:    用于读取 .pmx 格式的模型
* tcmalloc:  用于优化 stl 库的内存分配效率


## 基本流程

### main.cpp

* 初始化 win32 窗口，设置相关事件的回调函数
* 加载模型
* 开始绘制循环

### 绘制循环

* 处理 IO 事件
* 根据 camera 信息创建 World View Projection 矩阵，传入 Constant Buffer
* 将 vertex buffer, index buffer, constant buffer 传给 renderer 进行绘制
* 将绘制结果画到屏幕上（ Output-Merger  ）

### 绘制管线（ScanlineRenderer）

* Clear:    清理 `edge_table`, `polygon_list` 等容器
* IaStage:  input-assembler, 将 vertex buffer 和 index buffer 组合成三角形
* VSStage： Vertex Shader, 对顶点进行坐标变换，对三角形做剔除，以及归一化除法
* PSStage:  扫描线光栅化和像素渲染


#### VSStage

* 每个顶点乘以 WVP 矩阵到投影空间
* 只做剔除，不做裁剪： z < 0 的顶点在camera后面，会影响归一化，所以用w的值标记为剔除
* 归一化除法到 cvv 

#### PSStage

* buildTable:   将边和多边形加到 `edge_table` 和 `poly_list` 中, 每条边计算出 `edge_entry` 放到其对应的 `y_buttom` 的 slot 中，背面剔除的多边形直接丢弃
* 对屏幕逐行扫描：
* 更新 `aet` 中每条边的 `x_left` 
* 从 `edge_table` 添加新加入的边到 `aet`
* 从 `aet` 删除扫描完成的边
* 对 `aet` 排序
* 遍历 `aet` 中的每条边，着色对应区间（用Normal和LightDir计算简单漫反射），修改 `ipl`
