from flask import Blueprint, jsonify, request
from flask_jwt_extended import create_access_token, get_jwt, jwt_required
from datetime import datetime
from utils.db import execute_query, db_pool
import bcrypt
import re

# 创建蓝图
auth_bp = Blueprint('auth', __name__, url_prefix='/api/auth')

# 用户登录
@auth_bp.route('/login', methods=['POST'])
def login():
    try:
        data = request.get_json()

        # 验证输入
        if not data or 'user_id' not in data or 'password' not in data:
            return jsonify({"error": "User ID and password are required."}), 400

        user_id = data['user_id']
        input_password = data['password']

        # 查询用户的密码哈希
        query = "SELECT USER_NAME, PASSWORD_HASH FROM users WHERE USER_ID = %s"
        result = execute_query(query, (user_id,))

        if not result:
            return jsonify({"error": "User not found."}), 404

        stored_password_hash = result[0]['PASSWORD_HASH']

        # 验证密码
        if bcrypt.checkpw(input_password.encode('utf-8'), stored_password_hash.encode('utf-8')):
            # 生成JWT Token（包含额外声明）
            access_token = create_access_token(
                identity=user_id,
                additional_claims={
                    "username": result[0]['USER_NAME'],
                    "role": "user"  # 可从数据库读取实际角色
                }
            )

            return jsonify({
                "message": "Login successful",
                "access_token": access_token
            }), 200

            #return jsonify({"message": "Login successful", "user_id": user_id}), 200
        else:
            return jsonify({"error": "Invalid password."}), 401

    except Exception as e:
        print(str(e))
        return jsonify({"error": "Login failed", "details": str(e)}), 500

# 用户注册
@auth_bp.route('/register', methods=['POST'])
def register():
    try:
        data = request.get_json()

        # 验证输入
        if not data or 'user_name' not in data or 'password' not in data:
            return jsonify({"error": "Username and password are required."}), 400

        username = data['user_name']
        input_password = data['password']

        # 加密密码
        hashed_password = bcrypt.hashpw(input_password.encode('utf-8'), bcrypt.gensalt()).decode('utf-8')

        # 获取数据库连接
        conn = db_pool.get_connection()
        try:
            # 插入新用户到数据库
            query = """
                INSERT INTO users (USER_NAME, PASSWORD_HASH)
                VALUES (%s, %s)
            """
            execute_query(query, (username, hashed_password), connection=conn)

            # 获取新用户的 ID
            user_id_query = "SELECT LAST_INSERT_ID() AS user_id"
            user_id_result = execute_query(user_id_query, connection=conn)
            user_id = user_id_result[0]['user_id'] if user_id_result else None

            return jsonify({"message": "Registration successful!", "user_id": user_id}), 201
        finally:
            conn.close()  # 确保连接在操作完成后关闭

    except Exception as e:
        print(str(e))
        return jsonify({"error": "Registration failed", "details": str(e)}), 500

# 用户登出
@auth_bp.route('/logout', methods=['DELETE'])
@jwt_required()
def logout():
    try:

        # 获取当前 Token 信息
        jwt_data = get_jwt()
        jti = jwt_data['jti']
        exp_timestamp = jwt_data['exp']

        # 转换过期时间为 datetime
        expires_at = datetime.fromtimestamp(exp_timestamp)

        # 将 Token 加入黑名单
        execute_query(
            "INSERT INTO token_blacklist (jti, expires_at) VALUES (%s, %s)",
            (jti, expires_at)
        )
        return jsonify({"message": "成功退出登录"}), 200

    except Exception as e:
        print(str(e))
        return jsonify({"error": "退出登录失败"}), 500