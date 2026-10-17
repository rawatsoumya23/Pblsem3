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
-- Dumping data for table `checkins`
--

LOCK TABLES `checkins` WRITE;
/*!40000 ALTER TABLE `checkins` DISABLE KEYS */;
INSERT INTO `checkins` VALUES (1,101,5,'2026-10-15 20:34:39',NULL),(2,102,6,'2026-10-15 20:34:39',NULL),(3,103,3,'2026-10-15 20:34:39',NULL),(4,104,2,'2026-10-15 20:34:39',NULL),(5,105,6,'2026-10-15 20:34:39',NULL),(6,106,5,'2026-10-15 20:34:39',NULL),(7,107,3,'2026-10-15 20:34:39',NULL),(8,108,4,'2026-10-15 20:34:39',NULL),(10,110,3,'2026-10-15 20:34:39',NULL),(12,112,3,'2026-10-15 20:34:39',NULL),(13,201,5,'2026-10-15 20:34:39',NULL),(14,202,6,'2026-10-15 20:34:39',NULL),(15,203,6,'2026-10-15 20:34:39',NULL),(16,204,3,'2026-10-15 20:34:39',NULL),(17,205,4,'2026-10-15 20:34:39',NULL),(18,206,5,'2026-10-15 20:34:39',NULL),(19,207,3,'2026-10-15 20:34:39',NULL),(20,208,6,'2026-10-15 20:34:39',NULL);
/*!40000 ALTER TABLE `checkins` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `emergencies`
--

LOCK TABLES `emergencies` WRITE;
/*!40000 ALTER TABLE `emergencies` DISABLE KEYS */;
INSERT INTO `emergencies` VALUES (1,101,1,'Electrical',2,NULL,'Pending',NULL,'2026-10-04 19:38:46',NULL,0,0,5),(2,101,1,'Fire',3,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,1,0,5),(3,102,1,'Collapse',1,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,1,1,4),(4,103,1,'Medical',1,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,0,1,6),(5,104,1,'Fall',1,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,0,0,6),(6,105,1,'Vehicle',2,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,0,0,1),(7,106,1,'Fire',1,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,0,0,3),(8,NULL,1,'Medical',1,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,0,0,2),(9,107,1,'Collapse',2,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,1,0,5),(10,108,1,'Medical',2,NULL,'Pending',NULL,'2026-10-15 20:30:29',NULL,0,1,4);
/*!40000 ALTER TABLE `emergencies` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `incident_history`
--

LOCK TABLES `incident_history` WRITE;
/*!40000 ALTER TABLE `incident_history` DISABLE KEYS */;
INSERT INTO `incident_history` VALUES (1,1,'Emergency Reported','2026-10-04 19:39:11');
/*!40000 ALTER TABLE `incident_history` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `rescue_teams`
--

LOCK TABLES `rescue_teams` WRITE;
/*!40000 ALTER TABLE `rescue_teams` DISABLE KEYS */;
INSERT INTO `rescue_teams` VALUES (1,'Medical Team 1','Medical','Available',0,7),(2,'Medical Team 2','Medical','Available',1,7),(3,'Medical Team 3','Medical','Busy',2,7),(4,'Fire Rescue Team 1','Fire','Available',0,8),(5,'Fire Rescue Team 2','Fire','Available',1,8),(6,'Fire Rescue Team 3','Fire','Busy',2,8),(7,'General Rescue Team 1','General','Available',0,1),(8,'General Rescue Team 2','General','Available',1,1);
/*!40000 ALTER TABLE `rescue_teams` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `sites`
--

LOCK TABLES `sites` WRITE;
/*!40000 ALTER TABLE `sites` DISABLE KEYS */;
INSERT INTO `sites` VALUES (1,'Dehradun Highway Site','Dehradun'),(2,'Haridwar Highway Site','Haridwar'),(3,'Rishikesh Highway Site','Rishikesh');
/*!40000 ALTER TABLE `sites` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `team_equipment`
--

LOCK TABLES `team_equipment` WRITE;
/*!40000 ALTER TABLE `team_equipment` DISABLE KEYS */;
INSERT INTO `team_equipment` VALUES (1,'First Aid Kit'),(1,'Stretcher'),(2,'First Aid Kit'),(2,'Stretcher'),(3,'First Aid Kit'),(3,'Stretcher'),(4,'Fire Extinguisher'),(4,'Rescue Kit'),(5,'Fire Extinguisher'),(5,'Rescue Kit'),(6,'Fire Extinguisher'),(6,'Rescue Kit'),(7,'Rescue Kit'),(7,'Stretcher'),(8,'Rescue Kit'),(8,'Stretcher');
/*!40000 ALTER TABLE `team_equipment` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `workers`
--

LOCK TABLES `workers` WRITE;
/*!40000 ALTER TABLE `workers` DISABLE KEYS */;
INSERT INTO `workers` VALUES (101,'Rahul Sharma',28,'Electrician',1,'9876500001','9876501001','Yes','Yes',5,'Worker',1,1,1,0),(102,'Aman Kumar',32,'Mason',1,'9876500002','9876501002','Yes','Yes',6,'Worker',1,1,1,0),(103,'Rohit Singh',26,'Helper',1,'9876500003','9876501003','Yes','Yes',3,'Worker',1,1,1,0),(104,'Vikas Rawat',35,'Supervisor',1,'9876500004','9876501004','Yes','Yes',2,'Staff',1,1,1,0),(105,'Arjun Negi',30,'Welder',1,'9876500005','9876501005','Yes','Yes',6,'Worker',1,1,1,0),(106,'Deepak Joshi',29,'Electrician',1,'9876500006','9876501006','Yes','Yes',5,'Worker',1,1,1,0),(107,'Mohit Bisht',24,'Helper',1,'9876500007','9876501007','No','Yes',3,'Worker',1,1,1,0),(108,'Karan Singh',31,'Machine Operator',1,'9876500008','9876501008','Yes','Yes',4,'Worker',1,1,1,0),(109,'Sahil Kumar',27,'Mason',1,'9876500009','9876501009','Yes','No',6,'Worker',1,1,0,0),(110,'Naveen Rawat',34,'Crane Operator',1,'9876500010','9876501010','Yes','Yes',3,'Worker',1,1,1,0),(111,'Sumit Negi',25,'Helper',1,'9876500011','9876501011','Yes','Yes',3,'Worker',1,1,1,1),(112,'Rakesh Thakur',38,'Site Supervisor',1,'9876500012','9876501012','Yes','Yes',3,'Staff',1,1,1,0),(201,'Aakash Verma',29,'Electrician',1,'9876500021','9876501021','Yes','Yes',5,'Worker',1,1,1,0),(202,'Manish Kumar',33,'Mason',1,'9876500022','9876501022','Yes','Yes',6,'Worker',1,1,1,0),(203,'Vivek Singh',27,'Welder',1,'9876500023','9876501023','Yes','Yes',6,'Worker',1,1,1,0),(204,'Pankaj Rawat',30,'Helper',1,'9876500024','9876501024','Yes','Yes',3,'Worker',1,1,1,0),(205,'Ankit Sharma',36,'Machine Operator',1,'9876500025','9876501025','Yes','Yes',4,'Worker',1,1,1,0),(206,'Ravi Negi',28,'Electrician',1,'9876500026','9876501026','No','Yes',5,'Worker',1,1,1,0),(207,'Harsh Gupta',25,'Helper',1,'9876500027','9876501027','Yes','Yes',3,'Worker',1,1,1,0),(208,'Gaurav Joshi',32,'Mason',1,'9876500028','9876501028','Yes','No',6,'Worker',1,1,1,0),(209,'Nitin Bisht',29,'Crane Operator',1,'9876500029','9876501029','Yes','Yes',3,'Contractor',1,1,1,0),(210,'Ajay Rawat',40,'Site Supervisor',1,'9876500030','9876501030','Yes','Yes',3,'Staff',1,1,1,0),(301,'Rohit Kumar',26,'Electrician',1,'9876500041','9876501041','Yes','Yes',5,'Worker',1,1,1,0),(302,'Suresh Singh',34,'Mason',1,'9876500042','9876501042','Yes','Yes',6,'Worker',1,1,1,0),(303,'Yash Negi',23,'Helper',1,'9876500043','9876501043','Yes','Yes',3,'Worker',1,1,1,0),(304,'Abhishek Rawat',31,'Welder',1,'9876500044','9876501044','Yes','Yes',6,'Worker',1,1,1,0),(305,'Varun Sharma',28,'Machine Operator',1,'9876500045','9876501045','Yes','Yes',4,'Worker',1,1,1,0),(306,'Rajat Joshi',37,'Electrician',1,'9876500046','9876501046','Yes','No',5,'Worker',1,1,1,0),(307,'Dev Singh',25,'Helper',1,'9876500047','9876501047','No','Yes',3,'Contractor',1,1,1,0),(308,'Mukul Bisht',39,'Site Supervisor',1,'9876500048','9876501048','Yes','Yes',3,'Staff',1,1,1,0);
/*!40000 ALTER TABLE `workers` ENABLE KEYS */;
UNLOCK TABLES;

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
-- Dumping data for table `zone_edges`
--

LOCK TABLES `zone_edges` WRITE;
/*!40000 ALTER TABLE `zone_edges` DISABLE KEYS */;
INSERT INTO `zone_edges` VALUES (1,2,200),(2,3,300),(2,7,150),(2,8,150),(3,4,250),(3,6,400),(4,5,300),(6,4,350),(7,1,250);
/*!40000 ALTER TABLE `zone_edges` ENABLE KEYS */;
UNLOCK TABLES;

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

--
-- Dumping data for table `zones`
--

LOCK TABLES `zones` WRITE;
/*!40000 ALTER TABLE `zones` DISABLE KEYS */;
INSERT INTO `zones` VALUES (1,1,'Main Gate',0),(2,1,'Admin Office',0),(3,1,'Material Yard',0),(4,1,'Excavation Zone',0),(5,1,'Tunnel Section',1),(6,1,'Bridge Pier A',0),(7,1,'Medical Camp',0),(8,1,'Fire Post',0);
/*!40000 ALTER TABLE `zones` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-10-17 19:17:49
