Database Installation Order:
======================
Run the following SQL files in order in your database to successfully set up this section of the OregonCore project.
  1. sql/create_mysql.sql
  2. sql/world.sql
  3. sql/characters.sql
  4. sql/realmd.sql
  5. [OregonDatabase.zip](https://github.com/OregonCore/OregonCore/releases) (shuold be extracted first)

Contributing SQL:
======================
In order for us to keep track of SQL fixes we prefix our SQL files with a date format YYYY_MM_DD followed by the table the update is most likely to affect. In some cases, where an update fixes a quest or zone, it is okay to put that instead.


 - **A query must always end in a semi-colon( ; )**
 -  **Supply names where applicable**
 -  **DELETE before INSERT & Use exact fields**
 -  **If you're referencing a value more than 3 times, use a variable**
 - **Comments and Author references must be provided where applicable**

Wrong:

    INSERT INTO spell_group VALUES (1, 1239)
    
Correct:

    -- Add spell 1239 (Example Spell) to spell group 1
    DELETE FROM `spell_group` WHERE `id` = 1 AND `spell_id` = 1239;
    INSERT INTO spell_group (`id`,`spell_id`) VALUES
    (1, 1239); -- (Example Spell)
