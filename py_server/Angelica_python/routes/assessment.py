from flask import Blueprint, jsonify, request
from flask_jwt_extended import jwt_required, get_jwt_identity
from utils.db import execute_query, db_pool
from datetime import datetime, timedelta

assessments_bp = Blueprint('assessments', __name__, url_prefix='/api/assessments')

# 提交瘙痒评估记录
@assessments_bp.route('/addItch/<int:user_id>', methods=['POST'])
@jwt_required()
def submit_itch_assessment(user_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户数据",
                "code": 403
            }), 403

        # 获取请求参数
        data = request.get_json()
        if not data or 'itch_value' not in data:
            return jsonify({"error": "缺少必要参数 itch_value"}), 400

        itch_value = int(data['itch_value'])

        query = """
            INSERT INTO itch_assessment 
                (user_id, date, itch_value)
            VALUES 
                (%s, CURRENT_DATE(), %s)
        """

        # 执行插入/更新操作
        execute_query(query, (user_id, itch_value),)
        return jsonify({
                "message": "评估记录已保存",
            }), 201

    except ValueError:
        return jsonify({"error": "无效的评估值格式"}), 400
    except Exception as e:
        print(str(e))
        return jsonify({"error": "服务器内部错误"}), 500

# 提交疼痛评估记录
@assessments_bp.route('/addPain/<int:user_id>', methods=['POST'])
@jwt_required()
def submit_pain_assessment(user_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户数据",
                "code": 403
            }), 403

        # 获取请求参数
        data = request.get_json()
        if not data or 'pain_value' not in data:
            return jsonify({"error": "缺少必要参数 pain_value"}), 400

        pain_value = int(data['pain_value'])

        query = """
            INSERT INTO pain_assessment 
                (user_id, date, pain_value)
            VALUES 
                (%s, CURRENT_DATE(), %s)
        """

        # 执行插入/更新操作
        execute_query(query, (user_id, pain_value),)
        return jsonify({
                "message": "评估记录已保存",
            }), 201

    except ValueError:
        return jsonify({"error": "无效的评估值格式"}), 400
    except Exception as e:
        print(str(e))
        return jsonify({"error": "服务器内部错误"}), 500

# 查询今日评估

@assessments_bp.route('/today/<int:user_id>', methods=['GET'])
@jwt_required()
def get_today_assessment(user_id):
    try:
        current_user_id = int(get_jwt_identity())
        if current_user_id != user_id:
            return jsonify({"error": "无权访问其他用户数据", "code": 403}), 403

        # 复合查询SQL
        query = """
            SELECT 
                IF(i.itch_value IS NULL, 0, 1) AS itch_assessed,
                IF(p.pain_value IS NULL, 0, 1) AS pain_assessed,
                i.itch_value,
                p.pain_value
            FROM users u
            LEFT JOIN itch_assessment i 
                ON u.user_id = i.user_id 
                AND i.date = CURDATE()
            LEFT JOIN pain_assessment p 
                ON u.user_id = p.user_id 
                AND p.date = CURDATE()
            WHERE u.user_id = %s
        """

        result = execute_query(query, (user_id,))

        if not result:
            return jsonify({"error": "用户不存在"}), 404

        return jsonify(result[0]), 200

    except Exception as e:
        print(str(e))
        return jsonify({"error": "服务器内部错误"}), 500

# 查询过去7天的记录
@assessments_bp.route('/weekly_records/<int:user_id>', methods=['GET'])
@jwt_required()
def get_weekly_records(user_id):
    try:
        # 验证用户身份
        current_user_id = int(get_jwt_identity())
        if current_user_id != user_id:
            return jsonify({"error": "无权访问其他用户数据", "code": 403}), 403

        # 生成过去7天日期序列
        today = datetime.today().date()
        date_list = [today - timedelta(days=i) for i in range(6, -1, -1)]  # 从6天前到今日

        # 查询瘙痒记录
        itch_query = """
            SELECT date, itch_value 
            FROM itch_assessment 
            WHERE user_id = %s 
              AND date BETWEEN %s AND %s
        """
        itch_records = execute_query(itch_query,(user_id, date_list[0], date_list[-1]))
        itch_map = {r["date"].strftime("%Y-%m-%d"): r["itch_value"] for r in itch_records}

        # 查询疼痛记录
        pain_query = """
            SELECT date, pain_value 
            FROM pain_assessment 
            WHERE user_id = %s 
              AND date BETWEEN %s AND %s
        """
        pain_records = execute_query(pain_query, (user_id, date_list[0], date_list[-1]))
        pain_map = { r["date"].strftime("%Y-%m-%d"): r["pain_value"] for r in pain_records }

        response_data = []
        for date_obj in date_list:
            date_str = date_obj.strftime("%Y-%m-%d")
            response_data.append({
                "date": date_str,
                "itch_value": itch_map.get(date_str, None),
                "pain_value": pain_map.get(date_str, None)
            })

        return jsonify({"records": response_data}), 200

    except Exception as e:
        print(f"服务器错误: {str(e)}")
        return jsonify({"error": "服务器内部错误"}), 500