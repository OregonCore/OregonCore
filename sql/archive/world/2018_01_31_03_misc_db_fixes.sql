# Original work by Teppic1 on TrinityCore
# Ported to OregonCore by Uberhax
UPDATE `quest_template` SET `PrevQuestId`=364 WHERE `entry`=3096;
UPDATE `quest_template` SET `RequestItemsText`='Welcome to Camp Narache, $n. I hear good things about you. Your bloodline is strong, and many of the elders consider you skilled already. But that we will test.$b$bThe plains of Mulgore will be your home for sometime--you should do your best to learn it very well. One day you will travel to unfamilar lands to master greater skills. You must be ready.' WHERE `entry`=3092;
UPDATE `quest_template` SET `OfferRewardText`='And it is my job to make sure you are prepared by teaching you the basics of our profession--how to tame your pet, how to fire your rifle, and many other skills that will become necessary.$b$bHunters are a strong part of our tradition and history, $n. You would do well to remember how important you are to the tribe. You will be respected and revered if you serve our people well.' WHERE `entry`=3092;
UPDATE `quest_template` SET `OfferRewardText`='Just in time, $N! I was about ready to start digging a hole for our friend here.$B$BMy potion seems to be working; he\\\'s coming to.$B$BHey, I think he\\\'s trying to get your attention. Why don\\\'t you talk to him?' WHERE `entry`=9147;
UPDATE `quest_template` SET `OfferRewardText`='It was just horrible! Night elves! They conjured their moon crystal here for some nefarious plot, but I snuck away and gathered a larger group of our scouts and casters from the nearby Sanctum of the Sun. We killed most of them and the rest of the group is hunting down the remnants!$B$BI\\\'m glad that you\\\'re here now; I was only left a couple of scouts for protection! It\\\'s not safe out here!$B$BGive me a moment and I\\\'ll read through these plans to see what the night elves are up to.' WHERE `entry`=9166;

# Original work by PolarCookie for TrinityCore
# Ported to OregonCore by Uberhax
UPDATE `quest_template` SET `PrevQuestId`='76' WHERE `entry`=239;
UPDATE `quest_template` SET `PrevQuestId`='125' WHERE `entry`=89;

DELETE FROM `npc_vendor` WHERE `entry`=15309;
