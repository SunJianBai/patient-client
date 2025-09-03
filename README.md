# 智慧医疗患者端（Qt客户端）

## 项目简介

本项目为智慧医疗管理系统的患者端桌面客户端，基于 Qt/C++ 开发，支持 Linux/Windows。实现了患者注册、登录、预约挂号、病例查看、健康评估、医患聊天等核心功能，支持本地数据缓存与服务器端实时交互。

项目链接：https://github.com/SunJianBai/patient-client#

---

## 主要功能

* **用户注册/登录** ：支持账号注册、登录，登录状态持久化。
* **预约管理** ：挂号、查看、取消预约，实时获取医生排班与号源。
* **病例查看** ：历史就诊信息、医嘱、处方等一览。
* **健康评估** ：填写自测问卷，获取智能分析结果。
* **数据分析** ：展示就诊次数、预约情况等统计信息。
* **医患聊天** ：支持与医生实时聊天，气泡UI自适应，消息本地持久化。
* **本地缓存** ：SQLite保存最近消息和病例，支持离线只读。
* **系统设置** ：服务器IP/端口配置，自动记忆上次连接。

---

## 主要界面

* 登录/注册页
* 主页面（侧边栏导航）
* 首页（数据分析、健康评估结果）
* 预约挂号页
* 病例查看页
* 健康评估问卷页
* 聊天界面
* 个人信息页
* 设置对话框

---

## 主要文件与作用

### 启动与主窗口

* [main.cpp](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：应用入口，初始化数据库和socket，启动主窗口。
* `mainwindow.cpp/h`：登录窗口，负责账号登录、注册入口、记住密码、连接服务器设置。

### 主页面与导航

* `main_page.cpp/h`：主页面，侧边栏导航，动态加载各功能子页面。
* [global.qss](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：全局样式表，美化UI。

### 预约与病例

* `page_appointment.cpp/h/ui`：预约挂号页面，支持科室/医生选择、挂号、取消。
* `page_records.cpp/h/ui`：病例查看页面，展示历史就诊、医嘱、处方等。

### 健康评估

* `page_assessment.cpp/h/ui`：健康评估问卷填写与提交，支持多项健康信息采集。
* `page_dashboard.cpp/h/ui`：首页，展示健康评估结果、数据分析等。

### 聊天功能

* `page_chat.cpp/h/ui`：医患聊天主界面，消息收发、UI布局、历史加载。
* `chatbubblewidget.cpp/h`：自绘聊天气泡控件，支持内容自适应换行、短文本紧凑显示。
* `chatdatabase.cpp/h`：本地聊天消息数据库，SQLite实现，支持消息持久化。
* `chatlistwidget.cpp/h`：聊天消息列表控件（部分历史实现，现主用chatbubblewidget）。

### 用户与设置

* `signupform.cpp/h/ui`：注册窗口，支持账号注册、信息校验。
* `page_profile.cpp/h/ui`：个人信息页，展示和刷新用户基本信息。
* `settingdialog.cpp/h/ui`：服务器连接设置，支持IP/端口配置与记忆。

### 网络通信

* `jsonclient.cpp/h`：TCP客户端通信类，支持带长度前缀的JSON协议（部分实现见文档）。
* [usercontext.h](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：全局用户信息单例，管理当前登录用户上下文。

### 工具与辅助

* [datetime_utils.h](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：时间格式化工具。
* [res.qrc](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：Qt资源文件，管理图片、样式等资源。

### Python测试

* [test.py](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)、[ai_test.py](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：用于接口测试、AI评估等辅助脚本。

---

## 数据结构与核心模型

详见 `文档/核心域与概念模型（ER 视角）.md`，涵盖用户、预约、病例、处方、消息等核心表结构。

---

## 运行说明

1. 安装 Qt5/6、g++、sqlite3。
2. 使用 Qt Creator 打开 [patient-client.pro](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)，编译并运行。
3. 首次运行请在设置中配置服务器IP和端口。
4. 支持本地缓存，断网可只读历史消息和病例。

---

## 目录结构简述

* [main.cpp](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)、`mainwindow.*`：启动与登录
* `main_page.*`：主导航
* `page_*.cpp/h/ui`：各功能页面
* `chatbubblewidget.*`、`chatdatabase.*`：聊天相关
* `settingdialog.*`、`signupform.*`：设置与注册
* [resources](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)、[res.qrc](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：资源文件
* [文档](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：详细设计与接口文档

---

## 参考文档

* [患者端功能.md](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：功能总览
* `文档/核心域与概念模型（ER 视角）.md`：数据结构
* [客户端收发.md](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：通信协议与实现
* [智慧医疗管理系统.md](vscode-file://vscode-app/usr/share/code/resources/app/out/vs/code/electron-browser/workbench/workbench.html)：系统架构与MVP
