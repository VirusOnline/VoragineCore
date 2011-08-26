-- SFK
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 3877;
DELETE FROM smart_scripts WHERE entryorguid=3877 AND source_type=0;
INSERT INTO smart_scripts (entryorguid,source_type,id,link,event_type,event_phase_mask,event_chance,event_flags,event_param1,event_param2,event_param3,event_param4,action_type,action_param1,action_param2,action_param3,action_param4,action_param5,action_param6,target_type,target_param1,target_param2,target_param3,target_x,target_y,target_z,target_o,comment) VALUES 
(3877,0,0,0,0,0,100,4,2000,4000,6000,10000,11,91801,0,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"Mortal Strike H"),
(3877,0,1,0,0,0,100,0,5000,10000,9000,15000,11,7074,0,0,0,0,0,5,0,0,0,0.0,0.0,0.0,0.0,"New Script");
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47136;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47136 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47136, 0, 0, 0, 0, 0, 100, 0, 2000, 4000, 8000, 15000, 11, 3242, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Ravage'),
(47136, 0, 1, 0, 0, 0, 100, 4, 6000, 15000, 12000, 16000, 11, 91668, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Unstable Strength');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 3873;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=3873 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(3873, 0, 0, 0, 0, 0, 100, 0, 5000, 10000, 15000, 20000, 11, 7054, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Forsaken Ability'),
(3873, 0, 1, 0, 0, 0, 100, 4, 5000, 15000, 20000, 25000, 11, 91463, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Shield Wall');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 3869;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=3869 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(3869, 0, 0, 0, 0, 0, 100, 4, 100, 100, 1500, 1600, 11, 91611, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Gargoyle Strike H');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 3870;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=3870 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(3870, 0, 0, 0, 0, 0, 100, 2, 1000, 5000, 5000, 7000, 11, 30615, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Fear N'),
(3870, 0, 1, 0, 0, 0, 100, 4, 1000, 5000, 6000, 8000, 11, 38595, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Fear H');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47132;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47132 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47132, 0, 0, 0, 0, 0, 100, 0, 1000, 4000, 3000, 6000, 11, 91522, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Dark Touch');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47141;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47141 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47141, 0, 0, 0, 0, 0, 100, 4, 100, 100, 8000, 12000, 11, 91624, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Enhance Magic'),
(47141, 0, 1, 0, 0, 0, 100, 4, 1000, 1500, 2500, 3000, 11, 91622, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Arcane Barrage'),
(47141, 0, 2, 0, 0, 0, 100, 4, 3000, 6000, 3000, 6000, 11, 91623, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Explosion'),
(47141, 0, 3, 0, 0, 0, 100, 2, 1500, 3000, 10000, 12000, 11, 12611, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cone of Cold'),
(47141, 0, 4, 0, 0, 0, 100, 2, 2500, 4000, 5000, 8000, 11, 33833, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Arcane Missiles');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47135;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47135 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47135, 0, 0, 0, 0, 0, 100, 2, 1000, 5000, 10000, 15000, 11, 91561, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Disease Cloud'),
(47135, 0, 1, 0, 6, 0, 100, 4, 0, 0, 0, 0, 11, 91554, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Fetid Cloud');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47146;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47146 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47146, 0, 0, 0, 0, 0, 100, 0, 2000, 4000, 4000, 8000, 11, 32731, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Flay');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47138;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47138 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47138, 0, 0, 0, 0, 0, 100, 0, 2000, 6000, 9000, 13000, 11, 91677, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Pustulant Spit');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47232;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47232 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47232, 0, 0, 0, 0, 0, 100, 0, 4000, 6000, 5000, 8000, 11, 15496, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cleave'),
(47232, 0, 1, 0, 0, 0, 100, 4, 2000, 4000, 2500, 5000, 11, 91348, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Tenderize');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47134;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47134 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47134, 0, 0, 0, 0, 0, 100, 0, 2000, 6000, 6000, 8000, 11, 91088, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Consume Flesh'),
(47134, 0, 1, 0, 0, 0, 100, 4, 4000, 6000, 8000, 11000, 11, 91099, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Rotting Bile');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47143;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47143 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47143, 0, 0, 0, 0, 0, 100, 0, 5000, 7500, 10000, 15000, 11, 91419, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Bone Flurry'),
(47143, 0, 1, 0, 0, 0, 100, 4, 2000, 4000, 4000, 6000, 11, 91426, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Penetrating Strike');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 3875;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=3875 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(3875, 0, 0, 0, 0, 0, 100, 0, 2000, 4000, 10000, 12000, 11, 7057, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Haunting Spirits'),
(3875, 0, 1, 0, 0, 0, 100, 4, 4000, 8000, 8000, 15000, 11, 91291, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Drain Life');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47137;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47137 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47137, 0, 0, 0, 0, 0, 100, 0, 2000, 8000, 8000, 10000, 11, 91213, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Smash'),
(47137, 0, 1, 0, 0, 0, 100, 4, 5000, 8000, 8000, 12000, 11, 91220, 4, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cowering Roar');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47131;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47131 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47131, 0, 0, 0, 0, 0, 100, 0, 3000, 5000, 4000, 6000, 11, 91064, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Slashing Claws'),
(47131, 0, 1, 0, 0, 0, 100, 5, 500, 1000, 0, 0, 11, 91077, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Slashing Claws - Initial'),
(47131, 0, 2, 0, 0, 0, 100, 4, 14000, 16000, 8000, 10000, 11, 91077, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Slashing Claws - Repeat');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES (-91629, 91631, 'SFK trash');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47145;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47145 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47145, 0, 0, 0, 0, 0, 100, 0, 2000, 5000, 4000, 7000, 11, 87081, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Strike'),
(47145, 0, 1, 0, 0, 0, 100, 0, 4000, 6000, 5000, 8000, 11, 11972, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Shield Bash'),
(47145, 0, 2, 0, 0, 0, 100, 4, 4000, 6000, 20000, 25000, 11, 91629, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Shield of Bones');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47231;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47231 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47231, 0, 0, 0, 0, 0, 100, 2, 5000, 7000, 12000, 12000, 11, 12739, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Shadow Bolt'),
(47231, 0, 1, 0, 0, 0, 100, 4, 4000, 8000, 12000, 15000, 11, 91325, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Shadow Vortex'),
(47231, 0, 2, 0, 0, 0, 100, 4, 2000, 4000, 12000, 15000, 11, 91325, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Shadow Vortex');
UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 47140;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=47140 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47140, 0, 0, 0, 0, 0, 100, 2, 2000, 4000, 8000, 12000, 11, 13341, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Fire Blast N'),
(47140, 0, 1, 0, 0, 0, 100, 4, 2000, 4000, 8000, 12000, 11, 90840, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Fire Blast H'),
(47140, 0, 2, 0, 0, 0, 100, 2, 500, 500, 3000, 4000, 11, 15497, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Frostbolt N'),
(47140, 0, 3, 0, 0, 0, 100, 4, 500, 500, 3000, 4000, 11, 76067, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Frostbolt H'),
(47140, 0, 4, 0, 0, 0, 100, 4, 5000, 8000, 6000, 11000, 11, 91563, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Living Bomb');
-- TotT
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` IN (48571,40597,48983,45469,44715,44648,44841,44752,40633,44404);
DELETE FROM `smart_scripts` WHERE (`entryorguid`=48571 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(48571, 0, 0, 0, 1, 0, 100, 1, 100, 100, 0, 0, 46, 40, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Select Random and Move Forward'),
(48571, 0, 1, 0, 1, 0, 100, 0, 100, 100, 200, 200, 11, 90440, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Select Player and Enter Vehicle'),
(48571, 0, 2, 0, 1, 0, 100, 1, 10000, 10000, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Die');
DELETE FROM `creature_template_addon` WHERE (`entry`=48983);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES (48983, 0, 0, 0, 0, 0, '75699 0');
DELETE FROM `creature_template_addon` WHERE (`entry`=40597);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES (40597, 0, 0, 0, 0, 0, '75699 0');
DELETE FROM `smart_scripts` WHERE (`entryorguid`=48983 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(48983, 0, 0, 0, 1, 0, 100, 0, 3000, 3000, 1000, 1000, 11, 91350, 0, 0, 0, 0, 0, 18, 3, 0, 0, 0, 0, 0, 0, 'Kick');
DELETE FROM `smart_scripts` WHERE (`entryorguid`=40597 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(40597, 0, 0, 0, 1, 0, 100, 0, 3000, 3000, 1000, 1000, 11, 91350, 0, 0, 0, 0, 0, 18, 3, 0, 0, 0, 0, 0, 0, 'Kick');
UPDATE `creature_template` SET `minlevel` = 85, `maxlevel` = 85 WHERE `entry` IN (48571,40597,48983);
DELETE FROM `smart_scripts` WHERE (`entryorguid`=45469 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(45469, 0, 0, 0, 1, 0, 100, 1, 100, 100, 0, 0, 46, 40, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Move in player direction'),
(45469, 0, 1, 0, 1, 0, 100, 1, 6000, 6000, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Die');
UPDATE `creature_template` SET `unit_flags`=0, `dynamicflags`=8, `type_flags`=0, `VehicleId`=648 WHERE `entry`=48571 LIMIT 1;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=44715 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(44715, 0, 0, 0, 0, 0, 100, 0, 3000, 5000, 3000, 4000, 11, 83914, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, ''),
(44715, 0, 1, 0, 0, 0, 100, 0, 5000, 8000, 8000, 10000, 11, 83915, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, ''),
(44715, 0, 2, 0, 0, 0, 100, 4, 6000, 10000, 5000, 12000, 11, 83926, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, '');
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES (83929, 83981, 'TotT');
DELETE FROM `smart_scripts` WHERE (`entryorguid`=44648 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(44648, 0, 0, 0, 0, 0, 100, 0, 5000, 10000, 5000, 12000, 11, 83929, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, ''),
(44648, 0, 1, 0, 0, 0, 100, 0, 5000, 10000, 5000, 8000, 11, 83985, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, '');
DELETE FROM `smart_scripts` WHERE (`entryorguid`=44841 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(44841, 0, 0, 0, 0, 0, 100, 5, 100, 100, 0, 0, 11, 83970, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');
DELETE FROM `smart_scripts` WHERE (`entryorguid`=44752 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(44752, 0, 0, 0, 0, 0, 100, 1, 100, 100, 0, 0, 11, 83463, 0, 0, 0, 0, 0, 11, 40792, 40, 0, 0, 0, 0, 0, ''),
(44752, 0, 1, 0, 1, 0, 100, 1, 100, 100, 0, 0, 11, 83463, 0, 0, 0, 0, 0, 11, 40792, 40, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `mechanic_immune_mask` = 803160063 WHERE `entry` = 44752;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=40633 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(40633, 0, 0, 0, 0, 0, 100, 0, 6000, 6000, 6000, 6000, 11, 75907, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Arc Slash'),
(40633, 0, 1, 0, 0, 0, 100, 0, 15000, 15000, 15000, 15000, 11, 75998, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Enrage');
DELETE FROM `smart_scripts` WHERE (`entryorguid`=44404 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(44404, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 9000, 15000, 11, 75992, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Lightning Surge'),
(44404, 0, 1, 0, 0, 0, 100, 0, 6000, 6000, 5000, 8000, 11, 75813, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Chain Lightning');
update creature set spawntimesecs = 86400, spawnMask = 3 WHERE map = 643;
update gameobject set spawntimesecs = 86400, spawnMask = 3 WHERE map = 643;
UPDATE creature_template SET faction_A = 14, faction_H = 14 WHERE entry IN (44658,44715,44648,44841,44752);

DELETE FROM `creature_template_addon` WHERE (`entry`=40597);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES (40597, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `faction_A` = 14, `faction_H` = 14, `flags_extra` = 130, `ScriptName` = 'npc_geyser' WHERE `entry` = 40597;
DELETE FROM `creature_template_addon` WHERE (`entry`=`command`);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES (48983, 0, 0, 0, 0, 0, '');
UPDATE `creature_template` SET `faction_A` = 14, `faction_H` = 14, `flags_extra` = 130, `ScriptName` = 'npc_geyser' WHERE `entry` = 48983;
UPDATE `creature_template` SET `unit_flags` = 4 WHERE `entry` = 40792;
REPLACE INTO `areatrigger_teleport` (`id`, `name`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES (6201, 'Throne of the Tides (Enterance)', 643, -624.288, 806.302, 246.351, 0);
REPLACE INTO `areatrigger_teleport` (`id`, `name`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES (6200, 'Abyssal Maw - Teleport to Throne of the Tides Entrance', 0, -5621.06, 5468.73, -1801, 1);
REPLACE INTO `areatrigger_teleport` (`id`, `name`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES (6197, 'Throne of the Tides (Exit)', 0, -5571.15, 5552.86, -1793.53, 4.02);
