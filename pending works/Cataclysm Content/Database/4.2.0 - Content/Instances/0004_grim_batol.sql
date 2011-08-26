UPDATE creature_template set scriptname = 'boss_general_umbriss' where entry = 39625;
UPDATE creature_template set scriptname = 'npc_malignant' where entry = 39984;
UPDATE creature_template set scriptname = 'npc_trogg_dweller' where entry = 45467;

DELETE FROM `creature_text` WHERE `entry` IN (39625);
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(39625,0,0, 'A million more await my orders. What chance you do have?',1,0,0,0,0,18530, 'umbriss - SAY_AGGRO'),
(39625,1,0, 'Cover the rear! Alexstraza\'s brood decimate our rank!',1,0,0,0,0,18537, 'umbriss - SAY_BOMBING_1'),
(39625,2,0, 'Reinforce the front! We are being invaded!',1,0,0,0,0,18536, 'umbriss - SAY_BOMBING_2'),
(39625,3,0, 'Attack you cowardly vermin!',1,0,0,0,0,18535, 'umbriss - SAY_SUMMON'),
(39625,4,0, 'Messy...',1,0,0,0,0,18532, 'umbriss - SAY_KILL_1'),
(39625,5,0, 'Vermin, your dinner awaits!',1,0,0,0,0,18531, 'umbriss - SAY_KILL_2'),
(39625,6,0, 'General Umbriss sets his eyes on $N and begins to cast blitz.',3,0,0,0,0,18533, 'umbriss - EMOTE_BLITZ'),
(39625,7,0, 'General Umbriss begins to cast ground siege.',3,0,0,0,0,18534, 'umbriss - EMOTE_GROUND_SIEGE'),
(39625,8,0, 'General Umbriss goes into a frenzy !',3,0,0,0,0,0, 'umbriss - EMOTE_FRENZY');

update instance_template set script = 'instance_grimbatol' where map = 670;