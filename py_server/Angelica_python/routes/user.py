from flask import Blueprint, jsonify, request
from flask_jwt_extended import jwt_required, get_jwt_identity
from utils.db import execute_query, db_pool

user_bp = Blueprint('user', __name__, url_prefix='/api/user')

# 查询指定用户的详细信息
@user_bp.route('/queryUser/<int:user_id>', methods=['GET'])
@jwt_required()
def get_appointment_details(user_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户信息",
                "code": 403
            }), 403

        query = """
            SELECT 
                user_name,
                credit_level,
                status,
                phone_number,
                email,
                wechat
            FROM 
                users
            WHERE 
                user_id = %s
        """

        # 执行查询
        result = execute_query(query, (user_id,))

        if result:
            return jsonify(result[0]), 200  # 返回第一条结果
        else:
            return jsonify({"error": "未找到指定的用户记录"}), 404

    except Exception as e:
        print(str(e))
        return jsonify({"error": str(e)}), 500

#修改USER_ID的账号信息
@user_bp.route('/<int:user_id>/modInfo', methods=['POST'])
@jwt_required()
def mod_user_info(user_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户信息",
                "code": 403
            }), 403

        # 获取请求体JSON数据
        data = request.get_json()
        if not data:
            return jsonify({"error": "请求数据为空"}), 400

        execute_query(
            """
            UPDATE users
            SET phone_number = %s,
                email = %s,
                wechat = %s
            WHERE user_id = %s
            """,
            (data.get('phone', None), data.get('email', None), data.get('wechat', None), user_id)
        )

        return jsonify({"success": True}), 200

    except Exception as e:
        print(f"[ERROR] 修改用户信息失败: {str(e)}")
        return jsonify({"error": str(e)}), 500