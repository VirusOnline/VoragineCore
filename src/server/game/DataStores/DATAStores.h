/*
 * Copyright (C) 2008-2011 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2011 VoragineCore <http://www.projectvoragine.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef TRINITY_DATASTORES_H
#define TRINITY_DATASTORES_H

#include "Common.h"
#include "Storages.h"
#include "DATAStructure.h"

#include <list>

typedef std::list<uint32> SimpleFactionsList;

SimpleFactionsList const* GetFactionTeamList(uint32 faction);
const char* GetPetName(uint32 petfamily, uint32 dbclang);
uint32 GetTalentSpellCost(uint32 spellId);
TalentSpellPos const* GetTalentSpellPos(uint32 spellId);
SpellEffectEntry const* GetSpellEffectEntry(uint32 spellId, uint32 effect);
SpellReagentsEntry const* GetSpellReagentEntry(uint32 spellId, uint8 reagent);

int32 GetAreaFlagByAreaID(uint32 area_id);                  // -1 if not found
MapEntry const* GetMapEntry(uint32 map_id);
AreaTableEntry const* GetAreaEntryByAreaID(uint32 area_id);
AreaTableEntry const* GetAreaEntryByAreaFlagAndMap(uint32 area_flag,uint32 map_id);
uint32 GetAreaFlagByMapId(uint32 mapid);

WMOAreaTableEntry const* GetWMOAreaTableEntryByTripple(int32 rootid, int32 adtid, int32 groupid);

uint32 GetVirtualMapForMapAndZone(uint32 mapid, uint32 zoneId);

float GetGtSpellScalingValue(int8 class_, uint8 level);

enum ContentLevels
{
    CONTENT_1_60 = 0,
    CONTENT_61_70,
    CONTENT_71_80,
    CONTENT_81_85
};
ContentLevels GetContentLevelsForMapAndZone(uint32 mapid, uint32 zoneId);

bool IsTotemCategoryCompatiableWith(uint32 itemTotemCategoryId, uint32 requiredTotemCategoryId);

void Zone2MapCoordinates(float &x, float &y, uint32 zone);
void Map2ZoneCoordinates(float &x, float &y, uint32 zone);

typedef std::map<uint32/*pair32(map,diff)*/,MapDifficulty> MapDifficultyMap;
MapDifficulty const* GetMapDifficultyData(uint32 mapId, Difficulty difficulty);
MapDifficulty const* GetDownscaledMapDifficultyData(uint32 mapId, Difficulty &difficulty);

uint32 const* /*[MAX_TALENT_TABS]*/ GetTalentTabPages(uint8 cls);

PvPDifficultyEntry const* GetBattlegroundBracketByLevel(uint32 mapid, uint32 level);
PvPDifficultyEntry const* GetBattlegroundBracketById(uint32 mapid, BattlegroundBracketId id);


/*extern DATAStorage <GtOCTRegenHPEntry>                sGtOCTRegenHPStore;
extern DATAStorage <GtOCTRegenMPEntry>                sGtOCTRegenMPStore; -- not used currently
extern DATAStorage <GtRegenHPPerSptEntry>             sGtRegenHPPerSptStore;
extern DATAStorage <ItemDisplayInfoEntry>             sItemDisplayInfoStore; -- not used currently
extern DATAStorage <WorldMapAreaEntry>                sWorldMapAreaStore; -- use Zone2MapCoordinates and Map2ZoneCoordinates*/
extern DATAStorage <AchievementEntry>                 sAchievementStore;
extern DATAStorage <AchievementCriteriaEntry>         sAchievementCriteriaStore;
extern DATAStorage <AreaTableEntry>                   sAreaStore;// recommend access using functions
extern DATAStorage <AreaGroupEntry>                   sAreaGroupStore;
extern DATAStorage <AreaPOIEntry>                     sAreaPOIStore;
extern DATAStorage <AreaTriggerEntry>                 sAreaTriggerStore;
extern DATAStorage <ArmorLocationEntry>               sArmorLocationStore;
extern DATAStorage <AuctionHouseEntry>                sAuctionHouseStore;
extern DATAStorage <BankBagSlotPricesEntry>           sBankBagSlotPricesStore;
extern DATAStorage <BarberShopStyleEntry>             sBarberShopStyleStore;
extern DATAStorage <BattlemasterListEntry>            sBattlemasterListStore;
extern DATAStorage <ChatChannelsEntry>                sChatChannelsStore;
extern DATAStorage <CharStartOutfitEntry>             sCharStartOutfitStore;
extern DATAStorage <CharTitlesEntry>                  sCharTitlesStore;
extern DATAStorage <ChrClassesEntry>                  sChrClassesStore;
extern DATAStorage <ChrRacesEntry>                    sChrRacesStore;
extern DATAStorage <CinematicSequencesEntry>          sCinematicSequencesStore;
extern DATAStorage <CreatureDisplayInfoEntry>         sCreatureDisplayInfoStore;
extern DATAStorage <CreatureFamilyEntry>              sCreatureFamilyStore;
extern DATAStorage <CreatureSpellDataEntry>           sCreatureSpellDataStore;
extern DATAStorage <CreatureTypeEntry>                sCreatureTypeStore;
extern DATAStorage <CurrencyTypesEntry>               sCurrencyTypesStore;
extern DATAStorage <DungeonEncounterEntry>            sDungeonEncounterStore;
extern DATAStorage <DestructibleModelDataEntry>       sDestructibleModelDataStore;
extern DATAStorage <DurabilityCostsEntry>             sDurabilityCostsStore;
extern DATAStorage <DurabilityQualityEntry>           sDurabilityQualityStore;
extern DATAStorage <EmotesEntry>                      sEmotesStore;
extern DATAStorage <EmotesTextEntry>                  sEmotesTextStore;
extern DATAStorage <FactionEntry>                     sFactionStore;
extern DATAStorage <FactionTemplateEntry>             sFactionTemplateStore;
extern DATAStorage <GameObjectDisplayInfoEntry>       sGameObjectDisplayInfoStore;
extern DATAStorage <GemPropertiesEntry>               sGemPropertiesStore;
extern DATAStorage <GlyphPropertiesEntry>             sGlyphPropertiesStore;
extern DATAStorage <GlyphSlotEntry>                   sGlyphSlotStore;

extern DATAStorage <GtBarberShopCostBaseEntry>        sGtBarberShopCostBaseStore;
extern DATAStorage <GtCombatRatingsEntry>             sGtCombatRatingsStore;
extern DATAStorage <GtChanceToMeleeCritBaseEntry>     sGtChanceToMeleeCritBaseStore;
extern DATAStorage <GtChanceToMeleeCritEntry>         sGtChanceToMeleeCritStore;
extern DATAStorage <GtChanceToSpellCritBaseEntry>     sGtChanceToSpellCritBaseStore;
extern DATAStorage <GtChanceToSpellCritEntry>         sGtChanceToSpellCritStore;
extern DATAStorage <GtOCTClassCombatRatingScalarEntry>sGtOCTClassCombatRatingScalarStore;
extern DATAStorage <GtRegenMPPerSptEntry>             sGtRegenMPPerSptStore;
extern DATAStorage <GtOCTHpPerStaminaEntry>           sGtOCTHpPerStaminaStore;
extern DATAStorage <gtSpellScaling>                   sGtSpellScalingStore;
extern DATAStorage <HolidaysEntry>                    sHolidaysStore;
extern DATAStorage <ItemArmorQualityEntry>            sItemArmorQualityStore;
extern DATAStorage <ItemArmorShieldEntry>             sItemArmorShieldStore;
extern DATAStorage <ItemArmorTotalEntry>              sItemArmorTotalStore;
extern DATAStorage <ItemBagFamilyEntry>               sItemBagFamilyStore;
extern DATAStorage <ItemDamageEntry>                  sItemDamageAmmoStore;
extern DATAStorage <ItemDamageEntry>                  sItemDamageOneHandStore;
extern DATAStorage <ItemDamageEntry>                  sItemDamageOneHandCasterStore;
extern DATAStorage <ItemDamageEntry>                  sItemDamageRangedStore;
extern DATAStorage <ItemDamageEntry>                  sItemDamageThrownStore;
extern DATAStorage <ItemDamageEntry>                  sItemDamageTwoHandStore;
extern DATAStorage <ItemDamageEntry>                  sItemDamageTwoHandCasterStore;
extern DATAStorage <ItemDamageEntry>                  sItemDamageWandStore;
extern DATAStorage <ItemLimitCategoryEntry>           sItemLimitCategoryStore;
extern DATAStorage <ItemRandomPropertiesEntry>        sItemRandomPropertiesStore;
extern DATAStorage <ItemRandomSuffixEntry>            sItemRandomSuffixStore;
extern DATAStorage <ItemSetEntry>                     sItemSetStore;
extern DATAStorage <LFGDungeonEntry>                  sLFGDungeonStore;
extern DATAStorage <LockEntry>                        sLockStore;
extern DATAStorage <MailTemplateEntry>                sMailTemplateStore;
extern DATAStorage <MapEntry>                         sMapStore;
extern DATAStorage <PhaseEntry>                       sPhaseStore;
//extern DATAStorage <MapDifficultyEntry>               sMapDifficultyStore; -- use GetMapDifficultyData insteed
extern MapDifficultyMap                              sMapDifficultyMap;
extern DATAStorage <MovieEntry>                       sMovieStore;
extern DATAStorage <MountCapabilityEntry>             sMountCapabilityStore;
extern DATAStorage <MountTypeEntry>                   sMountTypeStore;
extern DATAStorage <OverrideSpellDataEntry>           sOverrideSpellDataStore;
extern DATAStorage <QuestSortEntry>                   sQuestSortStore;
extern DATAStorage <QuestXPEntry>                     sQuestXPStore;
extern DATAStorage <QuestFactionRewEntry>             sQuestFactionRewardStore;
extern DATAStorage <RandomPropertiesPointsEntry>      sRandomPropertiesPointsStore;
extern DATAStorage <ScalingStatDistributionEntry>     sScalingStatDistributionStore;
extern DATAStorage <ScalingStatValuesEntry>           sScalingStatValuesStore;
extern DATAStorage <SkillLineEntry>                   sSkillLineStore;
extern DATAStorage <SkillLineAbilityEntry>            sSkillLineAbilityStore;
extern DATAStorage <SoundEntriesEntry>                sSoundEntriesStore;
extern DATAStorage <SpellCastTimesEntry>              sSpellCastTimesStore;
extern DATAStorage <SpellDifficultyEntry>             sSpellDifficultyStore;
extern DATAStorage <SpellDurationEntry>               sSpellDurationStore;
extern DATAStorage <SpellFocusObjectEntry>            sSpellFocusObjectStore;
extern DATAStorage <SpellItemEnchantmentEntry>        sSpellItemEnchantmentStore;
extern DATAStorage <SpellItemEnchantmentConditionEntry>sSpellItemEnchantmentConditionStore;
extern SpellCategoryStore                            sSpellCategoryStore;
extern PetFamilySpellsStore                          sPetFamilySpellsStore;
extern DATAStorage <SpellRadiusEntry>                 sSpellRadiusStore;
extern DATAStorage <SpellRangeEntry>                  sSpellRangeStore;
extern DATAStorage <SpellRuneCostEntry>               sSpellRuneCostStore;
extern DATAStorage <SpellShapeshiftFormEntry>         sSpellShapeshiftFormStore;
extern DATAStorage <SpellEntry>                       sSpellStore;
extern DATAStorage <SpellEntry_n>                     sTrueSpellStore;
extern DATAStorage <SpellAuraOptionsEntry>            sSpellAuraOptionsStore;
extern DATAStorage <SpellAuraRestrictionsEntry>       sSpellAuraRestrictionsStore;
extern DATAStorage <SpellCastingRequirementsEntry>    sSpellCastingRequirementsStore;
extern DATAStorage <SpellCategoriesEntry>             sSpellCategoriesStore;
extern DATAStorage <SpellClassOptionsEntry>           sSpellClassOptionsStore;
extern DATAStorage <SpellCooldownsEntry>              sSpellCooldownsStore;
extern DATAStorage <SpellEffectEntry>                 sSpellEffectStore;
extern DATAStorage <SpellEquippedItemsEntry>          sSpellEquippedItemsStore;
extern DATAStorage <SpellInterruptsEntry>             sSpellInterruptsStore;
extern DATAStorage <SpellLevelsEntry>                 sSpellLevelsStore;
extern DATAStorage <SpellPowerEntry>                  sSpellPowerStore;
extern DATAStorage <SpellReagentsEntry>               sSpellReagentsStore;
extern DATAStorage <SpellScalingEntry>                sSpellScalingStore;
extern DATAStorage <SpellShapeshiftEntry>             sSpellShapeshiftStore;
extern DATAStorage <SpellTargetRestrictionsEntry>     sSpellTargetRestrictionsStore;
extern DATAStorage <SpellTotemsEntry>                 sSpellTotemsStore;
extern DATAStorage <SummonPropertiesEntry>            sSummonPropertiesStore;
extern DATAStorage <GuildPerksEntry>                  sGuildPerksStore;
extern DATAStorage <TalentEntry>                      sTalentStore;
extern DATAStorage <TalentTabEntry>                   sTalentTabStore;
extern DATAStorage <TalentTreePrimarySpellsEntry>     sTalentTreePrimarySpellsStore;
extern DATAStorage <TaxiNodesEntry>                   sTaxiNodesStore;
extern DATAStorage <TaxiPathEntry>                    sTaxiPathStore;
extern TaxiMask                                      sTaxiNodesMask;
extern TaxiMask                                      sOldContinentsNodesMask;
extern TaxiMask                                      sHordeTaxiNodesMask;
extern TaxiMask                                      sAllianceTaxiNodesMask;
extern TaxiMask                                      sDeathKnightTaxiNodesMask;
extern TaxiPathSetBySource                           sTaxiPathSetBySource;
extern TaxiPathNodesByPath                           sTaxiPathNodesByPath;
extern DATAStorage <TotemCategoryEntry>               sTotemCategoryStore;
extern DATAStorage <VehicleEntry>                     sVehicleStore;
extern DATAStorage <VehicleSeatEntry>                 sVehicleSeatStore;
extern DATAStorage <WMOAreaTableEntry>                sWMOAreaTableStore;
extern DATAStorage <WorldMapOverlayEntry>             sWorldMapOverlayStore;
extern DATAStorage <WorldSafeLocsEntry>               sWorldSafeLocsStore;
extern DATAStorage <ItemEntry>                        sItemStore;
extern DATAStorage <ItemCurrencyCostEntry>            sItemCurrencyCostStore;
extern DATAStorage <ItemExtendedCostEntry>            sItemExtendedCostStore;

void LoadDATAStores();

// script support functions
 DATAStorage <SoundEntriesEntry>                      const* GetSoundEntriesStore();
 DATAStorage <SpellEntry>                             const* GetSpellStore();
 DATAStorage <SpellRangeEntry>                        const* GetSpellRangeStore();
 DATAStorage <FactionEntry>                           const* GetFactionStore();
 DATAStorage <CreatureDisplayInfoEntry>               const* GetCreatureDisplayStore();
 DATAStorage <EmotesEntry>                            const* GetEmotesStore();
 DATAStorage <EmotesTextEntry>                        const* GetEmotesTextStore();
 DATAStorage <AchievementEntry>                       const* GetAchievementStore();
#endif
