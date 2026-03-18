from flask import Blueprint, jsonify, request
from flask_jwt_extended import jwt_required, get_jwt_identity
from utils.db import execute_query, db_pool

appointments_bp = Blueprint('appointments', __name__, url_prefix='/api/appointments')

@appointments_bp.route('/<int:user_id>/addAppointment', methods=['POST'])
@jwt_required()
def add_appointment(user_id):
    # 获取Token中的用户身份
    current_user_id = int(get_jwt_identity())

    # 验证用户身份一致性
    if current_user_id != user_id:
        return jsonify({
            "error": "无权访问其他用户信息",
            "code": 403
        }), 403

    # 信用等级检查
    check = """
        SELECT credit_level
        FROM users
        WHERE user_id = %s
    """

    try:
        allow = execute_query(check, (user_id,))
        if (allow[0]['credit_level'] == 0):
            return jsonify({"message": "no credit"}), 201

    except Exception as e:
        print(str(e))
        return jsonify({"error": str(e)}), 500

    data = request.json
    hospital_id = data.get('hospital_id')
    doctor_id = data.get('doctor_id')
    price = data.get('price')
    appointment_date = data.get('appointment_date')
    time_slot = data.get('time_slot')
    disease_description = data.get('disease_description')
    status = "待就诊"
    patient_name = data.get('patient_name')
    id_type = data.get('id_type')
    id_number = data.get('id_number')
    phone = data.get('phone')

    schedule_id = data.get('schedule_id')
    # try:
    #     # 开始事务
    #     conn = db_pool.get_connection()
    #     cursor = conn.cursor()
    #
    #     # 使用行级锁检查可用预约量
    #     cursor.execute("SELECT available_slots FROM doctor_schedule WHERE schedule_id = %s FOR UPDATE", (schedule_id,))
    #     available_slots = cursor.fetchone()
    #
    #     if available_slots and available_slots[0] > 0:
    #         # 插入预约记录
    #         query_insert = """
    #             INSERT INTO appointment (user_id, hospital_id, doctor_id, price, appointment_date, time_slot, disease_description, status, patient_name, id_type, id_number, phone, constitution)
    #             VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
    #         """
    #         cursor.execute(query_insert, (user_id, hospital_id, doctor_id, price, appointment_date, time_slot, disease_description, status, patient_name, id_type, id_number, phone, data.get('constitution', None)))
    #
    #         # 更新可用预约量
    #         query_update_slots = """
    #             UPDATE doctor_schedule
    #             SET available_slots = available_slots - 1
    #             WHERE schedule_id = %s AND available_slots > 0
    #         """
    #         cursor.execute(query_update_slots, (schedule_id,))
    #
    #         uodate_status = """
    #             UPDATE users
    #             SET status = '挂号中'
    #             WHERE user_id = %s
    #         """
    #         cursor.execute(uodate_status, (user_id,))
    #
    #         # 提交事务
    #         conn.commit()
    #         return jsonify({"message": "预约成功"}), 201
    #     else:
    #         return jsonify({"error": "预约名额不足，请更换医师"}), 400
    #
    # except Exception as e:
    #     conn.rollback()  # 回滚事务
    #     print(str(e))
    #     return jsonify({"error": str(e)}), 500
    # finally:
    #     cursor.close()
    #     conn.close()
    try:
        # 开始事务
        conn = db_pool.get_connection()
        cursor = conn.cursor()

        # 使用行级锁检查可用预约量
        cursor.execute("SELECT available_slots FROM doctor_schedule WHERE schedule_id = %s FOR UPDATE", (schedule_id,))
        available_slots = cursor.fetchone()

        if available_slots and available_slots[0] > 0:
            # 更新可用预约量（尽量缩短锁的持有时间）
            query_update_slots = """
                UPDATE doctor_schedule
                SET available_slots = available_slots - 1
                WHERE schedule_id = %s AND available_slots > 0
            """
            cursor.execute(query_update_slots, (schedule_id,))

            # 插入预约记录
            query_insert = """
                INSERT INTO appointment (user_id, hospital_id, doctor_id, price, appointment_date, time_slot, disease_description, status, patient_name, id_type, id_number, phone, constitution)
                VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
            """
            cursor.execute(query_insert, (
            user_id, hospital_id, doctor_id, price, appointment_date, time_slot, disease_description, status,
            patient_name, id_type, id_number, phone, data.get('constitution', None)))

            # 提交事务
            conn.commit()
            return jsonify({"message": "预约成功"}), 201
        else:
            return jsonify({"error": "预约名额不足，请更换医师"}), 400

    except Exception as e:
        conn.rollback()  # 回滚事务
        print(str(e))
        return jsonify({"error": str(e)}), 500
    finally:
        cursor.close()
        conn.close()

# 根据预约状态分类返回订单信息
@appointments_bp.route('/queryByStatus/<int:user_id>', methods=['GET'])
@jwt_required()
def query_appointments_by_status(user_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户信息",
                "code": 403
            }), 403

        # 从 GET 请求中获取查询参数 status
        status = request.args.get('status')

        # 构建基础 SQL 查询语句
        base_query = """
            SELECT 
                a.status,
                a.appointment_id,
                a.patient_name,
                a.appointment_date,
                a.time_slot,
                h.name AS hospital_name
            FROM 
                appointment a
            INNER JOIN 
                hospitals h
            ON 
                a.hospital_id = h.hospital_id
            WHERE 
                a.user_id = %s
        """

        # 如果 status 参数存在且有效，则添加到查询条件
        valid_statuses = ['已完成', '已取消', '待就诊', '已爽约']
        params = [user_id]
        if status:
            if status not in valid_statuses:
                return jsonify({"error": "无效的状态值"}), 400
            base_query += " AND a.status = %s"
            params.append(status)

        # 添加排序
        base_query += " ORDER BY a.appointment_date ASC"

        # 执行查询
        results = execute_query(base_query, tuple(params))

        # 返回查询结果
        return jsonify({
            #"status": status if status else "所有状态",
            "user_id": user_id,
            "data": results
        }), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500

# 查询指定appointment_id的详细信息
@appointments_bp.route('/queryDetails/<int:user_id>/<int:appointment_id>', methods=['GET'])
@jwt_required()
def get_appointment_details(user_id, appointment_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户信息",
                "code": 403
            }), 403

        # 连接appointment、hospitals 和 doctors表
        query = """
            SELECT 
                a.appointment_id,
                a.patient_name,
                a.appointment_date,
                a.time_slot,
                a.disease_description,
                a.price,
                a.status,
                a.created_at,
                a.id_type,
                a.id_number,
                a.phone,
                a.constitution,
                h.name AS hospital_name,
                h.region AS hospital_region,
                h.address AS hospital_address,
                d.name AS doctor_name
            FROM 
                appointment a
            INNER JOIN 
                hospitals h ON a.hospital_id = h.hospital_id
            INNER JOIN 
                doctors d ON a.doctor_id = d.doctor_id
            WHERE 
                a.appointment_id = %s
        """

        # 执行查询
        result = execute_query(query, (appointment_id,))

        if result:
            return jsonify(result[0]), 200  # 返回第一条结果
        else:
            return jsonify({"error": "未找到指定的预约记录"}), 404

    except Exception as e:
        return jsonify({"error": str(e)}), 500

# 取消预约功能
@appointments_bp.route('/cancel/<int:user_id>/<int:appointment_id>', methods=['PUT'])
@jwt_required()
def cancel_appointment(user_id, appointment_id):
    try:
        # 获取Token中的用户身份
        current_user_id = int(get_jwt_identity())

        # 验证用户身份一致性
        if current_user_id != user_id:
            return jsonify({
                "error": "无权访问其他用户信息",
                "code": 403
            }), 403

        # 检查是否存在该预约记录
        query_check = "SELECT status FROM appointment WHERE appointment_id = %s"
        result = execute_query(query_check, (appointment_id,))

        if not result:
            return jsonify({"error": "预约记录不存在"}), 404

        # 如果状态已经是 "已取消"，直接返回提示
        if result[0]["status"] == "已取消":
            return jsonify({"message": "该预约已被取消"}), 200

        # 更新预约状态为 "已取消"
        query_update = """
            UPDATE appointment 
            SET status = '已取消' 
            WHERE appointment_id = %s
        """
        execute_query(query_update, (appointment_id,))

        # 检测用户状态并更新
        # 检查用户是否有其他待就诊预约
        check_pending_query = """
            SELECT COUNT(*) AS pending_count 
            FROM appointment 
            WHERE user_id = %s 
              AND status = '待就诊'
        """
        pending_result = execute_query(check_pending_query, (user_id,))

        # 如果没有待就诊预约，更新用户状态
        if pending_result and pending_result[0]['pending_count'] == 0:
            update_user_query = """
                UPDATE users 
                SET status = '无挂号' 
                WHERE user_id = %s
            """
            execute_query(update_user_query, (user_id,))

        return jsonify({"message": "预约已成功取消"}), 200

    except Exception as e:
        print(str(e))
        return jsonify({"error": str(e)}), 500