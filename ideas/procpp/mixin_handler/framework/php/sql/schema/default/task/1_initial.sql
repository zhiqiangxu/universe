CREATE TABLE task
(
    `id` INT(11) NOT NULL AUTO_INCREMENT,
    `project_id` INT(11) NOT NULL,
    `description` TEXT NOT NULL,
    `rd` VARCHAR(255) DEFAULT NULL,
    `knowledge` TEXT DEFAULT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB CHARSET=utf8;
