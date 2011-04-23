
CREATE TABLE `mdl_attendance_log` (
  `id` bigint(10) unsigned NOT NULL auto_increment,
  `sessionid` bigint(10) unsigned NOT NULL default '0',
  `studentid` bigint(10) unsigned NOT NULL default '0',
  `statusid` bigint(10) unsigned NOT NULL default '0',
  `statusset` varchar(100) default NULL,
  `timetaken` bigint(10) unsigned NOT NULL default '0',
  `takenby` bigint(10) unsigned NOT NULL default '0',
  `remarks` varchar(255) default NULL,
  PRIMARY KEY  (`id`),
  KEY `mdl_attelog_ses_ix` (`sessionid`),
  KEY `mdl_attelog_stu_ix` (`studentid`),
  KEY `mdl_attelog_sta_ix` (`statusid`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COMMENT='attendance_log table retrofitted from MySQL' 

 CREATE TABLE `mdl_attendance_sessions` (
  `id` bigint(10) unsigned NOT NULL auto_increment,
  `courseid` bigint(10) unsigned NOT NULL default '0',
  `sessdate` bigint(10) unsigned NOT NULL default '0',
  `duration` bigint(10) unsigned NOT NULL default '0',
  `lasttaken` bigint(10) unsigned default NULL,
  `lasttakenby` bigint(10) unsigned NOT NULL default '0',
  `timemodified` bigint(10) unsigned default NULL,
  `description` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`),
  KEY `mdl_attesess_cou_ix` (`courseid`),
  KEY `mdl_attesess_ses_ix` (`sessdate`)
) ENGINE=MyISAM AUTO_INCREMENT=41 DEFAULT CHARSET=utf8 COMMENT='attendance_sessions table retrofitted from MySQL' 

CREATE TABLE `mdl_attendance_statuses` (
  `id` bigint(10) unsigned NOT NULL auto_increment,
  `courseid` bigint(10) unsigned NOT NULL default '0',
  `acronym` varchar(2) NOT NULL default '',
  `description` varchar(30) NOT NULL default '',
  `grade` smallint(3) NOT NULL default '0',
  `visible` tinyint(1) unsigned NOT NULL default '1',
  `deleted` tinyint(1) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `mdl_attestat_cou_ix` (`courseid`),
  KEY `mdl_attestat_vis_ix` (`visible`),
  KEY `mdl_attestat_del_ix` (`deleted`)
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=utf8 COMMENT='attendance_statuses table retrofitted from MySQL' 