-- Prevent Rot Hide Mongrel from casting Curse of Thule on himself
DELETE FROM creature_ai_scripts WHERE creature_id=1675;
INSERT INTO creature_ai_scripts VALUES 
('167501','1675','4','0','15','0','0','0','0','0','1','-5','-6','0','0','0','0','0','0','0','0','0','Rot Hide Mongrel - Random Say on Aggro'),
('167502','1675','0','0','100','0','7000','14000','30000','45000','11','3237','1','1','0','0','0','0','0','0','0','0','Rot Hide Mongrel - Cast Curse of Thule');
