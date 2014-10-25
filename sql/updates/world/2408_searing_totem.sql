update creature_template set AIName = '' where entry = 3902;
delete from creature_ai_scripts where creature_id = 3902;
