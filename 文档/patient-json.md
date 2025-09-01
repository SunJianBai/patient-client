协议基本规范

* **传输层** ：TCP
* **编码** ：UTF-8
* **封包** ：`[4字节长度前缀] + JSON内容`
* **JSON 格式通用字段** ：

```
  {
    "type": "xxx.yyy",   // 功能类型
    "seq": 1001,         // 请求序号，请求和响应对应
    "token": "xxxx",     // 登录成功后服务器下发，后续请求必须带
    "payload": {...}     // 具体数据
  }
```

* **响应格式** ：

```
  {
    "ok": true,          // 是否成功
    "seq": 1001,         // 对应请求序号
    "payload": {...},    // 成功时返回的数据
    "error": "msg"       // 失败时的错误信息
  }
```

---

## 患者端组功能

**主要功能**

* **用户注册/登录（密码加密传输，登录态保存）**
* **预约管理：挂号、查看、取消**
* **病例查看：历史就诊信息、医嘱**
* **健康评估：填写自测问卷并提交**
* **基础数据分析：查看自己的就诊次数、预约情况的图表**
* **聊天 UI：单聊（收/发消息，离线消息处理）**

## 患者端请求/响应

### 1. 用户注册

**请求字段**

* `username`：用户名（唯一标识患者）
* `password`：用户密码（明文传输时必须加密，或者至少走 TLS）
* `phone`：手机号（可选字段，用于找回密码）
* 

```
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
    "adress":"北京市"        # 家庭住址

  }
}
```

**响应字段**

* `user_id`：新注册用户的 ID

```
{
 "ok": true, 
 "seq": 1002, 
 "payload": {
     "user_id": 101
 }
}
```

### 2. 登录

* **请求**

  ```
  {
      "type": "login",   // 功能类型
      "seq": 1001,         // 请求序号，请求和响应对应
      "user":"AAA",//用户名
      "type": "patient" or "doctor",
      "pswd":"1234"//密码
    }
  ```
* **响应**

  * `user_id`：用户 ID
  * `role`：角色（这里应该是 `"patient"`，医生端登录会返回 `"doctor"`）
* * `token`：身份凭证（后续请求必须携带）

  ```
  {
  "seq":1001,
  "user_id": "1001",
  "ok":true//成功
  }
  ```

---

### 3. 预约管理

#### 获取科室列表

* **请求**
  **返回所有科室的名称列表**

```
{
  "type": "department_list",
  "seq": 1097,
}
```

* **响应**

```
{
  "type": "department_list",
  "seq": 1097,
   "payload": {
    "appointments": [
      {"department_name": "呼吸科"},
        {"department_name": "消化科"}
    ]
  }
}
```

#### 获取医生列表 doctor_list

* **请求**
* **根据科室获取当前科室的所有医生信息**

```
{
  "type": "doctor_list",
  "seq": 1098,
   "department_name":"呼吸科"
}
```

* **响应**

```
{
  "type": "doctor_list",
  "seq": 1098,
  "payload": {
    "appointments": [
      {"doctor_id": 2001,
       "full_name":"XXX"               #医生名字
      "bio":"XXXXXXXXXX",          #医生简介
       "duty_start":"XXXXXXXX"     #医生出诊时间
       "reg_fee" : 50,            #挂号费用
       "daily_quota" : 50          #每天挂号人数
      }
    ]
  }

}
```

#### 创建预约 `appt.create`

* **请求字段**

  * `doctor_id`：要预约的医生 ID
  * `start_time`：预约时间（ISO 8601 格式，如 `"2025-08-25T10:00:00"`）
* **挂号请求**

  ```
  {
    "type": "appt.create",
    "seq": 1003,
    "user_id": 1001,
    "payload": {
      "doctor_id": 2001,
      "start_time": "2025-08-25T10:00:00"
        "age":20,   #年龄
        "height":180,  #身高
        "weight":60,   #体重（kg）
  		"sympptoms":"感冒发烧"  #症状
    }
  }
  ```
* **响应字段**

  * `appt_id`：生成的预约 ID

  ```
  {"ok": true, 
   "seq": 3, 
   "payload": {"appt_id": 888}}
  ```

#### 查看预约 `appt.list`

* **请求字段**
  * **空对象即可（token 即表示用户身份）**

```
{
  "type": "appt.list",
  "seq": 1004,
  "user_id": "1234",   #user_id表明用户身份
  "payload": {}
}
```

**响应字段**

* `appointments`：数组，包含每个预约对象（查询该用户的全部预约记录）
  * `appt_id`：预约 ID
  * `doctor_name`：对应医生名字
  * `department_name` :科室名字
  * `time`：预约时间
  * `status`：状态（`pending/confirmed/cancelled` 等）

```
{
  "ok": true,
  "seq": 1004,
  "payload": {
    "num_pending":5,
    "num_confirmed":6,
     "num_cancelled":4,
    "appointments": [
      {"appt_id": 888, 
       "doctor_name": "张医生",
       "department_name":"呼吸科",
       "time": "2025-08-25 10:00", 
       "status": "confirmed"
      }
    ]
  }
}
```

#### 取消预约 `appt.cancel`

* **请求字段**

  * `appt_id`：要取消的预约 ID
* **响应字段**

  * **无额外字段，只返回 ok=true/false**
* **取消预约**

  ```
  {
    "type": "appt.cancel",
    "seq": 1005,
    "payload": {"appt_id": 888}
  }
  ```

  **响应：**

  ```
  {"ok": true, "seq": 5}
  ```

---

### 4. 病例查看

**（病例由医生处理过的预约生成，所以每个病例有对应的appt_id）**

* **请求字段**
  * **空对象即可**
* **响应字段**
  **就诊时间**
  * `appt_id`：病例 ID
  * `doctor_id`：医生 ID
  * **医生名字**
  * **医生科室名字**
  * **患者症状**
  * `prescription`：处方/医嘱内容
* **请求**
  ```
  {
    "type": "record.list",
    "seq": 1006,
    "user_id": "1234",   #user_id表明用户身份
     "appt_id":"101"   #预约id
  }
  ```
* **响应**
  ```
  {
    "ok": true,
    "seq": 1006,
    "payload": {
        "appt_id": 3001, 
        "doctor_id": 2001, 
        "doctor_name": "张医生",
        "department_name":"呼吸科",
        "sympptoms":"感冒发烧",
        "prescription": "多喝热水"
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
  * `ans(可选)：`string `可以集成ai给出建议`
* **请求**
  ```
  {
    "type": "health.submit",
    "seq": 1048,
    "user_id": "1234",
    "payload": {
        "time":"2025-08-25 10:00", 
       "risk_level": "高",
    	"advice": [
          "建议规律作息",
          "增加适度运动",
          "定期监测血糖"
    ]
    }
  }
  ```
* **响应**
  <pre class="md-fences mock-cm md-end-block" spellcheck="false" lang="json" cid="n212" mdtype="fences"><br/>{<br/>	"ok": true, <br/>	"seq": 1048<br/>}</pre>

#### 获取健康评估

* * `ans(可选)：`string `可以集成ai给出建议`
* **请求**
  ```
  {
    "type": "health.get",
    "seq": 1047,
    "user_id": "1234"
  }
  ```
* **响应**
  **type**
  **seq**
  **时间**
  **风险**
  **建议（3条）**
  ```
  {
      "type": "health.get",
  	"seq": 1047
    	"payload": {
      	"time":"2025-08-25 10:00", 
       "risk_level": "高",
    	"advice": [
          "建议规律作息",
          "增加适度运动",
          "定期监测血糖"
    ]
    }
  }
  ```

---

### 6. 首页-数据分析（患者端查看）

* **请求字段**
  * **空对象即可**
* **响应字段**
  * `visits`：就诊总次数
  * `appointments`：预约统计对象
    * `pending`：待就诊数量
    * `done`：已完成数量
* **请求**
  ```
  {
    "type": "stats.patient",
    "seq": 8,
    "token": "abcd1234",
    "payload": {}
  }
  ```
* **响应**
  ```
  {
    "ok": true,
    "seq": 8,
    "payload": {
      "visits": 5,
      "appointments": {"pending": 2, "done": 3}
    }
  }
  ```

### 7. 聊天

#### 发送消息 `chat.send`

* **请求字段**
  * `to`：对方 ID（这里是医生 ID）
  * `content`：消息内容（纯文本，后期可以扩展图片/语音）
* **响应字段**
  * `msg_id`：消息 ID（唯一标识，用于去重、存储）

#### 接收消息 `chat.recv`（服务器推送）

* **字段**
  * `from`：发送方 ID（医生 ID）
  * `content`：消息内容
  * `time`：发送时间（ISO 格式）
* **发送消息**
  ```
  {
    "type": "chat.send",
    "seq": 9,
    "token": "abcd1234",
    "payload": {
      "to": 2001,
      "content": "医生你好，我有点发烧"
    }
  }
  ```
* **响应**
  ```
  {"ok": true, "seq": 9, "payload": {"msg_id": 501}}
  ```
* **接收消息** （服务器主动推送）

```
  {
    "type": "chat.recv",
    "payload": {
      "from": 2001,
      "content": "注意多休息，多喝水",
      "time": "2025-08-25T11:00:00"
    }
  }
```

---

### 8.个人信息

#### 查看个人信息

```
{
  "type": "userinfo",
  "seq": 1099,
  "user_id": 1234
}
```

```
{ 
  "type": "userinfo",
  "seq": 1099,
  "payload": {
    "user": "alice",
    "role":'patient',
    "name":"用户真实姓名",
    "gender":"男",
    "phone": "13800138000",
    "id_number": "身份证",
    "adress":"北京市" 
  }
}
```

#### 修改个人信息

**姓名**

**手机号**

**身份证**

**住址**

```
{ 
  "type": "change_user_info",
  "seq": 1050,
  "payload": {
    "user_id": 1234,
    "name":"用户真实姓名",
    "phone": "13800138000",
    "id_number": "身份证",
    "adress":"北京市" 
  }
}
```

```
{
 "ok": true, 
 "seq": 1050, 
}
```

### 消息发送

**请求：**

```
{
"type":"message",
"sender":"patient",
"content":"xxxxxx"
}
```

**接收**

```
{
"type":"message.return",
"sender":"doctor",
 "time":"2025-08-25 10:00",
"content":"xxxxxx"
}
```
