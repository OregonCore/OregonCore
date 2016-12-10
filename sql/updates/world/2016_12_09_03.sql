DELETE FROM `script_texts`  WHERE `entry` IN (-1189001, -1189002, -1189003, -1189004, -1189005, -1189006, -1189007);
DELETE FROM `script_texts` WHERE `entry` IN (-1123682, -1123683, -1123684, -1123685, -1123686, -1123687, -1123688);
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES 
(-1123682, 'It is over, your search is done. Let fate choose now, the righteous one.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 11961, 1, 0, 0, 'HeadlessHorsman SAY_AGGRO'),
(-1123683, 'Your body lies beaten, battered and broken. Let my curse be your own, fate has spoken.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 11962, 1, 0, 0, 'Headless Horseman PLAYER_DEATH'),
(-1123684, 'Soldiers arise, stand and fight! Bring victory at last to this fallen knight!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 11963, 1, 0, 0, 'Headless Horseman SUMMON_PUMKINS'),
(-1123685, 'This end have I reached before. What new adventure lies in store?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 11964, 1, 0, 0, 'Headless Horseman SAY_DEATH'),
(-1123686, 'Harken, cur! Tis you I spurn! Now feel... the burn!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 12573, 1, 0, 0, 'Headless Horseman SAY_CONFLAG'),
(-1123687, 'Get over here, you idiot!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 12569, 1, 0, 0, 'Headless HorsemanBODY'),
(-1123688, 'Here\'s my body, fit and pure! Now, your blackened souls I\'ll cure!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 12567, 1, 0, 0, 'Headless HorsemanRejoin');
