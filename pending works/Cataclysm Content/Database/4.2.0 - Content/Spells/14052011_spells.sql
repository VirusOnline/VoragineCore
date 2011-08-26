DELETE FROM `spell_proc_event` WHERE `entry`=54936 LIMIT 1;
DELETE FROM `spell_proc_event` WHERE `entry` = 31801;
UPDATE `spell_proc_event` SET `procEx`=`procEx`|0x00000030 WHERE `entry` IN (31244,31245);
UPDATE `spell_proc_event` SET `SpellFamilyMask0`=`SpellFamilyMask0`|0x00000001 WHERE `entry` IN (66192,66191,65661);
DELETE FROM `spell_proc_event` WHERE `entry` = 20784;
INSERT INTO `spell_proc_event` VALUES
(20784,0x00,0,0x00000000,0x00000000,0x00000000,0x00000000,0x00000002,0,0,0);
DELETE FROM `spell_proc_event` WHERE `entry` = 46916;
UPDATE `spell_proc_event` SET `procEx` = 0x0010000 WHERE `entry` = 16864;
DELETE FROM `spell_proc_event` WHERE `entry` IN (49004,49508,49509);
INSERT INTO `spell_proc_event` VALUES
(49004,0x00,0,0x00000000,0x00000000,0x00000000,0x00000000,0x00000033,0,0,0),
(49508,0x00,0,0x00000000,0x00000000,0x00000000,0x00000000,0x00000033,0,0,0),
(49509,0x00,0,0x00000000,0x00000000,0x00000000,0x00000000,0x00000033,0,0,0);
UPDATE `spell_proc_event` SET `procEx` = 0, `ppmRate` = 1 WHERE `entry` = 51123;
UPDATE `spell_proc_event` SET `procEx` = 0, `ppmRate` = 3 WHERE `entry` = 51127;
UPDATE `spell_proc_event` SET `procEx` = 0, `ppmRate` = 5 WHERE `entry` = 51128;
DELETE FROM `conditions` WHERE `SourceEntry` = 56453;
INSERT INTO `conditions` VALUES
(17,0,56453,0,11,67544,0,0,0,'','Lock and Load - Lock and Load Marker');
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 56453;
INSERT INTO `spell_linked_spell` VALUES
(56453,67544,0,'Lock and Load Marker');
DELETE FROM `spell_proc_event` WHERE `entry` IN (20335,20336,20337);
