from flask import Blueprint, jsonify, request
from flask_jwt_extended import jwt_required, get_jwt_identity
from utils.db import execute_query, db_pool

diseases_bp = Blueprint('diseases', __name__, url_prefix='/api/diseases')

# 皮肤病图鉴
@diseases_bp.route('/<int:user_id>/<int:disease_id>', methods=['GET'])
@jwt_required()
def get_Diseases_by_id(user_id, disease_id):
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
                stage_count,
                stage1_name, stage1_image, stage1_desc,
                stage2_name, stage2_image, stage2_desc,
                stage3_name, stage3_image, stage3_desc,
                stage4_name, stage4_image, stage4_desc,
                stage5_name, stage5_image, stage5_desc
            FROM diseases 
            WHERE id = %s
        """
        result = execute_query(query, (disease_id,))

        if not result:
            return jsonify({"error": "Disease not found"}), 404

        disease_data = result[0]
        stages = []
        stage_count = disease_data['stage_count']

        for i in range(1, stage_count + 1):
            stage = {
                "name": disease_data.get(f'stage{i}_name'),
                "image": disease_data.get(f'stage{i}_image', ''),
                "description": disease_data.get(f'stage{i}_desc') or "暂无描述"
            }
            stages.append(stage)
        #print(stages)
        return jsonify({
            "id": disease_id,
            "stages": stages
        }), 200
    except Exception as e:
        print(str(e))
        return jsonify({"error-queryPat": str(e)}), 500