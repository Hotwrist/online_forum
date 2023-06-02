CREATE SCHEMA IF NOT EXISTS `Users` DEFAULT CHARACTER SET utf8;
USE `Users`;

/* Admin */
CREATE TABLE IF NOT EXISTS `Users`.`admin_credentials` (
	`admin_id` INT NOT NULL,
	`name` VARCHAR(50) NOT NULL,
	`username` VARCHAR(20) NOT NULL,
	PRIMARY KEY (`admin_id`),
	UNIQUE INDEX `unique_admin_username` (`username` ASC));

--#######################################################################################
/* Ordinary Users */
CREATE TABLE IF NOT EXISTS `Users`.`ordinary_users` (
	`ordinary_users_id` INT NOT NULL AUTO_INCREMENT,
	`name` VARCHAR(50) NOT NULL,
	`username` VARCHAR(20) NOT NULL,
	PRIMARY KEY (`ordinary_users_id`),
	UNIQUE INDEX `unique_username` (`username` ASC));

--#######################################################################################	

/* Login */
CREATE TABLE IF NOT EXISTS `Users`.`login` (
	`username` VARCHAR(20) NOT NULL,
	`password` VARCHAR(66) NOT NULL,
	`email` VARCHAR(50) NOT NULL,
	`fullname` VARCHAR(50) NOT NULL,
	`online` TINYINT NOT NULL,
	PRIMARY KEY (`username`),
	UNIQUE INDEX `unique_email` (`email` ASC),
	UNIQUE INDEX `username_UNIQUE` (`username` ASC));
    
--########################################################################################--	
/* Thread */
CREATE TABLE IF NOT EXISTS `Users`.`threads` (
	`thread_id` INT NULL AUTO_INCREMENT,
	`thread_name` VARCHAR(100) NOT NULL,
	PRIMARY KEY (`thread_id`),
	UNIQUE INDEX `unique_thread_name` (`thread_name` ASC));

/* Posts in a thread */
CREATE TABLE IF NOT EXISTS `Users`.`thread_posts` (
	`thread_post_id` INT NULL AUTO_INCREMENT,
	`fk_thread_name` VARCHAR(100) NOT NULL,
	`username` VARCHAR(20) NOT NULL,	
	`thread_post` VARCHAR(250) NOT NULL,
	PRIMARY KEY (`thread_post_id`),
	
	CONSTRAINT `thread_name`
		FOREIGN KEY (`fk_thread_name`)
		REFERENCES `Users`.`threads` (`thread_name`)
		ON DELETE CASCADE
		ON UPDATE CASCADE);
		
/* Database for sent emails, admin */
CREATE TABLE IF NOT EXISTS `Users`.`admin_email` (
	`email` VARCHAR(20) NOT NULL,
	`name` VARCHAR(20) NOT NULL,
	`code` VARCHAR(50) NOT NULL,
	PRIMARY KEY (`email`),
	CONSTRAINT `admin_email`
		FOREIGN KEY (`name`)
		REFERENCES `Users`.`admin_credentials`(`username`)
		ON DELETE CASCADE);

/* Database for sent emails for ordinary users */
CREATE TABLE IF NOT EXISTS `Users`.`sent_emails` (
	`email` VARCHAR(20) NOT NULL,
	`name` VARCHAR(20) NOT NULL,
	`code` VARCHAR(50) NOT NULL,
	PRIMARY KEY (`email`),
	CONSTRAINT `users_email`
		FOREIGN KEY (`name`)
		REFERENCES `Users`.`ordinary_users`(`username`)
		ON DELETE CASCADE);

--########################################################################################
/* Sessions */
CREATE TABLE IF NOT EXISTS `Users`.`Sessions` (
  `SessionID` VARCHAR(65) NOT NULL,
  `fk_username` VARCHAR(20) NOT NULL,
  `expires` VARCHAR(45) NOT NULL,
  `IP` VARCHAR(15) NOT NULL,
  `online` TINYINT NOT NULL,
  PRIMARY KEY (`SessionID`),
  UNIQUE INDEX `SessionID_UNIQUE` (`SessionID` ASC),
  INDEX `name_idx` (`fk_username` ASC),
  CONSTRAINT `fk_user_session`
    FOREIGN KEY (`fk_username`)
    REFERENCES `Users`.`login` (`username`)
    ON DELETE CASCADE
    ON UPDATE CASCADE);
    
/* TOTP */
CREATE TABLE IF NOT EXISTS `Users`.`TOTP` (
  `seed` VARCHAR(56) NOT NULL,
  `fk_username` VARCHAR(20) NOT NULL,
  `online` TINYINT NOT NULL,
  PRIMARY KEY (`fk_username`),
  UNIQUE INDEX `seed_UNIQUE` (`seed` ASC),
  CONSTRAINT `fk_user_totp`
    FOREIGN KEY (`fk_username`)
    REFERENCES `Users`.`login` (`username`)
    ON DELETE CASCADE
    ON UPDATE CASCADE);
    
/* 2FA */
CREATE TABLE IF NOT EXISTS `Users`.`2FA` (
  `code` INT NOT NULL,
  `fk_username` VARCHAR(20) NOT NULL,
  `id` INT NOT NULL AUTO_INCREMENT,
  `expires` DATETIME NULL,
  UNIQUE INDEX `fk_user_idx` (`fk_username` ASC),
  PRIMARY KEY (`id`),
  CONSTRAINT `fk_user_2fa` FOREIGN KEY (`fk_username`) REFERENCES `Users`.`login` (`username`)
  ON DELETE CASCADE
  ON UPDATE CASCADE);
--###########################################################################################

CREATE USER 'admin'@'localhost' IDENTIFIED BY 'vxbyt&^%rxcdquthy$@#990234&';
GRANT SELECT, DELETE ON Users.ordinary_users TO 'admin'@'localhost';
GRANT SELECT, DELETE ON Users.threads TO 'admin'@'localhost';
GRANT SELECT, DELETE ON Users.thread_posts TO 'admin'@'localhost';
GRANT SELECT, DELETE ON Users.sent_emails TO 'admin'@'localhost';
GRANT SELECT, DELETE ON Users.2FA TO 'admin'@'localhost';
GRANT SELECT, DELETE ON Users.Sessions TO 'admin'@'localhost';
GRANT SELECT, DELETE ON Users.admin_email TO 'admin'@'localhost';
GRANT SELECT, DELETE ON Users.TOTP TO 'admin'@'localhost';

CREATE USER 'register'@'localhost' IDENTIFIED BY 'xvbtyttrabcegtt&$22789%$&#@';
GRANT SELECT, INSERT, UPDATE ON Users.login TO 'register'@'localhost';
GRANT UPDATE(online) ON Users.login TO 'register'@'localhost';
GRANT SELECT, INSERT, UPDATE ON Users.admin_credentials TO 'register'@'localhost';
GRANT SELECT, INSERT, UPDATE ON Users.ordinary_users TO 'register'@'localhost';
GRANT SELECT, INSERT ON Users.TOTP TO 'register'@'localhost';
GRANT UPDATE(online, seed) ON Users.TOTP TO 'register'@'localhost';
GRANT SELECT, INSERT ON Users.2FA TO 'register'@'localhost';

CREATE USER 'stateful'@'localhost' IDENTIFIED BY 'mxyct654@3$$%uity987#&hicdf';
GRANT INSERT, SELECT ON Users.Sessions TO 'stateful'@'localhost';

CREATE USER 'login'@'localhost' IDENTIFIED BY 'xcv6ertwq2#$!@098ujtyvbxmsk';
GRANT SELECT, UPDATE ON Users.TOTP TO 'login'@'localhost';
GRANT SELECT, INSERT ON Users.2FA TO 'login'@'localhost';
GRANT SELECT, INSERT, UPDATE ON Users.login TO 'login'@'localhost';
GRANT SELECT, INSERT, UPDATE(online) ON Users.Sessions TO 'login'@'localhost';
GRANT SELECT, INSERT ON Users.sent_emails TO 'login'@'localhost';
GRANT SELECT, INSERT ON Users.admin_email TO 'login'@'localhost';

CREATE USER 'logout'@'localhost' IDENTIFIED BY 'u787%$pass##d@^&)097Tlo#$%*';
GRANT SELECT, DELETE ON Users.2FA TO 'logout'@'localhost';
GRANT SELECT, DELETE ON Users.Sessions TO 'logout'@'localhost';
GRANT SELECT, DELETE ON Users.sent_emails TO 'logout'@'localhost';
GRANT SELECT, DELETE ON Users.admin_email TO 'logout'@'localhost';
GRANT SELECT, DELETE ON Users.TOTP TO 'logout'@'localhost';

CREATE USER 'forum'@'localhost' IDENTIFIED BY 'uJkY%%usArs##d@^&)&@!*Tlo>Y';
GRANT SELECT, INSERT, DELETE ON Users.threads to 'forum'@'localhost';
GRANT SELECT, INSERT, DELETE ON Users.thread_posts to 'forum'@'localhost';
