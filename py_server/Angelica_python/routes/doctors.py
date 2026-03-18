from flask import Blueprint, jsonify, request
from flask_jwt_extended import jwt_required, get_jwt_identity
from utils.db import execute_query, db_pool

doctors_bp = Blueprint('doctors', __name__, url_prefix='/api/doctors')

# 根据城市名查询医院
@doctors_bp.route('/<int:user_id>/<int:hospital_id>/queryByHospitalId', methods=['GET'])
@jwt_required()
def get_doctors_by_hospital(user_id, hospital_id):
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
            SELECT doctor_id, name, title, department, introduction, expertise, price, picture 
            FROM doctors 
            WHERE hospital_id = %s
        """

        # 参数化查询，防止 SQL 注入

        doctors = execute_query(query, (hospital_id,))
        #print(doctors)
        return jsonify({
            "data": doctors
        }), 200
    except Exception as e:
        print(str(e))
        return jsonify({"error-queryHospitalsByCity": str(e)}), 500