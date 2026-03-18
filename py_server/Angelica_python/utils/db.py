# 公共数据库模块
import mysql.connector
from mysql.connector import pooling
from dotenv import load_dotenv
import os

load_dotenv()

# 连接池配置
db_pool = pooling.MySQLConnectionPool(
    pool_name="main_pool",
    pool_size=5,
    host=os.getenv("DB_HOST"),
    user=os.getenv("DB_USER"),
    password=os.getenv("DB_PASSWORD"),
    database=os.getenv("DB_NAME")
)

def execute_query(query, params=None, connection=None):
    """执行安全查询并返回结果"""
    try:
        if connection is None:
            conn = db_pool.get_connection()
        else:
            conn = connection  # 使用传入的连接

        cursor = conn.cursor(dictionary=True)

        if params is None:
            params = ()  # 无参数时传空元组
        elif not isinstance(params, (tuple, list)):
            params = (params,)  # 单个参数转为元组

        cursor.execute(query, params)

        # 判断是否为写入操作（INSERT/UPDATE/DELETE）
        if query.strip().upper().startswith(("INSERT", "UPDATE", "DELETE")):
            conn.commit()  # 提交事务

        result = cursor.fetchall()
        cursor.close()

        if connection is None:
            conn.close()  # 只有在没有传入连接时才关闭连接

        return result
    except mysql.connector.Error as err:
        raise RuntimeError(f"数据库操作失败: {err}")
