import os
import requests
from dotenv import load_dotenv
from openai import OpenAI

load_dotenv()  # 加载环境变量

def call_ds_api(request):
    # 调用deepseek的API
    BASE_URL = "https://api.deepseek.com"
    API_KEY = os.getenv("DEEPSEEK_API_KEY")  # 从环境变量获取密钥

    # 验证客户端请求
    client_data = request.json
    if not client_data or 'messages' not in client_data:
        return jsonify({"error": "Invalid request format"}), 400

    # 构造DeepSeek请求
    headers = {
        "Authorization": f"Bearer {API_KEY}",
        "Content-Type": "application/json"
    }

    payload = {
        "model": "deepseek-chat",
        "messages": client_data['messages'],
        "stream": False
    }

    client = OpenAI(api_key=API_KEY, base_url=BASE_URL)

    response = client.chat.completions.create(
        model="deepseek-chat",
        messages=client_data['messages'],
        stream=False
    )

    # 返回处理结果
    return response.choices[0].message.content