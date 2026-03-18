/*
 Navicat Premium Data Transfer

 Source Server         : mylaptop
 Source Server Type    : MySQL
 Source Server Version : 50731
 Source Host           : localhost:3306
 Source Schema         : angelica

 Target Server Type    : MySQL
 Target Server Version : 50731
 File Encoding         : 65001

 Date: 18/03/2026 17:50:12
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for appointment
-- ----------------------------
DROP TABLE IF EXISTS `appointment`;
CREATE TABLE `appointment`  (
  `appointment_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '预约号（唯一主键）',
  `user_id` int(11) UNSIGNED NOT NULL COMMENT '用户ID（关联 users 表）',
  `hospital_id` int(11) UNSIGNED NOT NULL COMMENT '医院ID（关联 hospitals 表）',
  `doctor_id` int(11) UNSIGNED NOT NULL COMMENT '医生ID（关联 doctors 表）',
  `price` int(11) NOT NULL COMMENT '挂号价格（单位：元）',
  `appointment_date` date NOT NULL COMMENT '预约日期（冗余存储，与排班日期一致）',
  `time_slot` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL,
  `disease_description` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL COMMENT '疾病描述文本',
  `status` enum('已完成','已取消','待就诊','已爽约') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT '待就诊' COMMENT '订单状态：已完成、已取消、待就诊、已失约',
  `created_at` timestamp(0) NOT NULL DEFAULT CURRENT_TIMESTAMP(0),
  `patient_name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `id_type` enum('居民身份证','护照','港澳居民来往内地通行证','台湾居民来往大陆通行证') CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '居民身份证',
  `id_number` varchar(30) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `phone` varchar(15) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `constitution` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  PRIMARY KEY (`appointment_id`) USING BTREE,
  INDEX `idx_user`(`user_id`) USING BTREE,
  INDEX `idx_doctor_date`(`doctor_id`, `appointment_date`) USING BTREE,
  INDEX `idx_status`(`status`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 103 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '预约挂号记录表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for diseases
-- ----------------------------
DROP TABLE IF EXISTS `diseases`;
CREATE TABLE `diseases`  (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `stage_count` tinyint(10) NOT NULL,
  `stage1_image` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `stage1_desc` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `stage1_name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `stage2_image` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `stage2_desc` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL,
  `stage2_name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `stage3_image` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `stage3_desc` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL,
  `stage3_name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `stage4_image` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `stage4_desc` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL,
  `stage4_name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `stage5_image` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `stage5_desc` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL,
  `stage5_name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for doctor_schedule
-- ----------------------------
DROP TABLE IF EXISTS `doctor_schedule`;
CREATE TABLE `doctor_schedule`  (
  `schedule_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '排班记录ID',
  `doctor_id` int(11) NOT NULL COMMENT '医生ID（关联 doctors 表）',
  `date` date NOT NULL COMMENT '排班日期（如：2024-04-12）',
  `time_slot` enum('上午','下午') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '时段：上午（am）、下午（pm）',
  `available_slots` int(11) NOT NULL DEFAULT 30 COMMENT '剩余可预约量（初始30）',
  PRIMARY KEY (`schedule_id`) USING BTREE,
  UNIQUE INDEX `unique_doctor_schedule`(`doctor_id`, `date`, `time_slot`) USING BTREE,
  CONSTRAINT `doctor_schedule_ibfk_1` FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`doctor_id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 64 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for doctors
-- ----------------------------
DROP TABLE IF EXISTS `doctors`;
CREATE TABLE `doctors`  (
  `doctor_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '医生唯一ID',
  `hospital_id` int(11) NOT NULL COMMENT '所属医院ID',
  `name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '医生姓名',
  `title` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '职称（如：主任医师、副主任医师）',
  `department` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '所属科室（如：心血管内科）',
  `introduction` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '医生简介',
  `expertise` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '擅长领域',
  `price` int(10) NULL DEFAULT NULL COMMENT '挂号费',
  `picture` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '医生图片路径',
  PRIMARY KEY (`doctor_id`) USING BTREE,
  INDEX `hospital_id`(`hospital_id`) USING BTREE,
  CONSTRAINT `doctors_ibfk_1` FOREIGN KEY (`hospital_id`) REFERENCES `hospitals` (`hospital_id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for hospitals
-- ----------------------------
DROP TABLE IF EXISTS `hospitals`;
CREATE TABLE `hospitals`  (
  `hospital_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `region` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `address` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `hospital_level` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'UNKNOWN' COMMENT '医院等级（如：三级甲等、专科医院、社区医院等）',
  `picture` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '照片图片路径',
  PRIMARY KEY (`hospital_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 107 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for itch_assessment
-- ----------------------------
DROP TABLE IF EXISTS `itch_assessment`;
CREATE TABLE `itch_assessment`  (
  `user_id` int(11) UNSIGNED NOT NULL COMMENT '关联用户ID',
  `date` date NOT NULL COMMENT '评估日期（格式：YYYY-MM-DD，存储完整日期）',
  `itch_value` tinyint(3) UNSIGNED NOT NULL COMMENT '瘙痒评估值（取值范围0-10，由应用层校验）',
  INDEX `itch_assessment`(`user_id`) USING BTREE,
  CONSTRAINT `itch_assessment` FOREIGN KEY (`user_id`) REFERENCES `users` (`USER_ID`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '用户每日瘙痒评估记录' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for pain_assessment
-- ----------------------------
DROP TABLE IF EXISTS `pain_assessment`;
CREATE TABLE `pain_assessment`  (
  `user_id` int(10) UNSIGNED NOT NULL COMMENT '关联用户ID',
  `date` date NOT NULL COMMENT '评估日期（格式：YYYY-MM-DD，存储完整日期）',
  `pain_value` tinyint(3) UNSIGNED NOT NULL COMMENT '疼痛评估值（取值范围0-10，由应用层校验）',
  INDEX `pain_assessment`(`user_id`) USING BTREE,
  CONSTRAINT `pain_assessment` FOREIGN KEY (`user_id`) REFERENCES `users` (`USER_ID`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '用户每日疼痛评估记录' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for patients
-- ----------------------------
DROP TABLE IF EXISTS `patients`;
CREATE TABLE `patients`  (
  `USER_ID` int(11) UNSIGNED NOT NULL COMMENT '用户唯一标识',
  `PATIENT_NUMBER` int(11) NOT NULL COMMENT '每个账号中就诊人唯一标识',
  `NAME` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '姓名',
  `ID_TYPE` enum('居民身份证','护照','港澳居民来往内地通行证','台湾居民来往大陆通行证') CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '居民身份证' COMMENT '证件类型',
  `ID_NUMBER` varchar(30) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '证件号码',
  `PHONE` varchar(15) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '手机号',
  `CONSTITUTION` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '中医体质类型',
  UNIQUE INDEX `ID_NUMBER`(`ID_NUMBER`) USING BTREE,
  UNIQUE INDEX `PHONE`(`PHONE`) USING BTREE,
  INDEX `patients`(`USER_ID`) USING BTREE,
  CONSTRAINT `patients` FOREIGN KEY (`USER_ID`) REFERENCES `users` (`USER_ID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for token_blacklist
-- ----------------------------
DROP TABLE IF EXISTS `token_blacklist`;
CREATE TABLE `token_blacklist`  (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `jti` varchar(36) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL,
  `expires_at` datetime(0) NOT NULL,
  `created_at` datetime(0) NULL DEFAULT CURRENT_TIMESTAMP(0),
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `jti`(`jti`) USING BTREE,
  INDEX `idx_jti`(`jti`) USING BTREE,
  INDEX `idx_expires_at`(`expires_at`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users`  (
  `USER_ID` int(11) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '用户唯一标识（自增主键）',
  `PASSWORD_HASH` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `USER_NAME` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `CREDIT_LEVEL` tinyint(4) NOT NULL DEFAULT 2,
  `STATUS` enum('挂号中','无挂号') CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '无挂号',
  `PHONE_NUMBER` varchar(15) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `EMAIL` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `WECHAT` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  PRIMARY KEY (`USER_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 100009 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Event structure for daily_schedule_maintenance
-- ----------------------------
DROP EVENT IF EXISTS `daily_schedule_maintenance`;
delimiter ;;
CREATE EVENT `daily_schedule_maintenance`
ON SCHEDULE
EVERY '1' DAY STARTS '2025-04-13 00:00:00'
ON COMPLETION PRESERVE
DO BEGIN
  START TRANSACTION;
  
  -- 删除昨天的过期排班记录
  DELETE FROM doctor_schedule 
  WHERE `date` = CURRENT_DATE - INTERVAL 1 DAY;

  -- 插入七天后的排班（仅当不存在时插入）
  INSERT INTO doctor_schedule (doctor_id, `date`, time_slot, available_slots)
  SELECT 
    d.doctor_id,
    CURRENT_DATE + INTERVAL 7 DAY AS target_date,
    ts.time_slot,
    30  -- 初始可用号量
  FROM doctors d
  CROSS JOIN (
    SELECT '上午' AS time_slot 
    UNION ALL 
    SELECT '下午'
  ) ts  -- 生成所有医生+时间段的组合
  WHERE NOT EXISTS (
    SELECT 1
    FROM doctor_schedule ds
    WHERE ds.doctor_id = d.doctor_id
      AND ds.`date` = CURRENT_DATE + INTERVAL 7 DAY
      AND ds.time_slot = ts.time_slot
  );

  COMMIT;
END
;;
delimiter ;

-- ----------------------------
-- Event structure for reset_credit_level_monthly
-- ----------------------------
DROP EVENT IF EXISTS `reset_credit_level_monthly`;
delimiter ;;
CREATE EVENT `reset_credit_level_monthly`
ON SCHEDULE
EVERY '1' MONTH STARTS '2025-05-01 00:00:00'
DO UPDATE users
  SET credit_level = 2
;;
delimiter ;

-- ----------------------------
-- Event structure for update_schedule_event
-- ----------------------------
DROP EVENT IF EXISTS `update_schedule_event`;
delimiter ;;
CREATE EVENT `update_schedule_event`
ON SCHEDULE
EVERY '1' DAY STARTS '2025-04-26 00:00:00'
DO CALL daily_schedule_maintenance()
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
