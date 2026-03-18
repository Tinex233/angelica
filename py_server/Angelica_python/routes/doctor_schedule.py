# doctor_schedule.py
from flask import Blueprint, jsonify, request
from flask_jwt_extended import jwt_required, get_jwt_identity
from utils.db import execute_query

doctor_schedule_bp = Blueprint('doctor_schedule', __name__, url_prefix='/api/doctor_schedule')

# 查询指定医生的排班信息
@doctor_schedule_bp.route('/<int:user_id>/<int:doctor_id>/schedule', methods=['GET'])
@jwt_required()
def get_doctor_schedule(user_id, doctor_id):
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
            SELECT schedule_id, date, time_slot
            FROM doctor_schedule
            WHERE doctor_id = %s AND available_slots > 0
            ORDER BY date ASC, time_slot ASC
        """

        # 使用参数化查询，防止 SQL 注入
        schedule_data = execute_query(query, (doctor_id,))

        # 返回 JSON 响应
        return jsonify({
            "doctor_id": doctor_id,
            "schedule": schedule_data
        }), 200
    except Exception as e:
        print(str(e))
        return jsonify({"error-queryDoctorSchedule": str(e)}), 500