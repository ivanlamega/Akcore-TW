-- phpMyAdmin SQL Dump
-- version 4.7.0
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Sep 08, 2019 at 12:47 AM
-- Server version: 10.1.25-MariaDB
-- PHP Version: 7.1.7

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `dbodev`
--

DELIMITER $$
--
-- Procedures
--
CREATE DEFINER=`root`@`localhost` PROCEDURE `AuthLogin` (IN `m_szUserID` VARCHAR(20), IN `m_szUserPW` VARCHAR(20), OUT `m_dwAccountID` INT, OUT `m_nResultCode` INT)  BEGIN

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

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `BuyItemFromShop` (IN `tblidx` INT, IN `CharID` INT, IN `place` INT, IN `pos` INT, IN `rank` INT, IN `grade` INT, IN `durability` INT, OUT `unique_iID` INT)  BEGIN



  INSERT INTO items (tblidx,owner_id,place,pos,rank,grade,durability)
  VALUES
  (tblidx ,CharID ,place ,pos ,rank ,grade ,durability);

  SET unique_iID = LAST_INSERT_ID();


END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `ChangeCharname` (IN `awchCharName` VARCHAR(16), IN `char_Id` INT, OUT `wResultCode` INT)  BEGIN

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


END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `ChangeGuildMaster` (IN `characterID` INT, IN `guildname` VARCHAR(30), IN `mastercharID` INT, OUT `wResultCode` INT)  BEGIN

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

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `CharBind` (IN `characterID` INT, IN `bindObjectTblidx` INT, OUT `currentWorldID` INT)  BEGIN

DECLARE currentWorldID INT;

	SELECT WorldID INTO currentWorldID FROM characters WHERE CharID = characterID LIMIT 1;

		IF(SELECT EXISTS(SELECT 1 FROM bind WHERE CharID = characterID)) THEN

				UPDATE bind SET WorldID = currentWorldID, BindObjectTblIdx = bindObjectTblidx WHERE CharID = characterID LIMIT 1;

		ELSE

				INSERT INTO bind (CharID,WorldID,BindObjectTblIdx)
				VALUES
				(characterID,currentWorldID,bindObjectTblidx);

		END IF;

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `CharCreate` (IN `awchCharName` VARCHAR(16), IN `byRace` INT, IN `byClass` INT, IN `byGender` INT, IN `byFace` INT, IN `byHair` INT, IN `byHairColor` INT, IN `bySkinColor` INT, IN `account_id` INT, IN `ServerID` INT, OUT `char_id` INT, OUT `wResultCode` INT)  BEGIN
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
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `CreateItem` (IN `tblidx` INT, IN `CharID` INT, IN `pos` INT, IN `Place` INT, OUT `unique_iID` INT)  BEGIN



  INSERT INTO items (tblidx,owner_id,place,pos)
  VALUES
  (tblidx ,CharID ,Place,pos);

  SET unique_iID = LAST_INSERT_ID();


END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `DisbandGuild` (IN `guild_name` VARCHAR(30), IN `characterID` INT, OUT `wResultCode` INT)  BEGIN

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



END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `GuildCreate` (IN `c_guild_name` VARCHAR(30), IN `characterID` INT, OUT `wResultCode` INT, OUT `cguildid` INT, OUT `charactername` VARCHAR(30))  BEGIN


DECLARE currentguild VARCHAR(30);

SELECT CharName,GuildName INTO charactername,currentguild FROM characters WHERE CharID = characterID;


SET wResultCode = 201;




IF(currentguild IS NULL OR currentguild = '') THEN


	IF(SELECT EXISTS(SELECT 1 FROM guilds WHERE GuildName = c_guild_name)) THEN
			
			SET wResultCode = 2106;

		ELSE

			IF NOT(c_guild_name REGEXP '[A-Za-z0-9]') THEN
				
				SET wResultCode = 2105;

			ELSE
				
				IF(SELECT EXISTS(SELECT 1 FROM bannword WHERE bannword = c_guild_name)) THEN
					
					SET wResultCode = 2106;

				ELSE
					

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

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `GuildInfo` (IN `characterID` INT, OUT `guild_id` INT, OUT `guild_name` VARCHAR(30), OUT `guild_master` INT, OUT `guild_notice` VARCHAR(256), OUT `guild_rep` INT, OUT `second_guild_master` INT, OUT `guild_functions` INT)  BEGIN

DECLARE current_guild INT;

	IF(SELECT EXISTS(SELECT 1 FROM guild_members WHERE MemberID = characterID)) THEN

		SELECT GuildID INTO current_guild FROM guild_members WHERE MemberID = characterID;


		SELECT GuildID,GuildName,GuildMaster,GuildNotice,GuildReputation,GuildSecondMaster,GuildFunctions
		INTO guild_id,guild_name,guild_master,guild_notice,guild_rep,second_guild_master,guild_functions
		FROM guilds
		WHERE GuildID = current_guild;


	END IF;

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `KickGuild` (IN `characterID` INT, OUT `wResultCode` INT)  BEGIN

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

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `LeaveGuild` (IN `characterID` INT, OUT `wResultCode` INT)  BEGIN

DECLARE isgmaster INT;
DECLARE issecgmaster INT;


SET wResultCode = 318;


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

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `NewSecondGuildMaster` (IN `characterID` INT, IN `guild_name` VARCHAR(30), OUT `wResultCode` INT)  BEGIN

IF(SELECT EXISTS(SELECT 1 FROM guild_members WHERE MemberID = characterID) )THEN

	UPDATE guilds SET GuildSecondMaster = characterID WHERE GuildName = guild_name;
	UPDATE guild_members SET is_secondguildmaster = 1 WHERE MemberID = characterID;

	SET wResultCode = 300;

ELSE

	SET wResultCode = 301;

END IF;

END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `UpdateGuildNotice` (IN `characterID` INT, IN `awchNotice` VARCHAR(256), OUT `wResultCode` INT)  BEGIN

	IF(SELECT EXISTS(SELECT 1 FROM guilds WHERE GuildMaster = characterID OR GuildSecondMaster = characterID)) THEN

		UPDATE guilds
		SET GuildNotice = awchNotice
		WHERE GuildMaster = characterID OR GuildSecondMaster = characterID LIMIT 1;

		SET wResultCode = 300;

	ELSE

		SET wResultCode = 319;

	END IF;

END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `accounts`
--

CREATE TABLE `accounts` (
  `AccountID` int(10) NOT NULL,
  `Username` varchar(20) DEFAULT NULL,
  `AccountPW` varchar(100) DEFAULT NULL,
  `acc_status` enum('block','active') DEFAULT 'active',
  `email` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `accounts`
--

INSERT INTO `accounts` (`AccountID`, `Username`, `AccountPW`, `acc_status`, `email`) VALUES
(1, 'test', 'test', 'active', NULL),
(2, 'Hero', '123', 'active', NULL),
(5, 'bona', 'bona', 'active', NULL),
(20, 'tok', 'tok', 'active', NULL),
(21, 'GM', 'gm', 'active', NULL),
(22, 'gm1', 'gm1', 'active', NULL),
(23, 'gm2', 'gm2', 'active', NULL),
(24, 'tes1', 'test1', 'active', NULL),
(25, 'test2', 'test2', 'active', NULL),
(26, 'test3', 'test3', 'active', NULL),
(27, 'test4', 'test4', 'active', NULL),
(28, 'test5', 'test5', 'active', NULL),
(29, 'test6', 'test6', 'active', NULL),
(30, 'test7', 'test7', 'active', NULL),
(31, 'test8', 'test8', 'active', NULL);

-- --------------------------------------------------------

--
-- Table structure for table `bannword`
--

CREATE TABLE `bannword` (
  `id` int(5) NOT NULL,
  `bannword` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `bind`
--

CREATE TABLE `bind` (
  `id` int(10) NOT NULL,
  `CharID` int(10) NOT NULL,
  `WorldID` tinyint(3) NOT NULL,
  `BindObjectTblIdx` int(10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `blacklist`
--

CREATE TABLE `blacklist` (
  `pkID` int(11) NOT NULL,
  `owner_id` int(10) NOT NULL,
  `target_id` int(10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `buddylist`
--

CREATE TABLE `buddylist` (
  `pkID` int(11) NOT NULL,
  `owner_id` int(10) NOT NULL,
  `friend_id` int(10) NOT NULL,
  `moveBlackList` int(1) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `characters`
--

CREATE TABLE `characters` (
  `CharID` int(10) NOT NULL,
  `CharName` varchar(20) NOT NULL,
  `AccountID` int(10) DEFAULT NULL,
  `Level` tinyint(3) DEFAULT '1',
  `Exp` int(10) DEFAULT '0',
  `MaxExpInThisLevel` int(10) DEFAULT '200',
  `Race` tinyint(1) DEFAULT NULL,
  `Class` tinyint(2) DEFAULT NULL,
  `ChangeClass` tinyint(1) DEFAULT '0',
  `Gender` tinyint(1) DEFAULT NULL,
  `Face` tinyint(2) DEFAULT NULL,
  `Adult` tinyint(1) UNSIGNED DEFAULT '0',
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
  `TutorialFlag` tinyint(1) DEFAULT '1',
  `NameChange` tinyint(1) DEFAULT '0',
  `Reputation` int(10) DEFAULT '0',
  `MudosaPoint` int(10) DEFAULT '0',
  `SpPoint` int(2) DEFAULT '0',
  `GameMaster` tinyint(1) DEFAULT '0',
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
  `Titulo` int(10) DEFAULT '0',
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
  `tlqdirZ` float(10,5) DEFAULT '0.00000'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `charquestlist`
--

CREATE TABLE `charquestlist` (
  `pkQtTable` int(11) NOT NULL,
  `charId` int(10) NOT NULL,
  `questID` int(100) NOT NULL,
  `isCompleted` smallint(1) NOT NULL DEFAULT '0',
  `currentStep` int(255) NOT NULL DEFAULT '255',
  `type` smallint(1) NOT NULL,
  `dwEventData` int(255) NOT NULL,
  `nextStep` int(255) NOT NULL DEFAULT '2',
  `timeRemaing` int(15) NOT NULL DEFAULT '255'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `friendlist`
--

CREATE TABLE `friendlist` (
  `id` int(11) NOT NULL,
  `user_id` int(10) NOT NULL,
  `friend_id` int(10) NOT NULL,
  `friend_name` varchar(20) NOT NULL,
  `blacklist` bit(1) NOT NULL DEFAULT b'0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `guilds`
--

CREATE TABLE `guilds` (
  `GuildID` int(5) NOT NULL,
  `GuildName` varchar(30) DEFAULT NULL,
  `GuildMasterName` varchar(30) DEFAULT NULL,
  `GuildMaster` int(10) DEFAULT NULL,
  `GuildSecondMaster` int(10) DEFAULT '0',
  `GuildNotice` varchar(256) DEFAULT NULL,
  `GuildReputation` int(10) DEFAULT '0',
  `GuildPointEver` int(10) DEFAULT '0',
  `GuildDisbandTime` datetime DEFAULT NULL,
  `GuildFunctions` int(15) DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `guild_members`
--

CREATE TABLE `guild_members` (
  `id` int(10) NOT NULL,
  `GuildID` int(5) NOT NULL,
  `MemberID` int(10) NOT NULL,
  `MemberName` varchar(30) NOT NULL,
  `is_guildmaster` tinyint(1) DEFAULT '0',
  `is_secondguildmaster` tinyint(1) DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `items`
--

CREATE TABLE `items` (
  `id` int(11) UNSIGNED NOT NULL,
  `tblidx` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `owner_id` int(11) UNSIGNED NOT NULL DEFAULT '0',
  `place` tinyint(1) UNSIGNED NOT NULL DEFAULT '1',
  `pos` smallint(5) UNSIGNED NOT NULL DEFAULT '0',
  `count` smallint(3) UNSIGNED NOT NULL DEFAULT '1',
  `rank` tinyint(1) UNSIGNED NOT NULL DEFAULT '1',
  `durability` smallint(3) UNSIGNED NOT NULL DEFAULT '100',
  `grade` tinyint(1) UNSIGNED NOT NULL DEFAULT '0',
  `battleAttribute` int(11) DEFAULT '0',
  `option0` int(11) DEFAULT '0',
  `option1` int(11) DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `items`
--

INSERT INTO `items` (`id`, `tblidx`, `owner_id`, `place`, `pos`, `count`, `rank`, `durability`, `grade`, `battleAttribute`, `option0`, `option1`) VALUES
(1400468, 19901, 26, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400469, 532001, 26, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400470, 500001, 26, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400471, 522001, 26, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400472, 522002, 26, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400473, 522003, 26, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400474, 19901, 27, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400475, 532001, 27, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400476, 500001, 27, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400477, 522001, 27, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400478, 522002, 27, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400479, 522003, 27, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400480, 19901, 28, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400481, 532001, 28, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400482, 500001, 28, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400483, 522001, 28, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400484, 522002, 28, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400485, 522003, 28, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400486, 19901, 29, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400487, 532001, 29, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400488, 500001, 29, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400489, 522001, 29, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400490, 522002, 29, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400491, 522003, 29, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400492, 19901, 30, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400493, 532001, 30, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400494, 500001, 30, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400495, 522001, 30, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400496, 522002, 30, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400497, 522003, 30, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400498, 19901, 31, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400499, 532001, 31, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400500, 500001, 31, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400501, 522001, 31, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400502, 522002, 31, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400503, 522003, 31, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400504, 19901, 32, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400505, 532001, 32, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400506, 500001, 32, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400507, 522001, 32, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400508, 522002, 32, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400509, 522003, 32, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400510, 19901, 33, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400511, 532001, 33, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400512, 500001, 33, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400513, 522001, 33, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400514, 522002, 33, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400515, 522003, 33, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400516, 19901, 34, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400517, 532001, 34, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400518, 500001, 34, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400519, 522001, 34, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400520, 522002, 34, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400521, 522003, 34, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400522, 19901, 35, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400523, 532001, 35, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400524, 500001, 35, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400525, 522001, 35, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400526, 522002, 35, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400527, 522003, 35, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400528, 19901, 36, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400529, 532001, 36, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400530, 500001, 36, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400531, 522001, 36, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400532, 522002, 36, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400533, 522003, 36, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400534, 19901, 37, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400535, 532001, 37, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400536, 500001, 37, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400537, 522001, 37, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400538, 522002, 37, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400539, 522003, 37, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400540, 19901, 38, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400541, 532001, 38, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400542, 500001, 38, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400543, 522001, 38, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400544, 522002, 38, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400545, 522003, 38, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400546, 19901, 39, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400547, 532001, 39, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400548, 500001, 39, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400549, 522001, 39, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400550, 522002, 39, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400551, 522003, 39, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400552, 19901, 40, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400553, 532001, 40, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400554, 500001, 40, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400555, 522001, 40, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400556, 522002, 40, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400557, 522003, 40, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400558, 19901, 41, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400559, 532001, 41, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400560, 500001, 41, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400561, 522001, 41, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400562, 522002, 41, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400563, 522003, 41, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400564, 19901, 42, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400565, 532001, 42, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400566, 500001, 42, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400567, 522001, 42, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400568, 522002, 42, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400569, 522003, 42, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400570, 19901, 43, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400571, 532001, 43, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400572, 500001, 43, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400573, 522001, 43, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400574, 522002, 43, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400575, 522003, 43, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400576, 19901, 44, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400577, 532001, 44, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400578, 500001, 44, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400579, 522001, 44, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400580, 522002, 44, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400581, 522003, 44, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400582, 19901, 45, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400583, 532001, 45, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400584, 500001, 45, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400585, 522001, 45, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400586, 522002, 45, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400587, 522003, 45, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400588, 19901, 46, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400589, 532001, 46, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400590, 500001, 46, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400591, 522001, 46, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400592, 522002, 46, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400593, 522003, 46, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400594, 19901, 47, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400595, 532001, 47, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400596, 500001, 47, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400597, 522001, 47, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400598, 522002, 47, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400599, 522003, 47, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400600, 19901, 48, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400601, 532001, 48, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400602, 500001, 48, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400603, 522001, 48, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400604, 522002, 48, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400605, 522003, 48, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400606, 19901, 49, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400607, 532001, 49, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400608, 500001, 49, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400609, 522001, 49, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400610, 522002, 49, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400611, 522003, 49, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400612, 19901, 50, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400613, 532001, 50, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400614, 500001, 50, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400615, 522001, 50, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400616, 522002, 50, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400617, 522003, 50, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400618, 19901, 51, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400619, 532001, 51, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400620, 500001, 51, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400621, 522001, 51, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400622, 522002, 51, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400623, 522003, 51, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400624, 19901, 52, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400625, 532001, 52, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400626, 500001, 52, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400627, 522001, 52, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400628, 522002, 52, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400629, 522003, 52, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400630, 19901, 53, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400631, 532001, 53, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400632, 500001, 53, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400633, 522001, 53, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400634, 522002, 53, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400635, 522003, 53, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400636, 19901, 54, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400637, 532001, 54, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400638, 500001, 54, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400639, 522001, 54, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400640, 522002, 54, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400641, 522003, 54, 7, 4, 1, 1, 100, 0, 0, 0, 0),
(1400642, 19901, 55, 0, 0, 0, 1, 10, 0, 0, 0, 0),
(1400643, 532001, 55, 7, 5, 1, 1, 100, 0, 0, 0, 0),
(1400644, 500001, 55, 7, 0, 1, 1, 100, 0, 0, 0, 0),
(1400645, 523001, 55, 7, 2, 1, 1, 100, 0, 0, 0, 0),
(1400646, 523002, 55, 7, 3, 1, 1, 100, 0, 0, 0, 0),
(1400647, 523003, 55, 7, 4, 1, 1, 100, 0, 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `mail`
--

CREATE TABLE `mail` (
  `id` int(10) NOT NULL,
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
  `bIsRead` bit(1) DEFAULT b'0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `portals`
--

CREATE TABLE `portals` (
  `id` int(10) NOT NULL,
  `CharID` int(10) DEFAULT NULL,
  `Point` smallint(3) DEFAULT NULL,
  `LocationX` float(10,5) DEFAULT NULL,
  `LocationY` float(10,5) DEFAULT NULL,
  `LocationZ` float(10,5) DEFAULT NULL,
  `DirectionX` float(10,5) DEFAULT NULL,
  `DirectionY` float(10,5) DEFAULT NULL,
  `DirectionZ` float(10,5) DEFAULT NULL,
  `WorldID` int(5) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `quickslot`
--

CREATE TABLE `quickslot` (
  `pkQuick` int(15) NOT NULL,
  `charId` int(11) DEFAULT NULL,
  `slotId_0` int(10) UNSIGNED DEFAULT '0',
  `slotId_1` int(10) UNSIGNED DEFAULT '0',
  `slotId_2` int(10) UNSIGNED DEFAULT '0',
  `slotId_3` int(10) UNSIGNED DEFAULT '0',
  `slotId_4` int(10) UNSIGNED DEFAULT '0',
  `slotId_5` int(10) UNSIGNED DEFAULT '0',
  `slotId_6` int(10) UNSIGNED DEFAULT '0',
  `slotId_7` int(10) UNSIGNED DEFAULT '0',
  `slotId_8` int(10) UNSIGNED DEFAULT '0',
  `slotId_9` int(10) UNSIGNED DEFAULT '0',
  `slotId_10` int(10) UNSIGNED DEFAULT '0',
  `slotId_11` int(10) UNSIGNED DEFAULT '0',
  `slotId_12` int(10) UNSIGNED DEFAULT '0',
  `slotId_13` int(10) UNSIGNED DEFAULT '0',
  `slotId_14` int(10) UNSIGNED DEFAULT '0',
  `slotId_15` int(10) UNSIGNED DEFAULT '0',
  `slotId_16` int(10) UNSIGNED DEFAULT '0',
  `slotId_17` int(10) UNSIGNED DEFAULT '0',
  `slotId_18` int(10) UNSIGNED DEFAULT '0',
  `slotId_19` int(10) UNSIGNED DEFAULT '0',
  `slotId_20` int(10) UNSIGNED DEFAULT '0',
  `slotId_21` int(10) UNSIGNED DEFAULT '0',
  `slotId_22` int(10) UNSIGNED DEFAULT '0',
  `slotId_23` int(10) UNSIGNED DEFAULT '0',
  `slotId_24` int(10) UNSIGNED DEFAULT '0',
  `slotId_25` int(10) UNSIGNED DEFAULT '0',
  `slotId_26` int(10) UNSIGNED DEFAULT '0',
  `slotId_27` int(10) UNSIGNED DEFAULT '0',
  `slotId_28` int(10) UNSIGNED DEFAULT '0',
  `slotId_29` int(10) UNSIGNED DEFAULT '0',
  `slotId_30` int(10) UNSIGNED DEFAULT '0',
  `slotId_31` int(10) UNSIGNED DEFAULT '0',
  `slotId_32` int(10) UNSIGNED DEFAULT '0',
  `slotId_33` int(10) UNSIGNED DEFAULT '0',
  `slotId_34` int(10) UNSIGNED DEFAULT '0',
  `slotId_35` int(10) UNSIGNED DEFAULT '0',
  `slotId_36` int(10) UNSIGNED DEFAULT '0',
  `slotId_37` int(10) UNSIGNED DEFAULT '0',
  `slotId_38` int(10) UNSIGNED DEFAULT '0',
  `slotId_39` int(10) UNSIGNED DEFAULT '0',
  `slotId_40` int(10) UNSIGNED DEFAULT '0',
  `slotId_41` int(10) UNSIGNED DEFAULT '0',
  `slotId_42` int(10) UNSIGNED DEFAULT '0',
  `slotId_43` int(10) UNSIGNED DEFAULT '0',
  `slotId_44` int(10) UNSIGNED DEFAULT '0',
  `slotId_45` int(10) UNSIGNED DEFAULT '0',
  `slotId_46` int(10) UNSIGNED DEFAULT '0',
  `slotId_47` int(10) DEFAULT NULL,
  `placeID` int(10) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `quickslot`
--

INSERT INTO `quickslot` (`pkQuick`, `charId`, `slotId_0`, `slotId_1`, `slotId_2`, `slotId_3`, `slotId_4`, `slotId_5`, `slotId_6`, `slotId_7`, `slotId_8`, `slotId_9`, `slotId_10`, `slotId_11`, `slotId_12`, `slotId_13`, `slotId_14`, `slotId_15`, `slotId_16`, `slotId_17`, `slotId_18`, `slotId_19`, `slotId_20`, `slotId_21`, `slotId_22`, `slotId_23`, `slotId_24`, `slotId_25`, `slotId_26`, `slotId_27`, `slotId_28`, `slotId_29`, `slotId_30`, `slotId_31`, `slotId_32`, `slotId_33`, `slotId_34`, `slotId_35`, `slotId_36`, `slotId_37`, `slotId_38`, `slotId_39`, `slotId_40`, `slotId_41`, `slotId_42`, `slotId_43`, `slotId_44`, `slotId_45`, `slotId_46`, `slotId_47`, `placeID`) VALUES
(23, 26, 10111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(24, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(25, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(26, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(27, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(28, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20091, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(29, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(30, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(31, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(32, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(33, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(34, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(35, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(36, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(37, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(38, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(39, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(40, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(41, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(42, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(43, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(44, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(45, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(46, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(47, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(48, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(49, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(50, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(51, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL),
(52, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);

-- --------------------------------------------------------

--
-- Table structure for table `skills`
--

CREATE TABLE `skills` (
  `id` int(11) UNSIGNED NOT NULL,
  `skill_id` int(10) UNSIGNED NOT NULL DEFAULT '0',
  `owner_id` int(11) UNSIGNED NOT NULL,
  `RpBonusAuto` bit(1) DEFAULT b'0',
  `RpBonusType` int(5) UNSIGNED NOT NULL DEFAULT '0',
  `SlotID` smallint(3) UNSIGNED NOT NULL DEFAULT '0',
  `TimeRemaining` int(15) UNSIGNED NOT NULL,
  `Exp` int(5) UNSIGNED NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `skills`
--

INSERT INTO `skills` (`id`, `skill_id`, `owner_id`, `RpBonusAuto`, `RpBonusType`, `SlotID`, `TimeRemaining`, `Exp`) VALUES
(179, 10111, 26, b'1111111111111111111111111111111', 0, 1, 0, 0),
(180, 10111, 27, b'1111111111111111111111111111111', 0, 1, 0, 0),
(181, 10111, 28, b'1111111111111111111111111111111', 0, 1, 0, 0),
(182, 10311, 28, b'1111111111111111111111111111111', 0, 3, 0, 1),
(183, 10211, 28, b'1111111111111111111111111111111', 0, 2, 0, 1),
(184, 20111, 28, b'1111111111111111111111111111111', 0, 3, 0, 1),
(185, 20021, 28, b'1111111111111111111111111111111', 0, 10, 0, 1),
(186, 20091, 28, b'1111111111111111111111111111111', 0, 18, 0, 1),
(187, 10111, 29, b'1111111111111111111111111111111', 0, 1, 0, 0),
(188, 10111, 30, b'1111111111111111111111111111111', 0, 1, 0, 0),
(189, 10111, 31, b'1111111111111111111111111111111', 0, 1, 0, 0),
(190, 10311, 31, b'1111111111111111111111111111111', 0, 3, 0, 1),
(191, 20111, 31, b'1111111111111111111111111111111', 0, 3, 0, 1),
(192, 20091, 31, b'1111111111111111111111111111111', 0, 18, 0, 1),
(193, 10111, 32, b'1111111111111111111111111111111', 0, 1, 0, 0),
(194, 10111, 33, b'1111111111111111111111111111111', 0, 1, 0, 0),
(195, 10111, 34, b'1111111111111111111111111111111', 0, 1, 0, 0),
(196, 10111, 35, b'1111111111111111111111111111111', 0, 1, 0, 0),
(197, 10111, 36, b'1111111111111111111111111111111', 0, 1, 0, 0),
(198, 10111, 37, b'1111111111111111111111111111111', 0, 1, 0, 0),
(199, 10111, 38, b'1111111111111111111111111111111', 0, 1, 0, 0),
(200, 10111, 39, b'1111111111111111111111111111111', 0, 1, 0, 0),
(201, 10111, 40, b'1111111111111111111111111111111', 0, 1, 0, 0),
(202, 10111, 41, b'1111111111111111111111111111111', 0, 1, 0, 0),
(203, 10111, 42, b'1111111111111111111111111111111', 0, 1, 0, 0),
(204, 10111, 43, b'1111111111111111111111111111111', 0, 1, 0, 0),
(205, 10111, 44, b'1111111111111111111111111111111', 0, 1, 0, 0),
(206, 10111, 45, b'1111111111111111111111111111111', 0, 1, 0, 0),
(207, 10111, 46, b'1111111111111111111111111111111', 0, 1, 0, 0),
(208, 10111, 47, b'1111111111111111111111111111111', 0, 1, 0, 0),
(209, 10111, 48, b'1111111111111111111111111111111', 0, 1, 0, 0),
(210, 10111, 49, b'1111111111111111111111111111111', 0, 1, 0, 0),
(211, 10111, 50, b'1111111111111111111111111111111', 0, 1, 0, 0),
(212, 10111, 51, b'1111111111111111111111111111111', 0, 1, 0, 0),
(213, 10111, 52, b'1111111111111111111111111111111', 0, 1, 0, 0),
(214, 10111, 53, b'1111111111111111111111111111111', 0, 1, 0, 0),
(215, 10111, 54, b'1111111111111111111111111111111', 0, 1, 0, 0),
(216, 110111, 55, b'1111111111111111111111111111111', 0, 1, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `warfoginfo`
--

CREATE TABLE `warfoginfo` (
  `idFog` int(11) NOT NULL,
  `hObject` int(11) NOT NULL,
  `owner_id` int(10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `accounts`
--
ALTER TABLE `accounts`
  ADD PRIMARY KEY (`AccountID`);

--
-- Indexes for table `bannword`
--
ALTER TABLE `bannword`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `bind`
--
ALTER TABLE `bind`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `blacklist`
--
ALTER TABLE `blacklist`
  ADD PRIMARY KEY (`pkID`);

--
-- Indexes for table `buddylist`
--
ALTER TABLE `buddylist`
  ADD PRIMARY KEY (`pkID`);

--
-- Indexes for table `characters`
--
ALTER TABLE `characters`
  ADD PRIMARY KEY (`CharID`);

--
-- Indexes for table `charquestlist`
--
ALTER TABLE `charquestlist`
  ADD PRIMARY KEY (`pkQtTable`);

--
-- Indexes for table `friendlist`
--
ALTER TABLE `friendlist`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `guilds`
--
ALTER TABLE `guilds`
  ADD PRIMARY KEY (`GuildID`);

--
-- Indexes for table `guild_members`
--
ALTER TABLE `guild_members`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `items`
--
ALTER TABLE `items`
  ADD PRIMARY KEY (`id`),
  ADD KEY `owner_id_idx` (`owner_id`),
  ADD KEY `item_vnum_index` (`tblidx`);

--
-- Indexes for table `mail`
--
ALTER TABLE `mail`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `portals`
--
ALTER TABLE `portals`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `quickslot`
--
ALTER TABLE `quickslot`
  ADD PRIMARY KEY (`pkQuick`);

--
-- Indexes for table `skills`
--
ALTER TABLE `skills`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `warfoginfo`
--
ALTER TABLE `warfoginfo`
  ADD PRIMARY KEY (`idFog`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `accounts`
--
ALTER TABLE `accounts`
  MODIFY `AccountID` int(10) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=32;
--
-- AUTO_INCREMENT for table `bannword`
--
ALTER TABLE `bannword`
  MODIFY `id` int(5) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `bind`
--
ALTER TABLE `bind`
  MODIFY `id` int(10) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `blacklist`
--
ALTER TABLE `blacklist`
  MODIFY `pkID` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `buddylist`
--
ALTER TABLE `buddylist`
  MODIFY `pkID` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `characters`
--
ALTER TABLE `characters`
  MODIFY `CharID` int(10) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=56;
--
-- AUTO_INCREMENT for table `charquestlist`
--
ALTER TABLE `charquestlist`
  MODIFY `pkQtTable` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `friendlist`
--
ALTER TABLE `friendlist`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `guilds`
--
ALTER TABLE `guilds`
  MODIFY `GuildID` int(5) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `guild_members`
--
ALTER TABLE `guild_members`
  MODIFY `id` int(10) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `items`
--
ALTER TABLE `items`
  MODIFY `id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1400648;
--
-- AUTO_INCREMENT for table `mail`
--
ALTER TABLE `mail`
  MODIFY `id` int(10) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `portals`
--
ALTER TABLE `portals`
  MODIFY `id` int(10) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `quickslot`
--
ALTER TABLE `quickslot`
  MODIFY `pkQuick` int(15) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=53;
--
-- AUTO_INCREMENT for table `skills`
--
ALTER TABLE `skills`
  MODIFY `id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=217;
--
-- AUTO_INCREMENT for table `warfoginfo`
--
ALTER TABLE `warfoginfo`
  MODIFY `idFog` int(11) NOT NULL AUTO_INCREMENT;COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
