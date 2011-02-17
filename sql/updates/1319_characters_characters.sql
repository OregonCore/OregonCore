ALTER TABLE characters
  ADD COLUMN `arenaPoints` int(10) UNSIGNED NOT NULL default '0' AFTER arena_pending_points,
  ADD COLUMN `totalHonorPoints` int(10) UNSIGNED NOT NULL default '0' AFTER arenaPoints,
  ADD COLUMN `todayHonorPoints` int(10) UNSIGNED NOT NULL default '0' AFTER totalHonorPoints,
  ADD COLUMN `yesterdayHonorPoints` int(10) UNSIGNED NOT NULL default '0' AFTER todayHonorPoints,
  ADD COLUMN `totalKills` int(10) UNSIGNED NOT NULL default '0' AFTER yesterdayHonorPoints,
  ADD COLUMN `todayKills` smallint(5) UNSIGNED NOT NULL default '0' AFTER totalKills,
  ADD COLUMN `yesterdayKills` smallint(5) UNSIGNED NOT NULL default '0' AFTER todayKills,
  ADD COLUMN `chosenTitle` int(10) UNSIGNED NOT NULL default '0' AFTER yesterdayKills,
  ADD COLUMN `watchedFaction` int(10) UNSIGNED NOT NULL default '0' AFTER chosenTitle,
  ADD COLUMN `drunk` smallint(5) UNSIGNED NOT NULL default '0' AFTER watchedFaction,
  ADD COLUMN `health` int(10) UNSIGNED NOT NULL default '0' AFTER drunk,
  ADD COLUMN `powerMana` int(10) UNSIGNED NOT NULL default '0' AFTER health,
  ADD COLUMN `powerRage` int(10) UNSIGNED NOT NULL default '0' AFTER powerMana,
  ADD COLUMN `powerFocus` int(10) UNSIGNED NOT NULL default '0' AFTER powerRage,
  ADD COLUMN `powerEnergy` int(10) UNSIGNED NOT NULL default '0' AFTER powerFocus,
  ADD COLUMN `powerHappiness` int(10) UNSIGNED NOT NULL default '0' AFTER powerEnergy;

UPDATE IGNORE characters SET
  arenaPoints = arena_pending_points +
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 1563))+2, length(SUBSTRING_INDEX(data, ' ', 1563+1))- length(SUBSTRING_INDEX(data, ' ', 1563)) - 1),
  totalHonorPoints =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 1562))+2, length(SUBSTRING_INDEX(data, ' ', 1562+1))- length(SUBSTRING_INDEX(data, ' ', 1562)) - 1),
  todayHonorPoints =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 1515))+2, length(SUBSTRING_INDEX(data, ' ', 1515+1))- length(SUBSTRING_INDEX(data, ' ', 1515)) - 1),
  yesterdayHonorPoints =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 1516))+2, length(SUBSTRING_INDEX(data, ' ', 1516+1))- length(SUBSTRING_INDEX(data, ' ', 1516)) - 1),
  totalKills =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 1517))+2, length(SUBSTRING_INDEX(data, ' ', 1517+1))- length(SUBSTRING_INDEX(data, ' ', 1517)) - 1),
  todayKills =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 1514))+2, length(SUBSTRING_INDEX(data, ' ', 1514+1))- length(SUBSTRING_INDEX(data, ' ', 1514)) - 1) & 0x0000FFFF,
  yesterdayKills =
    (SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 1514))+2, length(SUBSTRING_INDEX(data, ' ', 1514+1))- length(SUBSTRING_INDEX(data, ' ', 1514)) - 1) & 0xFFFF0000) >> 16,
  chosenTitle =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 648))+2, length(SUBSTRING_INDEX(data, ' ', 648+1))- length(SUBSTRING_INDEX(data, ' ', 648)) - 1),
  watchedFaction =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 1519))+2, length(SUBSTRING_INDEX(data, ' ', 1519+1))- length(SUBSTRING_INDEX(data, ' ', 1519)) - 1),
  drunk =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 241))+2, length(SUBSTRING_INDEX(data, ' ', 241+1))- length(SUBSTRING_INDEX(data, ' ', 241)) - 1) & 0xFFFE,
  health =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 22))+2, length(SUBSTRING_INDEX(data, ' ', 22+1))- length(SUBSTRING_INDEX(data, ' ', 22)) - 1),
  powerMana =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 23))+2, length(SUBSTRING_INDEX(data, ' ', 23+1))- length(SUBSTRING_INDEX(data, ' ', 23)) - 1),
  powerRage =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 24))+2, length(SUBSTRING_INDEX(data, ' ', 24+1))- length(SUBSTRING_INDEX(data, ' ', 24)) - 1),
  powerFocus =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 25))+2, length(SUBSTRING_INDEX(data, ' ', 25+1))- length(SUBSTRING_INDEX(data, ' ', 25)) - 1),
  powerEnergy =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 26))+2, length(SUBSTRING_INDEX(data, ' ', 26+1))- length(SUBSTRING_INDEX(data, ' ', 26)) - 1),
  powerHappiness =
    SUBSTRING(data, length(SUBSTRING_INDEX(data, ' ', 27))+2, length(SUBSTRING_INDEX(data, ' ', 27+1))- length(SUBSTRING_INDEX(data, ' ', 27)) - 1);

ALTER TABLE characters
  DROP COLUMN arena_pending_points;
