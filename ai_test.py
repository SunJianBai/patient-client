import requests

def ask_ai(question, prompt=None):
    url = "https://api.siliconflow.cn/v1/chat/completions"
    token = "sk-cnaezfpjfsjptfogdusdjsanqxlptrdhyagrifilhthevspn"  # TODO: 替换为你的token
    if prompt is None:
        prompt = '''
你是一名专业健康管理 AI，请根据以下健康调查问卷信息进行风险评估与建议。

任务要求

阅读用户的完整问卷信息。

判定健康风险等级（只能为以下之一：无, 低, 中, 高, 极高）。

给出 3 条简短的健康建议（每条 ≤ 20 字）。

严格返回 JSON 格式，禁止输出额外说明。

风险等级判定逻辑（参考，不可忽略）

无：各项指标良好，无慢性病或家族病史，生活方式健康。

低：轻微不良习惯（如偶尔缺乏运动、饮食略不均衡），无慢性病。

中：存在多项不良习惯（如睡眠不足+不运动+饮食差），但无重大疾病。

高：已有慢性疾病 或 家族病史 + 不良生活习惯。

极高：多项严重风险叠加（如糖尿病+家族史+长期睡眠不足+不运动+饮食差）。

用户问卷信息
一、基本信息
1. 您的年龄范围是？ 18岁以下
2. 您的性别是？ 男
3. 您的体重状况？（自报） 偏瘦

二、生活方式
4. 您平均每天的睡眠时间是？ 少于5小时
5. 您的睡眠质量如何？ 很好
6. 您是否有吸烟习惯？ 从不吸烟
7. 您是否有饮酒习惯？ 从不饮酒
8. 您每周的运动频率是？ 几乎不运动

三、饮食习惯
9. 您平均每天吃蔬菜的次数？ 几乎不吃
10. 您吃快餐/外卖的频率？ 几乎不吃

四、慢性疾病史
慢性疾病史: 糖尿病

五、家族病史
家族病史: 糖尿病

六、心理健康
11. 您最近是否经常感到压力大？ 从不
12. 您是否经常感到情绪低落？ 从不

七、体力与日常活动
13. 您是否能够独立完成日常工作（如买菜、做饭、清洁）？ 完全可以

输出格式
{
  "risk_level": "高",
  "advice": [
    "建议规律作息",
    "增加适度运动",
    "定期监测血糖"
  ]
}

'''
    payload = {
        "model": "deepseek-ai/DeepSeek-V3.1",
        "messages": [
            {
                "role": "user",
                "content": f"{prompt}\n{question}"
            }
        ]
    }
    headers = {
        "Authorization": f"Bearer {token}",
        "Content-Type": "application/json"
    }
    response = requests.post(url, json=payload, headers=headers)
    data = response.json()
    # 解析大模型回复
    try:
        reply = data["choices"][0]["message"]["content"]
    except Exception:
        reply = data
    # 尝试将reply转为标准JSON字符串（去除多余换行/空格/非JSON内容）
    import json, re
    if isinstance(reply, str):
        # 提取第一个JSON对象
        match = re.search(r'\{[\s\S]*\}', reply)
        if match:
            try:
                obj = json.loads(match.group(0))
                return json.dumps(obj, ensure_ascii=False)
            except Exception:
                pass
    return reply

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        # 命令行参数作为问卷内容
        question = sys.argv[1]
        prompt = None  # 用默认或脚本内提示词
    result = ask_ai(question, prompt)
    if isinstance(result, str):
        print(result)
    else:
        import json
        print(json.dumps(result, ensure_ascii=False))
