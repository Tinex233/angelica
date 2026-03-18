from flask import Blueprint, jsonify, request
from flask_jwt_extended import jwt_required, get_jwt_identity
from utils.db import execute_query, db_pool

hospitals_bp = Blueprint('hospitals', __name__, url_prefix='/api/hospitals')

# 根据城市名查询医院
@hospitals_bp.route('/<int:user_id>/<string:city>/queryByCity', methods=['GET'])
@jwt_required()
def get_hospitals_by_city(user_id, city):
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
            SELECT hospital_id, name, region, address, hospital_level, picture 
            FROM hospitals 
            WHERE region LIKE %s
        """

        # 参数化查询，防止 SQL 注入
        city_pattern = f"%{city}%"
        hospitals = execute_query(query, (city_pattern,))

        return jsonify({
            "data": hospitals
        }), 200
    except Exception as e:
        return jsonify({"error-queryHospitalsByCity": str(e)}), 500