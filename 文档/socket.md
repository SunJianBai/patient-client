根据你的 mainwindow.cpp 代码，结合 Qt 网络模块，客户端与服务器的连接、消息发送与接收的完整流程如下：

---

## 1. 连接服务器

- **Socket 创建**在 main.cpp 或 `MainWindow` 构造时，创建 `QTcpSocket` 对象（如 `m_socket`）。
- **设置服务器地址和端口**用户通过设置窗口（`SettingDialog`）输入服务器 IP 和端口，调用 `m_socket->connectToHost(ip, port)` 发起连接。
- **连接状态检查**
  通过 `m_socket->state()` 判断是否已连接，未连接时会弹窗提示。

---

## 2. 发送消息（以登录为例）

- **组装 JSON 数据**在 `validateLogin` 函数中，构造登录请求 JSON：
  ```cpp
  QJsonObject obj;
  obj["type"] = "login";
  obj["seq"] = 1001;
  obj["user"] = username;
  obj["pswd"] = password;
  obj["role"] = "patient";
  QJsonDocument doc(obj);
  QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
  ```
- **加长度前缀**服务器要求每个数据包前有4字节长度前缀，代码如下：
  ```cpp
  QByteArray packet;
  QDataStream stream(&packet, QIODevice::WriteOnly);
  stream.setVersion(QDataStream::Qt_5_10);
  stream << static_cast<quint32>(jsonData.size());
  packet.append(jsonData);
  ```
- **发送数据**
  通过 socket 发送数据包：
  ```cpp
  m_socket->write(packet);
  m_socket->flush();
  ```

---

## 3. 接收消息

- **阻塞等待服务器响应**登录时采用阻塞方式等待服务器返回：
  ```cpp
  if (!m_socket->waitForReadyRead(3000)) // 最多等待3秒
      return false;
  QByteArray resp = m_socket->readAll();
  ```
- **解析长度前缀和 JSON**先读4字节长度，再读 JSON 内容：
  ```cpp
  QDataStream respStream(resp);
  respStream.setVersion(QDataStream::Qt_5_10);
  quint32 len = 0;
  respStream >> len;
  QByteArray jsonResp = resp.right(resp.size() - 4);
  QJsonDocument respDoc = QJsonDocument::fromJson(jsonResp);
  ```
- **处理服务器返回**
  判断 type/seq/ok 字段，决定是否登录成功，并可提取 user_id 等信息。

---

## 4. 典型流程时序图（文本版）

```plaintext
用户操作
    │
    ▼
MainWindow::on_loginBtn_clicked()
    │
    ├─> 检查 socket 连接状态
    │
    ├─> validateLogin(username, password)
    │      │
    │      ├─> 组装 JSON，带长度前缀
    │      ├─> m_socket->write(packet)
    │      ├─> m_socket->waitForReadyRead()
    │      ├─> m_socket->readAll()
    │      └─> 解析 JSON，判断登录结果
    │
    └─> 登录成功则 mainPage->show()
```

---

## 5. 其他消息收发

- 其他业务（如聊天、预约等）也是组装 JSON → 加长度前缀 → write → flush。
- 接收消息时，通常在主页面用 `connect(socket, &QTcpSocket::readyRead, ...)` 监听，解析数据包并分发到各业务模块。

---

### 总结

- **连接**：通过 QTcpSocket 的 connectToHost 实现，支持断线重连。
- **发送**：所有消息都需加4字节长度前缀，JSON 格式。
- **接收**：先读长度，再读 JSON，按 type/seq 分发处理。
- **阻塞/异步**：登录用阻塞等待，主页面用异步 readyRead 信号。

如需具体某一业务（如聊天、健康评估等）的消息收发流程，请继续告知！
