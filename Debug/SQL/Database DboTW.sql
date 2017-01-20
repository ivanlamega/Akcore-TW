/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50621
Source Host           : localhost:3306
Source Database       : twserver

Target Server Type    : MYSQL
Target Server Version : 50621
File Encoding         : 65001

Date: 2017-01-15 00:18:23
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `accounts`
-- ----------------------------
DROP TABLE IF EXISTS `accounts`;
CREATE TABLE `accounts` (
  `AccountID` int(10) NOT NULL AUTO_INCREMENT,
  `Username` varchar(20) DEFAULT NULL,
  `AccountPW` varchar(100) DEFAULT NULL,
  `acc_status` enum('block','active') DEFAULT 'active',
  `email` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`AccountID`)
) ENGINE=InnoDB AUTO_INCREMENT=32 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of accounts
-- ----------------------------
INSERT INTO `accounts` VALUES ('1', 'test', 'test', 'active', null);
INSERT INTO `accounts` VALUES ('5', 'bona', 'bona', 'active', null);
INSERT INTO `accounts` VALUES ('20', 'tok', 'tok', 'active', null);
INSERT INTO `accounts` VALUES ('21', 'GM', 'gm', 'active', null);
INSERT INTO `accounts` VALUES ('22', 'gm1', 'gm1', 'active', null);
INSERT INTO `accounts` VALUES ('23', 'gm2', 'gm2', 'active', null);
INSERT INTO `accounts` VALUES ('24', 'tes1', 'test1', 'active', null);
INSERT INTO `accounts` VALUES ('25', 'test2', 'test2', 'active', null);
INSERT INTO `accounts` VALUES ('26', 'test3', 'test3', 'active', null);
INSERT INTO `accounts` VALUES ('27', 'test4', 'test4', 'active', null);
INSERT INTO `accounts` VALUES ('28', 'test5', 'test5', 'active', null);
INSERT INTO `accounts` VALUES ('29', 'test6', 'test6', 'active', null);
INSERT INTO `accounts` VALUES ('30', 'test7', 'test7', 'active', null);
INSERT INTO `accounts` VALUES ('31', 'test8', 'test8', 'active', null);

-- ----------------------------
-- Table structure for `bannword`
-- ----------------------------
DROP TABLE IF EXISTS `bannword`;
CREATE TABLE `bannword` (
  `id` int(5) NOT NULL AUTO_INCREMENT,
  `bannword` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of bannword
-- ----------------------------

-- ----------------------------
-- Table structure for `bind`
-- ----------------------------
DROP TABLE IF EXISTS `bind`;
CREATE TABLE `bind` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `CharID` int(10) NOT NULL,
  `WorldID` tinyint(3) NOT NULL,
  `BindObjectTblIdx` int(10) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of bind
-- ----------------------------

-- ----------------------------
-- Table structure for `blacklist`
-- ----------------------------
DROP TABLE IF EXISTS `blacklist`;
CREATE TABLE `blacklist` (
  `pkID` int(11) NOT NULL AUTO_INCREMENT,
  `owner_id` int(10) NOT NULL,
  `target_id` int(10) NOT NULL,
  PRIMARY KEY (`pkID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of blacklist
-- ----------------------------

-- ----------------------------
-- Table structure for `buddylist`
-- ----------------------------
DROP TABLE IF EXISTS `buddylist`;
CREATE TABLE `buddylist` (
  `pkID` int(11) NOT NULL AUTO_INCREMENT,
  `owner_id` int(10) NOT NULL,
  `friend_id` int(10) NOT NULL,
  `moveBlackList` int(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`pkID`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of buddylist
-- ----------------------------

-- ----------------------------
-- Table structure for `characters`
-- ----------------------------
DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `CharID` int(10) NOT NULL AUTO_INCREMENT,
  `CharName` varchar(20) NOT NULL,
  `AccountID` int(10) DEFAULT NULL,
  `Level` tinyint(3) DEFAULT '3',
  `Exp` int(10) DEFAULT '0',
  `MaxExpInThisLevel` int(10) DEFAULT '200',
  `Race` tinyint(1) DEFAULT NULL,
  `Class` tinyint(2) DEFAULT NULL,
  `ChangeClass` bit(1) DEFAULT b'0',
  `Gender` tinyint(1) DEFAULT NULL,
  `Face` tinyint(2) DEFAULT NULL,
  `Adult` tinyint(1) unsigned DEFAULT '0',
  `Hair` tinyint(2) DEFAULT NULL,
  `HairColor` tinyint(2) DEFAULT NULL,
  `SkinColor` tinyint(2) DEFAULT NULL,
  `CurLocX` float(10,5) DEFAULT '4583.00000',
  `CurLocY` float(10,5) DEFAULT '0.00000',
  `CurLocZ` float(10,5) DEFAULT '4070.00000',
  `CurDirX` float(10,5) DEFAULT '0.00000',
  `CurDirY` float(10,5) DEFAULT '0.00000',
  `CurDirZ` float(10,5) DEFAULT '1.00000',
  `WorldTable` int(10) DEFAULT '1',
  `WorldID` int(10) DEFAULT '1',
  `Money` int(10) DEFAULT '0',
  `MoneyBank` int(10) DEFAULT '0',
  `MapInfoIndex` int(4) DEFAULT '1',
  `TutorialFlag` bit(1) DEFAULT b'1',
  `NameChange` bit(1) DEFAULT b'0',
  `Reputation` int(10) DEFAULT '0',
  `MudosaPoint` int(10) DEFAULT '0',
  `SpPoint` int(2) DEFAULT '0',
  `GameMaster` bit(1) DEFAULT b'0',
  `GuildID` int(4) DEFAULT '0',
  `GuildName` varchar(30) DEFAULT NULL,
  `BaseStr` int(3) DEFAULT '10',
  `LastStr` int(3) DEFAULT '10',
  `BaseCon` int(3) DEFAULT '10',
  `LastCon` int(3) DEFAULT '10',
  `BaseFoc` int(3) DEFAULT '10',
  `LastFoc` int(3) DEFAULT '10',
  `BaseDex` int(3) DEFAULT '10',
  `LastDex` int(3) DEFAULT '10',
  `BaseSol` int(3) DEFAULT '10',
  `LastSol` int(3) DEFAULT '10',
  `BaseEng` int(3) DEFAULT '10',
  `LastEng` int(3) DEFAULT '10',
  `BaseMaxLP` int(5) DEFAULT '100',
  `LastMaxLP` int(5) DEFAULT '100',
  `BaseMaxEP` int(5) DEFAULT '100',
  `LastMaxEP` int(5) DEFAULT '100',
  `BaseMaxRP` int(5) DEFAULT '100',
  `LastMaxRP` int(5) DEFAULT '100',
  `BaseLpRegen` int(5) DEFAULT '0',
  `LastLpRegen` int(5) DEFAULT '0',
  `BaseLpSitdownRegen` int(5) DEFAULT '0',
  `LastLpSitdownRegen` int(5) DEFAULT '0',
  `BaseLpBattleRegen` int(5) DEFAULT '0',
  `LastLpBattleRegen` int(5) DEFAULT '0',
  `BaseEpRegen` int(5) DEFAULT '0',
  `LastEpRegen` int(5) DEFAULT '0',
  `BaseEpSitdownRegen` int(5) DEFAULT '0',
  `LastEpSitdownRegen` int(5) DEFAULT '0',
  `BaseEpBattleRegen` int(5) DEFAULT '0',
  `LastEpBattleRegen` int(5) DEFAULT '0',
  `BaseRpRegen` int(5) DEFAULT '0',
  `LastRpRegen` int(5) DEFAULT '0',
  `LastRpDimimutionRate` int(5) DEFAULT '0',
  `BasePhysicalOffence` int(5) DEFAULT '10',
  `LastPhysicalOffence` int(5) DEFAULT '10',
  `BasePhysicalDefence` int(5) DEFAULT '10',
  `LastPhysicalDefence` int(5) DEFAULT '10',
  `BaseEnergyOffence` int(5) DEFAULT '10',
  `LastEnergyOffence` int(5) DEFAULT '10',
  `BaseEnergyDefence` int(5) DEFAULT '10',
  `LastEnergyDefence` int(5) DEFAULT '10',
  `BaseAttackRate` int(5) DEFAULT '2',
  `LastAttackRate` int(5) DEFAULT '2',
  `BaseDodgeRate` int(5) DEFAULT '2',
  `LastDodgeRate` int(5) DEFAULT '2',
  `BaseBlockRate` int(5) DEFAULT '2',
  `LastBlockRate` int(5) DEFAULT '2',
  `BaseCurseSuccessRate` int(5) DEFAULT '0',
  `LastCurseSuccessRate` int(5) DEFAULT '0',
  `BaseCurseToleranceRate` int(5) DEFAULT '0',
  `LastCurseToleranceRate` int(5) DEFAULT '0',
  `BasePhysicalCriticalRate` int(5) DEFAULT '0',
  `LastPhysicalCriticalRate` int(5) DEFAULT '0',
  `BaseEnergyCriticalRate` int(5) DEFAULT '0',
  `LastEnergyCriticalRate` int(5) DEFAULT '0',
  `LastRunSpeed` float(10,5) DEFAULT '8.00000',
  `BaseAttackSpeedRate` int(5) DEFAULT '1000',
  `LastAttackSpeedRate` int(5) DEFAULT '1000',
  `BaseAttackRange` float(10,5) DEFAULT '10.00000',
  `LastAttackRange` float(10,5) DEFAULT '10.00000',
  `CastingTimeChangePercent` float(10,5) DEFAULT '100.00000',
  `CoolTimeChangePercent` float(10,5) DEFAULT '100.00000',
  `KeepTimeChangePercent` float(10,5) DEFAULT '100.00000',
  `DotValueChangePercent` float(10,5) DEFAULT '100.00000',
  `DotTimeChangeAbsolute` float(10,5) DEFAULT '100.00000',
  `RequiredEpChangePercent` float(10,5) DEFAULT '100.00000',
  `HonestOffence` float(10,5) DEFAULT '0.00000',
  `HonestDefence` float(10,5) DEFAULT '0.00000',
  `StrangeOffence` float(10,5) DEFAULT '0.00000',
  `StrangeDefence` float(10,5) DEFAULT '0.00000',
  `WildOffence` float(10,5) DEFAULT '0.00000',
  `WildDefence` float(10,5) DEFAULT '0.00000',
  `EleganceOffence` float(10,5) DEFAULT '0.00000',
  `EleganceDefence` float(10,5) DEFAULT '0.00000',
  `FunnyOffence` float(10,5) DEFAULT '0.00000',
  `FunnyDefence` float(10,5) DEFAULT '0.00000',
  `ParalyzeToleranceRate` int(5) DEFAULT '0',
  `TerrorToleranceRate` int(5) DEFAULT '0',
  `ConfuseToleranceRate` int(5) DEFAULT '0',
  `StoneToleranceRate` int(5) DEFAULT '0',
  `CandyToleranceRate` int(5) DEFAULT '0',
  `ParalyzeKeepTimeDown` float(10,5) DEFAULT '0.00000',
  `TerrorKeepTimeDown` float(10,5) DEFAULT '0.00000',
  `ConfuseKeepTimeDown` float(10,5) DEFAULT '0.00000',
  `StoneKeepTimeDown` float(10,5) DEFAULT '0.00000',
  `CandyKeepTimeDown` float(10,5) DEFAULT '0.00000',
  `BleedingKeepTimeDown` float(10,5) DEFAULT '0.00000',
  `PoisonKeepTimeDown` float(10,5) DEFAULT '0.00000',
  `StomachacheKeepTimeDown` float(10,5) DEFAULT '0.00000',
  `CriticalBlockSuccessRate` float(10,5) DEFAULT '0.00000',
  `GuardRate` int(5) DEFAULT '0',
  `SkillDamageBlockModeSuccessRate` float(10,5) DEFAULT '0.00000',
  `CurseBlockModeSuccessRate` float(10,5) DEFAULT '0.00000',
  `KnockdownBlockModeSuccessRate` float(10,5) DEFAULT '0.00000',
  `HtbBlockModeSuccessRate` float(10,5) DEFAULT '0.00000',
  `SitDownLpRegenBonusRate` float(10,5) DEFAULT '0.00000',
  `SitDownEpRegenBonusRate` float(10,5) DEFAULT '0.00000',
  `PhysicalCriticalDamageBonusRate` float(10,5) DEFAULT '0.00000',
  `EnergyCriticalDamageBonusRate` float(10,5) DEFAULT '0.00000',
  `ItemUpgradeBonusRate` float(10,5) DEFAULT '0.00000',
  `ItemUpgradeBreakBonusRate` float(10,5) DEFAULT '0.00000',
  `CurAP` int(10) DEFAULT '450000' COMMENT 'FlyingAP',
  `CurLP` int(10) DEFAULT '100',
  `CurEP` int(10) DEFAULT '100',
  `CurRP` int(10) DEFAULT '100',
  `OnlineID` int(5) DEFAULT '0',
  `MailIsAway` int(10) DEFAULT NULL,
  `IsOnline` int(10) DEFAULT '0',
  `ServerID` int(10) DEFAULT '0',
  `Titulo` int(10) DEFAULT '215',
  `Token` int(10) DEFAULT '0',
  `MixExp` int(10) DEFAULT '0',
  `MixLevel` int(10) DEFAULT '0',
  `LastMaxAp` int(5) DEFAULT '450000',
  `BaseMaxAp` int(5) DEFAULT '450000',
  `tlqlocX` float(10,5) DEFAULT '0.00000',
  `tlqlocY` float(10,5) DEFAULT '0.00000',
  `tlqlocZ` float(10,5) DEFAULT '0.00000',
  `tlqdirX` float(10,5) DEFAULT '0.00000',
  `tlqdirY` float(10,5) DEFAULT '0.00000',
  `tlqdirZ` float(10,5) DEFAULT '0.00000',
  PRIMARY KEY (`CharID`)
) ENGINE=InnoDB AUTO_INCREMENT=26 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of characters
-- ----------------------------

-- ----------------------------
-- Table structure for `charquestlist`
-- ----------------------------
DROP TABLE IF EXISTS `charquestlist`;
CREATE TABLE `charquestlist` (
  `pkQtTable` int(11) NOT NULL AUTO_INCREMENT,
  `charId` int(10) NOT NULL,
  `questID` int(100) NOT NULL,
  `isCompleted` smallint(1) NOT NULL DEFAULT '0',
  `currentStep` int(255) NOT NULL DEFAULT '255',
  `type` smallint(1) NOT NULL,
  `dwEventData` int(255) NOT NULL,
  `nextStep` int(255) NOT NULL DEFAULT '2',
  `timeRemaing` int(15) NOT NULL DEFAULT '255',
  PRIMARY KEY (`pkQtTable`)
) ENGINE=InnoDB AUTO_INCREMENT=196 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of charquestlist
-- ----------------------------

-- ----------------------------
-- Table structure for `friendlist`
-- ----------------------------
DROP TABLE IF EXISTS `friendlist`;
CREATE TABLE `friendlist` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(10) NOT NULL,
  `friend_id` int(10) NOT NULL,
  `friend_name` varchar(20) NOT NULL,
  `blacklist` bit(1) NOT NULL DEFAULT b'0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of friendlist
-- ----------------------------

-- ----------------------------
-- Table structure for `guild_members`
-- ----------------------------
DROP TABLE IF EXISTS `guild_members`;
CREATE TABLE `guild_members` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `GuildID` int(5) NOT NULL,
  `MemberID` int(10) NOT NULL,
  `MemberName` varchar(30) NOT NULL,
  `is_guildmaster` tinyint(1) DEFAULT '0',
  `is_secondguildmaster` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of guild_members
-- ----------------------------

-- ----------------------------
-- Table structure for `guilds`
-- ----------------------------
DROP TABLE IF EXISTS `guilds`;
CREATE TABLE `guilds` (
  `GuildID` int(5) NOT NULL AUTO_INCREMENT,
  `GuildName` varchar(30) DEFAULT NULL,
  `GuildMasterName` varchar(30) DEFAULT NULL,
  `GuildMaster` int(10) DEFAULT NULL,
  `GuildSecondMaster` int(10) DEFAULT '0',
  `GuildNotice` varchar(256) DEFAULT NULL,
  `GuildReputation` int(10) DEFAULT '0',
  `GuildPointEver` int(10) DEFAULT '0',
  `GuildDisbandTime` datetime DEFAULT NULL,
  `GuildFunctions` int(15) DEFAULT '0',
  PRIMARY KEY (`GuildID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of guilds
-- ----------------------------

-- ----------------------------
-- Table structure for `items`
-- ----------------------------
DROP TABLE IF EXISTS `items`;
CREATE TABLE `items` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `tblidx` int(11) unsigned NOT NULL DEFAULT '0',
  `owner_id` int(11) unsigned NOT NULL DEFAULT '0',
  `place` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `pos` smallint(5) unsigned NOT NULL DEFAULT '0',
  `count` smallint(3) unsigned NOT NULL DEFAULT '1',
  `rank` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `durability` smallint(3) unsigned NOT NULL DEFAULT '100',
  `grade` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `battleAttribute` int(11) DEFAULT '0',
  `option0` int(11) DEFAULT '0',
  `option1` int(11) DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `owner_id_idx` (`owner_id`),
  KEY `item_vnum_index` (`tblidx`)
) ENGINE=MyISAM AUTO_INCREMENT=1400468 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of items
-- ----------------------------

-- ----------------------------
-- Table structure for `mail`
-- ----------------------------
DROP TABLE IF EXISTS `mail`;
CREATE TABLE `mail` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `CharID` int(11) DEFAULT NULL,
  `byDay` tinyint(2) DEFAULT NULL,
  `tCreateTime` int(50) NOT NULL,
  `SenderType` tinyint(1) NOT NULL DEFAULT '0',
  `byMailType` tinyint(1) NOT NULL DEFAULT '1',
  `byTextSize` int(10) DEFAULT NULL,
  `dwZenny` int(10) DEFAULT '0',
  `wszText` varchar(130) DEFAULT NULL,
  `item_id` int(11) DEFAULT NULL,
  `item_place` int(10) DEFAULT NULL,
  `item_pos` int(10) DEFAULT NULL,
  `wszTargetName` varchar(16) DEFAULT NULL,
  `wszFromName` varchar(16) DEFAULT NULL,
  `bIsAccept` bit(1) DEFAULT b'0',
  `bIsLock` bit(1) DEFAULT b'0',
  `bIsRead` bit(1) DEFAULT b'0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of mail
-- ----------------------------

-- ----------------------------
-- Table structure for `portals`
-- ----------------------------
DROP TABLE IF EXISTS `portals`;
CREATE TABLE `portals` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `CharID` int(10) DEFAULT NULL,
  `Point` smallint(3) DEFAULT NULL,
  `LocationX` float(10,5) DEFAULT NULL,
  `LocationY` float(10,5) DEFAULT NULL,
  `LocationZ` float(10,5) DEFAULT NULL,
  `DirectionX` float(10,5) DEFAULT NULL,
  `DirectionY` float(10,5) DEFAULT NULL,
  `DirectionZ` float(10,5) DEFAULT NULL,
  `WorldID` int(5) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of portals
-- ----------------------------

-- ----------------------------
-- Table structure for `quickslot`
-- ----------------------------
DROP TABLE IF EXISTS `quickslot`;
CREATE TABLE `quickslot` (
  `pkQuick` int(15) NOT NULL AUTO_INCREMENT,
  `charId` int(11) DEFAULT NULL,
  `slotId_0` int(10) unsigned DEFAULT '0',
  `slotId_1` int(10) unsigned DEFAULT '0',
  `slotId_2` int(10) unsigned DEFAULT '0',
  `slotId_3` int(10) unsigned DEFAULT '0',
  `slotId_4` int(10) unsigned DEFAULT '0',
  `slotId_5` int(10) unsigned DEFAULT '0',
  `slotId_6` int(10) unsigned DEFAULT '0',
  `slotId_7` int(10) unsigned DEFAULT '0',
  `slotId_8` int(10) unsigned DEFAULT '0',
  `slotId_9` int(10) unsigned DEFAULT '0',
  `slotId_10` int(10) unsigned DEFAULT '0',
  `slotId_11` int(10) unsigned DEFAULT '0',
  `slotId_12` int(10) unsigned DEFAULT '0',
  `slotId_13` int(10) unsigned DEFAULT '0',
  `slotId_14` int(10) unsigned DEFAULT '0',
  `slotId_15` int(10) unsigned DEFAULT '0',
  `slotId_16` int(10) unsigned DEFAULT '0',
  `slotId_17` int(10) unsigned DEFAULT '0',
  `slotId_18` int(10) unsigned DEFAULT '0',
  `slotId_19` int(10) unsigned DEFAULT '0',
  `slotId_20` int(10) unsigned DEFAULT '0',
  `slotId_21` int(10) unsigned DEFAULT '0',
  `slotId_22` int(10) unsigned DEFAULT '0',
  `slotId_23` int(10) unsigned DEFAULT '0',
  `slotId_24` int(10) unsigned DEFAULT '0',
  `slotId_25` int(10) unsigned DEFAULT '0',
  `slotId_26` int(10) unsigned DEFAULT '0',
  `slotId_27` int(10) unsigned DEFAULT '0',
  `slotId_28` int(10) unsigned DEFAULT '0',
  `slotId_29` int(10) unsigned DEFAULT '0',
  `slotId_30` int(10) unsigned DEFAULT '0',
  `slotId_31` int(10) unsigned DEFAULT '0',
  `slotId_32` int(10) unsigned DEFAULT '0',
  `slotId_33` int(10) unsigned DEFAULT '0',
  `slotId_34` int(10) unsigned DEFAULT '0',
  `slotId_35` int(10) unsigned DEFAULT '0',
  `slotId_36` int(10) unsigned DEFAULT '0',
  `slotId_37` int(10) unsigned DEFAULT '0',
  `slotId_38` int(10) unsigned DEFAULT '0',
  `slotId_39` int(10) unsigned DEFAULT '0',
  `slotId_40` int(10) unsigned DEFAULT '0',
  `slotId_41` int(10) unsigned DEFAULT '0',
  `slotId_42` int(10) unsigned DEFAULT '0',
  `slotId_43` int(10) unsigned DEFAULT '0',
  `slotId_44` int(10) unsigned DEFAULT '0',
  `slotId_45` int(10) unsigned DEFAULT '0',
  `slotId_46` int(10) unsigned DEFAULT '0',
  `slotId_47` int(10) DEFAULT NULL,
  `placeID` int(10) DEFAULT NULL,
  PRIMARY KEY (`pkQuick`)
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of quickslot
-- ----------------------------

-- ----------------------------
-- Table structure for `skills`
-- ----------------------------
DROP TABLE IF EXISTS `skills`;
CREATE TABLE `skills` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `skill_id` int(10) unsigned NOT NULL DEFAULT '0',
  `owner_id` int(11) unsigned NOT NULL,
  `RpBonusAuto` bit(1) DEFAULT b'0',
  `RpBonusType` int(5) unsigned NOT NULL DEFAULT '0',
  `SlotID` smallint(3) unsigned NOT NULL DEFAULT '0',
  `TimeRemaining` int(15) unsigned NOT NULL,
  `Exp` int(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=179 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of skills
-- ----------------------------

-- ----------------------------
-- Table structure for `warfoginfo`
-- ----------------------------
DROP TABLE IF EXISTS `warfoginfo`;
CREATE TABLE `warfoginfo` (
  `idFog` int(11) NOT NULL AUTO_INCREMENT,
  `hObject` int(11) NOT NULL,
  `owner_id` int(10) NOT NULL,
  PRIMARY KEY (`idFog`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of warfoginfo
-- ----------------------------

-- ----------------------------
-- Procedure structure for `AuthLogin`
-- ----------------------------
DROP PROCEDURE IF EXISTS `AuthLogin`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `AuthLogin`(IN m_szUserID VARCHAR(20), IN m_szUserPW VARCHAR(20), OUT m_dwAccountID INT, OUT m_nResultCode INT)
BEGIN

	DECLARE dec_pw VARCHAR(100);
	DECLARE dec_status VARCHAR(7);

		IF(SELECT EXISTS(SELECT 1 FROM accounts WHERE Username = m_szUserID)) THEN
				SELECT AccountID,AccountPW,acc_status INTO m_dwAccountID,dec_pw,dec_status
				FROM accounts
				WHERE Username = m_szUserID;

					IF (dec_pw = m_szUserPW) THEN
							SET m_nResultCode = 100;
					ELSE
							SET m_nResultCode = 107;
					END IF;
					IF (dec_status = 'block') THEN
							SET m_nResultCode = 113;
					END IF;

		ELSE
			SET m_nResultCode = 108;
		END IF;

END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `BuyItemFromShop`
-- ----------------------------
DROP PROCEDURE IF EXISTS `BuyItemFromShop`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `BuyItemFromShop`(IN tblidx INT, IN CharID INT, IN place INT, IN pos INT,IN rank INT,IN grade INT, IN durability INT,OUT unique_iID INT)
BEGIN



  INSERT INTO items (tblidx,owner_id,place,pos,rank,grade,durability)
  VALUES
  (tblidx ,CharID ,place ,pos ,rank ,grade ,durability);

  SET unique_iID = LAST_INSERT_ID();


END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `ChangeCharname`
-- ----------------------------
DROP PROCEDURE IF EXISTS `ChangeCharname`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `ChangeCharname`(IN awchCharName VARCHAR(16), IN char_Id INT, OUT wResultCode INT)
BEGIN

		IF(SELECT EXISTS(SELECT 1 FROM characters WHERE CharName = awchCharName)) THEN

			SET wResultCode = 205;

		ELSE

				IF(SELECT EXISTS(SELECT 1 FROM bannword WHERE bannword = c_guild_name)) THEN

					SET wResultCode = 205;

				ELSE

					UPDATE characters SET CharName = awchCharName, NameChange = 0 WHERE CharID = char_Id LIMIT 1;

					SET wResultCode = 200;

				END IF;

		END IF;


END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `ChangeGuildMaster`
-- ----------------------------
DROP PROCEDURE IF EXISTS `ChangeGuildMaster`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `ChangeGuildMaster`(IN characterID INT, IN guildname VARCHAR(30), IN mastercharID INT, OUT wResultCode INT)
BEGIN

DECLARE issecguildmaster INT;
DECLARE guild_id INT;
DECLARE char_name VARCHAR(30);

SET wResultCode = 301;

IF(SELECT EXISTS(SELECT 1 FROM guild_members WHERE MemberID = characterID))THEN

	SELECT CharName INTO char_name FROM characters WHERE CharID = characterID;
	SELECT GuildID,is_secondguildmaster INTO guild_id,issecguildmaster FROM guild_members WHERE MemberID = characterID;

		IF(issecguildmaster = 1) THEN
			UPDATE guild_members SET is_secondguildmaster = 0 , is_guildmaster = 1 WHERE MemberID = characterID AND GuildID = guild_id;
			UPDATE guild_members SET is_guildmaster = 0 WHERE GuildID = guild_id AND MemberID = mastercharID;
			UPDATE guilds SET GuildMasterName = char_name , GuildMaster = characterID, GuildSecondMaster = 0 WHERE GuildID = guild_id;
			SET wResultCode = 300;

		ELSE

			UPDATE guild_members SET is_guildmaster = 0 WHERE GuildID = guild_id AND MemberID = mastercharID;
			UPDATE guilds SET GuildMasterName = char_name , GuildMaster = characterID WHERE GuildID = guild_id;
			UPDATE guild_members SET is_guildmaster = 1 WHERE MemberID = characterID AND GuildID = guild_id;
			SET wResultCode = 300;
		END IF;

ELSE

	SET wResultCode = 301;

END IF;

END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `CharBind`
-- ----------------------------
DROP PROCEDURE IF EXISTS `CharBind`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CharBind`(IN characterID INT, IN bindObjectTblidx INT, OUT currentWorldID INT)
BEGIN

DECLARE currentWorldID INT;

	SELECT WorldID INTO currentWorldID FROM characters WHERE CharID = characterID LIMIT 1;

		IF(SELECT EXISTS(SELECT 1 FROM bind WHERE CharID = characterID)) THEN

				UPDATE bind SET WorldID = currentWorldID, BindObjectTblIdx = bindObjectTblidx WHERE CharID = characterID LIMIT 1;

		ELSE

				INSERT INTO bind (CharID,WorldID,BindObjectTblIdx)
				VALUES
				(characterID,currentWorldID,bindObjectTblidx);

		END IF;

END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `CharCreate`
-- ----------------------------
DROP PROCEDURE IF EXISTS `CharCreate`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CharCreate`(IN awchCharName VARCHAR(16), IN byRace INT, IN byClass INT, IN byGender INT, IN byFace INT, IN byHair INT, IN byHairColor INT, IN bySkinColor INT, IN account_id INT,IN ServerID INT, OUT char_id INT, OUT wResultCode INT)
BEGIN
		IF(SELECT EXISTS(SELECT 1 FROM characters WHERE CharName = awchCharName)) THEN
			SET wResultCode = 205;
		ELSE
				IF(SELECT EXISTS(SELECT 1 FROM bannword WHERE bannword = awchCharName)) THEN
					SET wResultCode = 205;
				ELSE
					INSERT INTO characters (CharName,AccountID,Race,Class,Gender,Face,Hair,HairColor,SkinColor, Money,ServerID)
					VALUES
					(awchCharName,account_id,byRace,byClass,byGender,byFace,byHair,byHairColor,bySkinColor, 10000,ServerID);
					SET char_id = LAST_INSERT_ID();
					INSERT INTO quickslot(`charId`)
					VALUES(char_id);
					INSERT INTO items (`owner_id`, `tblidx`, `place`, `pos`, `count`, `rank`, `durability`)
					VALUES(char_id, 19901, 0, 0, 0, 1, 10),
						    (char_id, 532001, 7, 5, 1, 1, 100);
					IF (byRace = 0) THEN
						IF (byClass = 0) THEN
							INSERT INTO skills (`skill_id`, `owner_id`, RpBonusAuto, `RpBonusType`, `SlotID`, `TimeRemaining`, `Exp`)
							VALUES (10111,char_id,0,0,1,0,0);
							INSERT INTO items (`owner_id`, `tblidx`, `place`, `pos`, `count`, `rank`, `durability`)
							VALUES (char_id,  500001, 7, 0, 1, 1, 100),
									(char_id,  522001, 7, 2, 1, 1, 100),
							    (char_id,  522002, 7, 3, 1, 1, 100),									
						    	(char_id,  522003, 7, 4, 1, 1, 100);
						END IF;
						IF (byClass = 1) THEN
						INSERT INTO items (`owner_id`, `tblidx`, `place`, `pos`, `count`, `rank`, `durability`)
							VALUES (char_id, 500001, 7, 0, 1, 1, 100),
									(char_id,  523001, 7, 2, 1, 1, 100),
							    (char_id,  523002, 7, 3, 1, 1, 100),
						    	(char_id,  523003, 7, 4, 1, 1, 100);
						INSERT INTO skills (`skill_id`, `owner_id`, `SlotID`)
							VALUES(110111,char_id,1);
						END IF;
						UPDATE characters SET CurLocX = 4467, CurLocY = -40, CurLocZ = 3951 WHERE CharID = char_id LIMIT 1;
					END IF;
					IF (byRace = 1) THEN
					UPDATE characters SET CurLocX = 3131, CurLocY = -32, CurLocZ = -2755 WHERE CharID = char_id LIMIT 1;
						IF (byClass = 3) THEN
							INSERT INTO skills (`skill_id`, `owner_id`, RpBonusAuto, `RpBonusType`, `SlotID`, `TimeRemaining`, `Exp`)
							VALUES (310111,char_id,0,0,1,0,0);
							INSERT INTO items (`owner_id`, `tblidx`, `place`, `pos`, `count`, `rank`, `durability`)
							VALUES (char_id,  500001, 7, 0, 1, 1, 100),
								(char_id,  525001, 7, 2, 1, 1, 100),
								(char_id,  525002, 7, 3, 1, 1, 100),
						    (char_id,  525003, 7, 4, 1, 1, 100);
							END IF;
						IF (byClass = 4) THEN
							INSERT INTO items (`owner_id`, `tblidx`, `place`, `pos`, `count`, `rank`, `durability`)
							VALUES(char_id,  506001, 7, 0, 1, 1, 100),
									(char_id,  526001, 7, 2, 1, 1, 100),
							    (char_id,  526002, 7, 3, 1, 1, 100),
						    	(char_id,  526003, 7, 4, 1, 1, 100);
							INSERT INTO skills (`skill_id`, `owner_id`, `SlotID`)
							VALUES(410111,char_id,1);
						END IF;
					END IF;
					IF (byRace = 2) THEN
						IF (byClass = 5) THEN
							INSERT INTO skills (`skill_id`, `owner_id`, RpBonusAuto, `RpBonusType`, `SlotID`, `TimeRemaining`, `Exp`)
							VALUES (510111,char_id,0,0,1,0,0);
							INSERT INTO items (`owner_id`, `tblidx`, `place`, `pos`, `count`, `rank`, `durability`)
							VALUES(char_id,  500001, 7, 0, 1, 1, 100),
									(char_id,  527001, 7, 2, 1, 1, 100),
							    (char_id,  527002, 7, 3, 1, 1, 100),
						    	(char_id,  527003, 7, 4, 1, 1, 100);
						END IF;
						IF (byClass = 6) THEN
							INSERT INTO skills (`skill_id`, `owner_id`, RpBonusAuto, `RpBonusType`, `SlotID`, `TimeRemaining`, `Exp`)
							VALUES (610111,char_id,0,0,1,0,0);
							INSERT INTO items (`owner_id`, `tblidx`, `place`, `pos`, `count`, `rank`, `durability`)
							VALUES(char_id,  500001, 7, 0, 1, 1, 100),
									(char_id,  527001, 7, 2, 1, 1, 100),
							    (char_id,  527002, 7, 3, 1, 1, 100),
						    	(char_id,  527003, 7, 4, 1, 1, 100);
						END IF;
					UPDATE characters SET CurLocX = 5775, CurLocY = -74, CurLocZ = -4032 WHERE CharID = char_id LIMIT 1;
					END IF;
				SET wResultCode = 200;
				END IF;
		END IF;
END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `CreateItem`
-- ----------------------------
DROP PROCEDURE IF EXISTS `CreateItem`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `CreateItem`(IN tblidx INT, IN CharID INT, IN pos INT,IN Place INT, OUT unique_iID INT)
BEGIN



  INSERT INTO items (tblidx,owner_id,place,pos)
  VALUES
  (tblidx ,CharID ,Place,pos);

  SET unique_iID = LAST_INSERT_ID();


END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `DisbandGuild`
-- ----------------------------
DROP PROCEDURE IF EXISTS `DisbandGuild`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `DisbandGuild`(IN guild_name VARCHAR(30), IN characterID INT, OUT wResultCode INT)
BEGIN

	DECLARE guild_ID INT;

	SELECT GuildID INTO guild_ID FROM guilds WHERE GuildMaster = characterID;

	IF(guild_ID IS NULL OR guild_ID = '') THEN

		SET wResultCode = 319;

	ELSE

		DELETE FROM guilds WHERE GuildMaster = characterID;
		DELETE FROM guild_members WHERE GuildID = guild_ID;
		UPDATE characters SET GuildID = 0 , GuildName = NULL WHERE GuildID = guild_ID;

		SET wResultCode = 320;

	END IF;



END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GuildCreate`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GuildCreate`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GuildCreate`(IN c_guild_name VARCHAR(30), IN characterID INT, OUT wResultCode INT, OUT cguildid INT, OUT charactername VARCHAR(30))
BEGIN


DECLARE currentguild VARCHAR(30);

SELECT CharName,GuildName INTO charactername,currentguild FROM characters WHERE CharID = characterID;

/*SET GAME FAIL AS STANDARD*/
SET wResultCode = 201;

/*Check if already inside guild or already has a guild*/


IF(currentguild IS NULL OR currentguild = '') THEN

/*Check if guild name already exist*/
	IF(SELECT EXISTS(SELECT 1 FROM guilds WHERE GuildName = c_guild_name)) THEN
			/*GUILD ALREADY EXIST*/
			SET wResultCode = 2106;

		ELSE

			IF NOT(c_guild_name REGEXP '[A-Za-z0-9]') THEN
				/*BAD LETTERS IN NAME*/
				SET wResultCode = 2105;

			ELSE
				/*SELECT NAMES FROM BANNWORD TABLE*/
				IF(SELECT EXISTS(SELECT 1 FROM bannword WHERE bannword = c_guild_name)) THEN
					/*BAD GUILD NAME*/
					SET wResultCode = 2106;

				ELSE
					/*CREATE GUILD SUCCESS*/

					INSERT INTO guilds (GuildName,GuildMasterName,GuildMaster)
					VALUES
					(c_guild_name,charactername,characterID);

					SET wResultCode = 200;
					SET cguildid = LAST_INSERT_ID();

					UPDATE characters SET GuildID = cguildid, GuildName = c_guild_name WHERE CharID = characterID;

					INSERT INTO guild_members (GuildID,MemberID,MemberName,is_guildmaster)
					VALUES
					(cguildid,characterID,charactername,'1');

				END IF;

			END IF;

	END IF;

ELSE

SET wResultCode = 327;

END IF;

END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GuildInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GuildInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GuildInfo`(IN characterID INT, OUT guild_id INT, OUT guild_name VARCHAR(30), OUT guild_master INT, OUT guild_notice VARCHAR(256), OUT guild_rep INT, OUT second_guild_master INT, OUT guild_functions INT)
BEGIN

DECLARE current_guild INT;

	IF(SELECT EXISTS(SELECT 1 FROM guild_members WHERE MemberID = characterID)) THEN

		SELECT GuildID INTO current_guild FROM guild_members WHERE MemberID = characterID;


		SELECT GuildID,GuildName,GuildMaster,GuildNotice,GuildReputation,GuildSecondMaster,GuildFunctions
		INTO guild_id,guild_name,guild_master,guild_notice,guild_rep,second_guild_master,guild_functions
		FROM guilds
		WHERE GuildID = current_guild;


	END IF;

END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `KickGuild`
-- ----------------------------
DROP PROCEDURE IF EXISTS `KickGuild`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `KickGuild`(IN characterID INT, OUT wResultCode INT)
BEGIN

DECLARE isgmaster INT;


IF(SELECT EXISTS(SELECT 1 FROM guild_members WHERE MemberID = characterID))THEN

	SELECT is_guildmaster INTO isgmaster FROM guild_members WHERE MemberID = characterID;

	IF(isgmaster = 1) THEN

		SET wResultCode = 331;

	ELSE

		UPDATE characters SET GuildID = 0 , GuildName = NULL WHERE CharID = characterID LIMIT 1;
		DELETE FROM guild_members WHERE MemberID = characterID;

		SET wResultCode = 300;

	END IF;

ELSE

	SET wResultCode = 318;

END IF;

END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `LeaveGuild`
-- ----------------------------
DROP PROCEDURE IF EXISTS `LeaveGuild`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `LeaveGuild`(IN characterID INT, OUT wResultCode INT)
BEGIN

DECLARE isgmaster INT;
DECLARE issecgmaster INT;

/*SET AS STANDARD THAT HE IS NOT INSIDE A GUILD*/
SET wResultCode = 318;

/*CHECK IF GUILD LEADER*/
IF(SELECT EXISTS(SELECT 1 FROM guild_members WHERE MemberID = characterID) )THEN

	SELECT is_guildmaster,is_secondguildmaster INTO isgmaster,issecgmaster FROM guild_members WHERE MemberID = characterID;

		IF(isgmaster = 1) THEN
			SET wResultCode = 329;
		END IF;

		IF(issecgmaster = 1) THEN
			DELETE FROM guild_members WHERE MemberID = characterID;
			UPDATE guilds SET GuildSecondMaster = 0 WHERE GuildSecondMaster = characterID LIMIT 1;
			UPDATE characters SET GuildID = 0 , GuildName = NULL WHERE CharID = characterID LIMIT 1;
			SET wResultCode = 348;
		END IF;

		IF(isgmaster = 0 AND issecgmaster = 0) THEN
			DELETE FROM guild_members WHERE MemberID = characterID;
			UPDATE guilds SET GuildSecondMaster = 0 WHERE GuildSecondMaster = characterID LIMIT 1;
			UPDATE characters SET GuildID = 0 , GuildName = NULL WHERE CharID = characterID LIMIT 1;
			SET wResultCode = 348;
		END IF;


ELSE

	SET wResultCode = 318;

END IF;

END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `NewSecondGuildMaster`
-- ----------------------------
DROP PROCEDURE IF EXISTS `NewSecondGuildMaster`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `NewSecondGuildMaster`(IN characterID INT, IN guild_name VARCHAR(30), OUT wResultCode INT)
BEGIN

IF(SELECT EXISTS(SELECT 1 FROM guild_members WHERE MemberID = characterID) )THEN

	UPDATE guilds SET GuildSecondMaster = characterID WHERE GuildName = guild_name;
	UPDATE guild_members SET is_secondguildmaster = 1 WHERE MemberID = characterID;

	SET wResultCode = 300;

ELSE

	SET wResultCode = 301;

END IF;

END;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `UpdateGuildNotice`
-- ----------------------------
DROP PROCEDURE IF EXISTS `UpdateGuildNotice`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `UpdateGuildNotice`(IN characterID INT,  IN awchNotice VARCHAR(256), OUT wResultCode INT)
BEGIN

	IF(SELECT EXISTS(SELECT 1 FROM guilds WHERE GuildMaster = characterID OR GuildSecondMaster = characterID)) THEN

		UPDATE guilds
		SET GuildNotice = awchNotice
		WHERE GuildMaster = characterID OR GuildSecondMaster = characterID LIMIT 1;

		SET wResultCode = 300;

	ELSE

		SET wResultCode = 319;

	END IF;

END;;
DELIMITER ;
