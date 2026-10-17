-- MySQL dump 10.13  Distrib 26.7.0, for Win64 (x86_64)
--
-- Host: localhost    Database: RAKSHANETRA
-- ------------------------------------------------------
-- Server version	26.7.0

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `checkins`
--

DROP TABLE IF EXISTS `checkins`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `checkins` (
  `checkin_id` int NOT NULL AUTO_INCREMENT,
  `worker_id` int NOT NULL,
  `zone_id` int NOT NULL,
  `check_in` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `check_out` datetime DEFAULT NULL,
  PRIMARY KEY (`checkin_id`),
  KEY `worker_id` (`worker_id`),
  KEY `zone_id` (`zone_id`),
  CONSTRAINT `checkins_ibfk_1` FOREIGN KEY (`worker_id`) REFERENCES `workers` (`worker_id`),
  CONSTRAINT `checkins_ibfk_2` FOREIGN KEY (`zone_id`) REFERENCES `zones` (`zone_id`)
) ENGINE=InnoDB AUTO_INCREMENT=32 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `emergencies`
--

DROP TABLE IF EXISTS `emergencies`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `emergencies` (
  `emergency_id` int NOT NULL AUTO_INCREMENT,
  `worker_id` int DEFAULT NULL,
  `site_id` int DEFAULT NULL,
  `accident_type` varchar(50) DEFAULT NULL,
  `people_affected` int DEFAULT NULL,
  `severity` varchar(20) DEFAULT NULL,
  `status` enum('Pending','Dispatched','Escalated','Resolved') NOT NULL DEFAULT 'Pending',
  `assigned_team_id` int DEFAULT NULL,
  `reported_time` datetime DEFAULT CURRENT_TIMESTAMP,
  `resolved_time` datetime DEFAULT NULL,
  `trapped` tinyint(1) NOT NULL DEFAULT '0',
  `unconscious` tinyint(1) NOT NULL DEFAULT '0',
  `zone_id` int DEFAULT NULL,
  PRIMARY KEY (`emergency_id`),
  KEY `worker_id` (`worker_id`),
  KEY `site_id` (`site_id`),
  KEY `assigned_team_id` (`assigned_team_id`),
  KEY `fk_emerg_zone` (`zone_id`),
  CONSTRAINT `emergencies_ibfk_1` FOREIGN KEY (`worker_id`) REFERENCES `workers` (`worker_id`),
  CONSTRAINT `emergencies_ibfk_2` FOREIGN KEY (`site_id`) REFERENCES `sites` (`site_id`),
  CONSTRAINT `emergencies_ibfk_3` FOREIGN KEY (`assigned_team_id`) REFERENCES `rescue_teams` (`team_id`),
  CONSTRAINT `fk_emerg_zone` FOREIGN KEY (`zone_id`) REFERENCES `zones` (`zone_id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `incident_history`
--

DROP TABLE IF EXISTS `incident_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `incident_history` (
  `history_id` int NOT NULL AUTO_INCREMENT,
  `emergency_id` int DEFAULT NULL,
  `action` varchar(100) DEFAULT NULL,
  `action_time` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`history_id`),
  KEY `emergency_id` (`emergency_id`),
  CONSTRAINT `incident_history_ibfk_1` FOREIGN KEY (`emergency_id`) REFERENCES `emergencies` (`emergency_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rescue_teams`
--

DROP TABLE IF EXISTS `rescue_teams`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `rescue_teams` (
  `team_id` int NOT NULL,
  `team_name` varchar(100) NOT NULL,
  `specialization` varchar(50) NOT NULL,
  `availability` enum('Available','Busy') NOT NULL DEFAULT 'Available',
  `workload` int DEFAULT '0',
  `base_zone_id` int DEFAULT NULL,
  PRIMARY KEY (`team_id`),
  KEY `fk_team_zone` (`base_zone_id`),
  CONSTRAINT `fk_team_zone` FOREIGN KEY (`base_zone_id`) REFERENCES `zones` (`zone_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sites`
--

DROP TABLE IF EXISTS `sites`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `sites` (
  `site_id` int NOT NULL,
  `site_name` varchar(100) NOT NULL,
  `location` varchar(100) NOT NULL,
  PRIMARY KEY (`site_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `team_equipment`
--

DROP TABLE IF EXISTS `team_equipment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `team_equipment` (
  `team_id` int NOT NULL,
  `item` varchar(50) NOT NULL,
  PRIMARY KEY (`team_id`,`item`),
  CONSTRAINT `team_equipment_ibfk_1` FOREIGN KEY (`team_id`) REFERENCES `rescue_teams` (`team_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `workers`
--

DROP TABLE IF EXISTS `workers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `workers` (
  `worker_id` int NOT NULL,
  `name` varchar(100) NOT NULL,
  `age` int DEFAULT NULL,
  `role` varchar(50) DEFAULT NULL,
  `site_id` int DEFAULT NULL,
  `phone` varchar(15) DEFAULT NULL,
  `emergency_contact` varchar(15) DEFAULT NULL,
  `training_status` varchar(20) DEFAULT NULL,
  `ppe_status` varchar(20) DEFAULT NULL,
  `zone_id` int DEFAULT NULL,
  `category` enum('Worker','Staff','Contractor') NOT NULL DEFAULT 'Worker',
  `helmet` tinyint(1) NOT NULL DEFAULT '1',
  `vest` tinyint(1) NOT NULL DEFAULT '1',
  `boots` tinyint(1) NOT NULL DEFAULT '1',
  `id_revoked` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`worker_id`),
  KEY `site_id` (`site_id`),
  KEY `fk_worker_zone` (`zone_id`),
  CONSTRAINT `fk_worker_zone` FOREIGN KEY (`zone_id`) REFERENCES `zones` (`zone_id`),
  CONSTRAINT `workers_ibfk_1` FOREIGN KEY (`site_id`) REFERENCES `sites` (`site_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `zone_edges`
--

DROP TABLE IF EXISTS `zone_edges`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `zone_edges` (
  `from_zone` int NOT NULL,
  `to_zone` int NOT NULL,
  `distance_m` int NOT NULL,
  PRIMARY KEY (`from_zone`,`to_zone`),
  KEY `to_zone` (`to_zone`),
  CONSTRAINT `zone_edges_ibfk_1` FOREIGN KEY (`from_zone`) REFERENCES `zones` (`zone_id`),
  CONSTRAINT `zone_edges_ibfk_2` FOREIGN KEY (`to_zone`) REFERENCES `zones` (`zone_id`),
  CONSTRAINT `zone_edges_chk_1` CHECK ((`distance_m` > 0))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `zones`
--

DROP TABLE IF EXISTS `zones`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `zones` (
  `zone_id` int NOT NULL AUTO_INCREMENT,
  `site_id` int NOT NULL,
  `zone_name` varchar(50) NOT NULL,
  `restricted` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`zone_id`),
  UNIQUE KEY `site_id` (`site_id`,`zone_name`),
  CONSTRAINT `zones_ibfk_1` FOREIGN KEY (`site_id`) REFERENCES `sites` (`site_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-10-17 19:17:39
