DELETE FROM oregon_string WHERE entry in (811, 812, 813, 814, 815, 816);
INSERT INTO oregon_string VALUES
 (812,'Guild Master',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (813,'Officer',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (814,'Veteran',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (815,'Member',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
 (816,'Initiate',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
 
INSERT INTO `oregon_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
(811, 'You cannot enter %s while in a ghost mode!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
