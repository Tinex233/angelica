from flask import Flask, request, jsonify
from flask_jwt_extended import JWTManager, get_jwt_identity, verify_jwt_in_request, jwt_required
from datetime import timedelta
from flask_cors import CORS
from ds import call_ds_api
import requests
import os
from utils.db import execute_query
from routes.patients import patients_bp
from routes.diseases import diseases_bp
from routes.hospitals import hospitals_bp
from routes.doctors import doctors_bp
from routes.doctor_schedule import doctor_schedule_bp
from routes.appointment import appointments_bp
from routes.login import auth_bp
from routes.user import user_bp
from routes.assessment import assessments_bp

app = Flask(__name__)

app.config["JWT_SECRET_KEY"] = os.environ.get("JWT_SECRET_KEY")
app.config["JWT_ACCESS_TOKEN_EXPIRES"] = timedelta(hours=2)  # Token有效期
app.config["JWT_REFRESH_TOKEN_EXPIRES"] = timedelta(days=7)    # Refresh Token有效期
jwt = JWTManager(app)

app.register_blueprint(patients_bp)
app.register_blueprint(diseases_bp)
app.register_blueprint(hospitals_bp)
app.register_blueprint(doctors_bp)
app.register_blueprint(doctor_schedule_bp)
app.register_blueprint(appointments_bp)
app.register_blueprint(auth_bp)
app.register_blueprint(user_bp)
app.register_blueprint(assessments_bp)
CORS(app)  # 解决跨域问题

# JWT错误处理
@jwt.unauthorized_loader
def handle_unauthorized_error(reason):
    return jsonify({"error": "未授权访问", "reason": reason}), 401

@jwt.invalid_token_loader
def handle_invalid_token_error(reason):
    return jsonify({"error": "无效Token", "reason": reason}), 422

@jwt.expired_token_loader
def handle_expired_token_error(jwt_header, jwt_payload):
    return jsonify({"error": "Token已过期", "action": "请重新登录"}), 401

#调用deepseek API
@app.route('/chat/<int:user_id>', methods=['POST'])
@jwt_required()
def handle_chat(user_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户信息",
                "code": 403
            }), 403
        # verify_jwt_in_request(optional=True)

        result = call_ds_api(request)
        return result

    except requests.exceptions.RequestException as e:
        print(str(e))
        return jsonify({"error": f"API请求失败: {str(e)}"}), 500
    except Exception as e:
        print(str(e))
        return jsonify({"error": f"服务器错误: {str(e)}"}), 500

# 黑名单检查
@jwt.token_in_blocklist_loader
def check_if_token_revoked(jwt_header, jwt_payload):
    """
    检查Token是否在黑名单中
    """
    jti = jwt_payload['jti']

    try:
        result = execute_query(
            "SELECT id FROM token_blacklist WHERE jti = %s",
            (jti,)
        )
        return len(result) > 0
    except Exception as e:
        print(f"黑名单检查失败: {str(e)}")
        return True  # 当检查失败时视为Token失效

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
