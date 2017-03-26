SET @GUID := 107706;
SET @POOL := 1208;

-- ----------------------------
-- Sandarr Dunereaver (10080)
-- ----------------------------
DELETE FROM `creature` WHERE `guid` IN (@GUID, @GUID + 1);
INSERT INTO `creature` VALUES (@GUID, 10080, 209, 1, 0, 0, 1546.25, 1017.14, 8.87683, 0.0109968, 7200, 0, 0, 5544, 0, 0); -- Sandarr Dunereaver
INSERT INTO `creature` VALUES (@GUID + 1, 7269, 209, 1, 0, 0, 1546.25, 1017.14, 8.87683, 0.0109968, 60, 5, 0, 1782, 0, 1); -- Sandarr Dunereaver placeholder

DELETE FROM `pool_creature` WHERE `guid` IN (@GUID, @GUID + 1);
INSERT INTO `pool_creature` VALUES
(@GUID, @POOL, 10, 'Sandarr Dunereaver (Zul\'Farrak)'),
(@GUID + 1, @POOL, 0, 'Sandarr Dunereaver placeholder (Zul\'Farrak)');

DELETE FROM `pool_template` WHERE `entry` = @POOL;
INSERT INTO `pool_template` VALUES
(@POOL, 1, 'Sandarr Dunereaver (Zul\'Farrak)');

-- ----------------------------
-- Dustwraith (10081)
-- ----------------------------
DELETE FROM `creature` WHERE `guid` IN (@GUID + 2, @GUID + 3);
INSERT INTO `creature` VALUES (@GUID + 2, 10081, 209, 1, 0, 0, 1627.47, 1186.85, 8.87693, 0.647954, 7200, 15, 0, 5757, 0, 1); -- Dustwraith
INSERT INTO `creature` VALUES (@GUID + 3, 7269, 209, 1, 0, 0, 1627.47, 1186.85, 8.87693, 0.647954, 60, 5, 0, 1782, 0, 1); -- Dustwraith placeholder

DELETE FROM `pool_creature` WHERE `guid` IN (@GUID + 2, @GUID + 3);
INSERT INTO `pool_creature` VALUES
(@GUID + 2, @POOL + 1, 10, 'Dustwraith (Zul\'Farrak)'),
(@GUID + 3, @POOL + 1, 0, 'Dustwraith placeholder (Zul\'Farrak)');

DELETE FROM `pool_template` WHERE `entry` = @POOL + 1;
INSERT INTO `pool_template` VALUES
(@POOL + 1, 1, 'Dustwraith (Zul\'Farrak)');

-- ----------------------------
-- Zerillis (10082)
-- ----------------------------
DELETE FROM `creature` WHERE `guid` IN (@GUID + 4);
INSERT INTO `creature` VALUES (@GUID + 4, 10082, 209, 1, 0, 0, 1628.94, 952.042, 8.87684, 5.43152, 7200, 15, 0, 5544, 0, 1); -- Zerillis

DELETE FROM `pool_creature` WHERE `guid` IN (@GUID + 4, 44170);
INSERT INTO `pool_creature` VALUES
(@GUID + 4, @POOL + 2, 30, 'Zerillis (Zul\'Farrak)'),
(44170, @POOL + 2, 0, 'Zerillis placeholder (Zul\'Farrak)');

DELETE FROM `pool_template` WHERE `entry` = @POOL + 1;
INSERT INTO `pool_template` VALUES
(@POOL + 2, 1, 'Zerillis (Zul\'Farrak)');