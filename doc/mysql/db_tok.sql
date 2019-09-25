DROP TABLE IF EXISTS `t_tokme_member`;
CREATE TABLE `t_tokme_member` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `tok_id` char(77) NOT NULL DEFAULT '' COMMENT 'tox id',
  `pk` char(65) NOT NULL DEFAULT '' COMMENT 'public key',
  `nick_name` varbinary(128) NOT NULL DEFAULT '' COMMENT 'user nick name',
  `bio` varbinary(1024) NOT NULL DEFAULT '' COMMENT 'user self introduction',
  `signature` varbinary(1024) NOT NULL DEFAULT '' COMMENT 'user signature',
  `create_time` bigint(11) unsigned NOT NULL DEFAULT '0' COMMENT 'first start time',
  `update_time` bigint(11) unsigned NOT NULL DEFAULT '0' COMMENT 'last update time',
  PRIMARY KEY (`id`),
  UNIQUE KEY `index_pk` (`pk`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='tokme member ';