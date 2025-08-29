协议基本规范

* **传输层** ：TCP
* **编码** ：UTF-8
* **封包** ：`[4字节长度前缀] + JSON内容`
* **JSON 格式通用字段** ：

```json
  {
    "type": "xxx.yyy",   // 功能类型
    "seq": 1001,         // 请求序号，请求和响应对应
    "token": "xxxx",     // 登录成功后服务器下发，后续请求必须带
    "payload": {...}     // 具体数据
  }
```

* **响应格式** ：

```json
  {
    "ok": true,          // 是否成功
    "seq": 1001,         // 对应请求序号
    "payload": {...},    // 成功时返回的数据
    "error": "msg"       // 失败时的错误信息
  }
```

---

## 患者端组功能

**主要功能**

- 用户注册/登录（密码加密传输，登录态保存）
- 预约管理：挂号、查看、取消
- 病例查看：历史就诊信息、医嘱
- 健康评估：填写自测问卷并提交
- 基础数据分析：查看自己的就诊次数、预约情况的图表
- 聊天 UI：单聊（收/发消息，离线消息处理）

## 患者端请求/响应

### 1. 用户注册

**请求字段**

* `username`：用户名（唯一标识患者）
* `password`：用户密码（明文传输时必须加密，或者至少走 TLS）
* `phone`：手机号（可选字段，用于找回密码）
* 

{
  "type": "register",
  "seq": 1002,
  "payload": {
    "user": "alice",
    "passwd": "123456",
    "name":"用户真实姓名",
    "gender":"男"or"女"，
    "phone": "13800138000"，
    "id_number": "身份证"，
    "adress":"北京市"        # 家庭住址

  }
}

**响应字段**

* `user_id`：新注册用户的 ID

```json
{"ok": true, "seq": 1, "payload": {"user_id": 101}}
```

### 2. 登录

* **请求**

  ```json
  {
      "type": "login",   // 功能类型
      "seq": 1001,         // 请求序号，请求和响应对应
      "user":"AAA",//用户名
      "pswd":"1234"//密码
    }
  ```
* **响应**

  * `user_id`：用户 ID
  * `role`：角色（这里应该是 `"patient"`，医生端登录会返回 `"doctor"`）
* * `token`：身份凭证（后续请求必须携带）

  ```json
  {
  "seq":1001,
  "ok":true//成功
  }
  ```

---

### 3. 预约管理

#### 创建预约 `appt.create`

* **请求字段**
  * `doctor_id`：要预约的医生 ID
  * `start_time`：预约时间（ISO 8601 格式，如 `"2025-08-25T10:00:00"`）
* **响应字段**
  * `appt_id`：生成的预约 ID

#### 查看预约 `appt.list`

* **请求字段**
  * 空对象即可（token 即表示用户身份）
* **响应字段**
  * `appointments`：数组，包含每个预约对象
    * `appt_id`：预约 ID
    * `doctor_id`：对应医生 ID
    * `time`：预约时间
    * `status`：状态（`pending/confirmed/cancelled` 等）

#### 取消预约 `appt.cancel`

* **请求字段**

  * `appt_id`：要取消的预约 ID
* **响应字段**

  * 无额外字段，只返回 ok=true/false
* **挂号请求**

  ```json
  {
    "type": "appt.create",
    "seq": 3,
    "token": "abcd1234",
    "payload": {
      "doctor_id": 2001,
      "start_time": "2025-08-25T10:00:00"
    }
  }
  ```
* **响应**

  ```json
  {"ok": true, "seq": 3, "payload": {"appt_id": 888}}
  ```
* **查看预约**

  ```json
  {
    "type": "appt.list",
    "seq": 4,
    "token": "abcd1234",
    "payload": {}
  }
  ```

  响应：

  ```json
  {
    "ok": true,
    "seq": 4,
    "payload": {
      "appointments": [
        {"appt_id": 888, "doctor_id": 2001, "time": "2025-08-25T10:00:00", "status": "confirmed"}
      ]
    }
  }
  ```
* **取消预约**

  ```json
  {
    "type": "appt.cancel",
    "seq": 5,
    "token": "abcd1234",
    "payload": {"appt_id": 888}
  }
  ```

  响应：

  ```json
  {"ok": true, "seq": 5}
  ```

---

### 4. 病例查看

* **请求字段**

  * 空对象即可
* **响应字段**

  * `records`：数组
    * `record_id`：病例 ID
    * `doctor_id`：医生 ID
    * `diagnosis`：诊断结果
    * `prescription`：处方/医嘱内容
* **请求**

  ```json
  {
    "type": "record.list",
    "seq": 6,
    "token": "abcd1234",
    "payload": {}
  }
  ```
* **响应**

  ```json
  {
    "ok": true,
    "seq": 6,
    "payload": {
      "records": [
        {"record_id": 3001, "doctor_id": 2001, "diagnosis": "flu", "prescription": "tamiflu"}
      ]
    }
  }
  ```

---

### 5. 健康评估

* **请求字段**

  * `answers`：数组，表示患者填写的问卷答案（可以用整数或字符串）
* **响应字段**

  * `score`：健康评分（数值型，比如 85）
  * `risk`：风险等级（如 `"low"`, `"medium"`, `"high"`）
  * `ans(可选)：`string ` 可以集成ai给出建议`
* **请求**

  ```json
  {
    "type": "health.submit",
    "seq": 7,
    "token": "abcd1234",
    "payload": {
      "answers": [1, 0, 3, 2]
    }
  }
  ```
* **响应**

  ```json

  {
  	"ok": true, 
  	"seq": 7, 
  	"payload": {"score": 85, "risk": "low"},
  	"ans":"AI给出的建议"
  }
  ```

---

### 6. 数据分析（患者端查看）

* **请求字段**

  * 空对象即可
* **响应字段**

  * `visits`：就诊总次数
  * `appointments`：预约统计对象
    * `pending`：待就诊数量
    * `done`：已完成数量
* **请求**

  ```json
  {
    "type": "stats.patient",
    "seq": 8,
    "token": "abcd1234",
    "payload": {}
  }
  ```
* **响应**

  ```json
  {
    "ok": true,
    "seq": 8,
    "payload": {
      "visits": 5,
      "appointments": {"pending": 2, "done": 3}
    }
  }
  ```

---
