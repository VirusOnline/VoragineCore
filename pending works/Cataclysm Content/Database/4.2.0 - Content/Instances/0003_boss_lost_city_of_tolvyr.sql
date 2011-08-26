-- General Husam
UPDATE `creature_template` SET `ScriptName`='boss_general_husam' WHERE `entry`=44577; 
UPDATE `creature_template` SET `ScriptName`='npc_trap' WHERE `entry`=44840; 
REPLACE INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`) VALUES (4004, 83454, 90029); -- SPELL_SHOCKWAVE_DMG
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES (83445, 83454, 'Husam Shockwave DMG');
-- High Prophet Barim
UPDATE `creature_template` SET `ScriptName`='boss_high_prophet_barim' WHERE `entry`=43612; 
UPDATE `creature_template` SET `ScriptName`='npc_harbinger_of_darkness' WHERE `entry`=43927; 
UPDATE `creature_template` SET `ScriptName`='npc_blaze_of_heavens' WHERE `entry`=48906; 
UPDATE `creature_template` SET `ScriptName`='npc_soul_fragment' WHERE `entry`=43934; 
REPLACE INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`) VALUES (4005, 81942, 90040); -- SPELL_HEAVENS_FURY_DMG
REPLACE INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (82320, 81947, 2, 'High Propher Barim SPELL_REPENTANCE');
-- Lockmaw & Augh
UPDATE creature_template SET ScriptName = "boss_lockmaw" WHERE entry = 43614;
UPDATE creature_template SET ScriptName = "npc_augh_scent" WHERE entry = 45379;
UPDATE creature_template SET ScriptName = "npc_augh_whirlwind" WHERE entry = 45378;
-- Texts
REPLACE INTO `script_texts` (`npc_entry`, `entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES 
(44577, -1755001, 'Invaders, you shall go no further!', 21886, 1, 0, 0, 'General Husam SAY_AGGRO'),
(44577, -1755002, 'Murkash!', 21887, 1, 0, 0, 'General Husam SAY_SHOCKWAVE'),
(44577, -1755003, 'Tread Lightly.', 21888, 1, 0, 0, 'General Husam SAY_DETONATE'),
(44577, -1755004, 'Siamat must not be freed! Turn back before it is too late!', 21885, 1, 0, 0, 'General Husam SAY_DEATH'),
(43612, -1755005, 'Begone infidels, you are not welcome here!', 19735, 1, 0, 0, 'High Prophet Barim SAY_AGGRO'),
(43612, -1755006, 'Kneel before me and repent!', 19737, 1, 0, 0, 'High Prophet Barim SAY_REPETANCE_1'),
(43612, -1755007, 'The heavens take you!', 19736, 1, 0, 0, 'High Prophet Barim SAY_REPETANCE_2'),
(43612, -1755008, 'Death is only the beginning!', 19733, 1, 0, 0, 'High Prophet Barim SAY_DEATH'),
(43612, -1755009, 'May peace find you now.', 19738, 1, 0, 0, 'High Prophet Barim SAY_KILL');

DELETE FROM `creature_text` WHERE `entry` IN (45378,45379);
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(45379,0,0, 'Bwagauugh!!! Augh feed you to da croc!!!',0,0,0,0,0,0, 'Augh - SAY_SCENT'),
(45378,0,0, 'Gwaaaaaaaaaaaahhh!!!',0,0,0,0,0,0, 'Augh - SAY_WHIRLWIND');

-- Siamat

DELETE FROM `creature_text` WHERE `entry` = 44819;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`)
VALUES
('44819', '1', '0', 'Winds of the south, rise and come to your masters aid!', '1', '0', '0', '0', '0', '20227', 'VO_TV_Siamat_Engage01'),
('44819', '0', '0', 'I. AM. UNLEASHED!', '1', '0', '0', '0', '0', '20231', 'VO_TV_Siamat_Intro01'),
('44819', '2', '1', 'Suffer the storm!', '1', '0', '0', '0', '0', '20229', 'VO_TV_Siamat_Event02'),
('44819', '3', '0', 'Nothing more than dust in the wind.', '1', '0', '0', '0', '0', '20232', 'VO_TV_Siamat_Kill01'),
('44819', '4', '0', 'The sky... Beckons...', '1', '0', '0', '0', '0', '20226', 'VO_TV_Siamat_Death01'),
('44819', '2', '0', 'Cower before the tempest storm!', '1', '0', '0', '0', '0', '20228', 'VO_TV_Siamat_Event01'),
('44819', '2', '2', 'Your city will be buried! Your lives forfeit to the elements! ', '1', '0', '0', '0', '0', '20230', 'VO_TV_Siamat_Event03');
  

UPDATE `creature` SET `spawnMask` = 3 WHERE `map` = 755;
UPDATE `gameobject` SET `spawnMask` = 3 WHERE `map` = 755;
DELETE FROM `creature` WHERE `id` IN (44796, 44711, 44712);
UPDATE `creature_template` SET `dmg_multiplier` = 1, `flags_extra` = 2, AIName = '' WHERE `entry` IN (45379,45378);
DELETE FROM `creature_template_addon` WHERE `entry`=45378;
UPDATE `creature_template` SET `dmg_multiplier` = 13 WHERE `entry` = 43614;
UPDATE `creature_template` SET `dmg_multiplier` = 27 WHERE `entry` = 4361400;
UPDATE `creature_template` SET `dmg_multiplier` = 4 WHERE `entry` = 43658;
UPDATE `creature_template` SET `dmg_multiplier` = 7.5 WHERE `entry` = 4365800;
UPDATE `creature_template` SET `dmg_multiplier` = 27, `AIName` = 'SmartAI' WHERE `entry` = 49045;
DELETE FROM `smart_scripts` WHERE (`entryorguid`=43658 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(43658, 0, 0, 0, 0, 0, 100, 0, 4000, 6000, 5000, 7500, 11, 81677, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, ' ');
DELETE FROM `smart_scripts` WHERE (`entryorguid`=49045 AND `source_type`=0);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(49045, 0, 0, 0, 0, 0, 100, 1, 100, 100, 0, 0, 11, 91415, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Augh - Frenzy'),
(49045, 0, 1, 0, 0, 0, 100, 0, 8000, 10000, 20000, 20000, 11, 91408, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Augh - Whirlwind'),
(49045, 0, 2, 0, 0, 0, 100, 0, 20000, 20000, 30000, 30000, 11, 90026, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Augh - Dragon\'s Breath'),
(49045, 0, 3, 0, 0, 0, 100, 0, 19000, 19000, 25000, 30000, 11, 90026, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Augh - Paralytic Blow Dart');

DELETE FROM creature WHERE id IN (43655,48906,43927,43934,43926,43801,43658,49045,45379,45378);
DELETE FROM gameobject WHERE id IN (2561) and map = 755;
UPDATE `creature_template` SET `unit_flags` = 6, `flags_extra` = 130 WHERE `entry` = 43650;
DELETE FROM `creature_template_addon` WHERE (`entry`=43650);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES (43650, 0, 0, 0, 1, 0, '81646 0 ');
UPDATE `creature_template` SET `unit_flags` = 6, `flags_extra` = 130 WHERE `entry` = 43655;
DELETE FROM `creature_template_addon` WHERE (`entry`=43655);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES (43655, 0, 0, 0, 1, 0, '81646 0 ');
UPDATE `creature_template` SET `lootid` = 0 WHERE `entry` = 4361400;