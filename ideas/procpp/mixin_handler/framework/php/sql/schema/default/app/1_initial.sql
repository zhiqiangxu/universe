CREATE TABLE app
(
    `id` varchar(32) NOT NULL,
    `secret` varchar(32) NOT NULL,
    `department` varchar(255) NOT NULL,
    `ctx` varchar(32) NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB CHARSET=utf8;
