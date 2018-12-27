CREATE TABLE `anti-farm_log` (
	`id` MEDIUMINT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
	`character` VARCHAR(12) NOT NULL,
	`account` VARCHAR(32) NOT NULL,
	`warnings` MEDIUMINT(1) UNSIGNED NULL DEFAULT '0',
	PRIMARY KEY (`id`)
)
COMMENT='Used by the Anti-Farm System'
COLLATE='utf8_general_ci'
ENGINE=InnoDB
;