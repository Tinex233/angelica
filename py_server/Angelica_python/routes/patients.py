from flask import Blueprint, jsonify, request
from flask_jwt_extended import jwt_required, get_jwt_identity
from utils.db import execute_query, db_pool

patients_bp = Blueprint('patients', __name__, url_prefix='/api/patients')

#根据USER_ID查询就诊人信息
@patients_bp.route('/<int:user_id>/queryPatients', methods=['GET'])
@jwt_required()
def get_patient_by_id(user_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户信息",
                "code": 403
            }), 403

        result = execute_query(
            "SELECT PATIENT_NUMBER, NAME, ID_TYPE, ID_NUMBER, PHONE, CONSTITUTION FROM patients WHERE USER_ID = %s",
            (user_id,))

        #print(result)
        return jsonify({
            "data": result  # 直接返回查询结果列表
        }), 200
    except Exception as e:
        return jsonify({"error-queryPat": str(e)}), 500

#添加就诊人信息到USER_ID
@patients_bp.route('/<int:user_id>/addPatients', methods=['POST'])
@jwt_required()
def add_patient(user_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户信息",
                "code": 403
            }), 403

        data = request.get_json()
        if not data:
            return jsonify({"error": "请求数据为空"}), 400

        # 必填字段验证
        required_fields = ['name', 'id_type', 'id_number', 'phone']
        for field in required_fields:
            if field not in data:
                return jsonify({"error": f"缺失必填字段: {field}"}), 400

        # 获取下一个可用编号
        def get_next_patient_number(conn, user_id):
            with conn.cursor() as cursor:
                # 锁定用户记录确保事务安全
                cursor.execute("SELECT MAX(patient_number) FROM patients WHERE user_id = %s FOR UPDATE", (user_id,))
                max_number = cursor.fetchone()[0]
                return (max_number or 0) + 1  # 从1开始递增

        # 在事务中执行操作
        conn = db_pool.get_connection()
        conn.start_transaction()  # 开启事务

        try:
            next_number = get_next_patient_number(conn, user_id)

            # 插入新就诊人
            with conn.cursor() as cursor:
                cursor.execute(
                    """
                    INSERT INTO patients 
                        (user_id, patient_number, name, id_type, id_number, phone, constitution)
                    VALUES 
                        (%s, %s, %s, %s, %s, %s, %s)
                    """,
                    (
                        user_id,
                        next_number,
                        data['name'],
                        data['id_type'],
                        data['id_number'],
                        data['phone'],
                        data.get('constitution', None)
                    )
                )
            conn.commit()  # 提交事务
            return jsonify({"success": True, "patient_number": next_number}), 201

        except Exception as e:
            conn.rollback()  # 回滚事务
            raise e
        finally:
            conn.close()

    except Exception as e:
        print(f"[ERROR] 添加就诊人失败: {str(e)}")
        return jsonify({"error": str(e)}), 500

#删除USER_ID的就诊人
@patients_bp.route('/<int:user_id>/deletePatients', methods=['POST'])
@jwt_required()
def del_patient(user_id):
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

        # 执行数据库删除
        execute_query(
            """
            DELETE FROM patients WHERE
                user_id = %s and patient_number = %s
            """,
            (user_id, data['patient_number'])
        )

        return jsonify({"success": True}), 200

    except Exception as e:
        print(f"[ERROR] 删除就诊人失败: {str(e)}")
        return jsonify({"error": str(e)}), 500

#修改USER_ID的指定就诊人的体质
@patients_bp.route('/<int:user_id>/modPatConstitution', methods=['POST'])
@jwt_required()
def mod_patient_cons(user_id):
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
            UPDATE patients 
            SET constitution = %s
            WHERE user_id = %s and patient_number = %s
            """,
            (data['constitution'],user_id, data['patient_number'])
        )

        return jsonify({"success": True}), 200

    except Exception as e:
        print(f"[ERROR] 修改就诊人体质失败: {str(e)}")
        return jsonify({"error": str(e)}), 500

#修改USER_ID的指定就诊人的信息
@patients_bp.route('/<int:user_id>/modPatient', methods=['POST'])
@jwt_required()
def mod_patient(user_id):
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

        name = data['patient_number']
        idtype = data['id_type']
        idnum = data['id_number']
        phone = data['phone']


        execute_query(
            """
            UPDATE patients 
            SET constitution = %s
            WHERE user_id = %s and patient_number = %s
            """,
            (data['constitution'],user_id, data['patient_number'])
        )

        return jsonify({"success": True}), 200

    except Exception as e:
        print(f"[ERROR] 修改就诊人体质失败: {str(e)}")
        return jsonify({"error": str(e)}), 500