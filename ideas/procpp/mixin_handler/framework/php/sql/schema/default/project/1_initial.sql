CREATE TABLE project
(
    `id` INT(11) NOT NULL AUTO_INCREMENT,
    `kickoff_date` DATETIME NOT NULL,
    `deadline` DATETIME DEFAULT NULL,
    `online_date` DATETIME DEFAULT NULL,
    `dev_branch` VARCHAR(255) DEFAULT NULL,
    `description` TEXT NOT NULL,
    `rd` VARCHAR(255) DEFAULT NULL,
    `qa` VARCHAR(255) DEFAULT NULL,
    `pm` VARCHAR(255) DEFAULT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB CHARSET=utf8;
