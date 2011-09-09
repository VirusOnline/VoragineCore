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

#include "gamePCH.h"
#include "DATAStores.h"

#include "Logging/Log.h"

#include "SharedDefines.h"
#include "SpellMgr.h"

#include "DATAfmt.h"

#include <map>

// NEW DATA FORMAT SYSTEM WILL NOT USE FOR A WHILE..

typedef std::map<uint16,uint32> AreaFlagByAreaID;
typedef std::map<uint32,uint32> AreaFlagByMapID;

struct WMOAreaTableTripple
{
    WMOAreaTableTripple(int32 r, int32 a, int32 g) :  groupId(g), rootId(r), adtId(a)
    {
    }

    bool operator <(const WMOAreaTableTripple& b) const
    {
        return memcmp(this, &b, sizeof(WMOAreaTableTripple))<0;
    }

    // ordered by entropy; that way memcmp will have a minimal medium runtime
    int32 groupId;
    int32 rootId;
    int32 adtId;
};

/*DATAStorage <GtOCTRegenHPEntry>                      sGtOCTRegenHPStore(LookupFormat(GtOCTRegenHPfmt));
DATAStorage <GtOCTRegenMPEntry>                      sGtOCTRegenMPStore(LookupFormat(GtOCTRegenMPfmt));  -- not used currently
DATAStorage <GtRegenHPPerSptEntry>                   sGtRegenHPPerSptStore(LookupFormat(GtRegenHPPerSptfmt));
DATAStorage <ItemDisplayInfoEntry>                   sItemDisplayInfoStore(LookupFormat(ItemDisplayTemplateEntryfmt)); -- not used currently
DATAStorage <ItemCondExtCostsEntry>                  sItemCondExtCostsStore(LookupFormat(ItemCondExtCostsEntryfmt));*/
DATAStorage <AreaTableEntry>                         sAreaStore(LookupFormat(AreaTableEntryfmt));
DATAStorage <AreaGroupEntry>                         sAreaGroupStore(LookupFormat(AreaGroupEntryfmt));
DATAStorage <AreaPOIEntry>                           sAreaPOIStore(LookupFormat(AreaPOIEntryfmt));
DATAStorage <AchievementEntry>                       sAchievementStore(LookupFormat(Achievementfmt));
DATAStorage <AchievementCriteriaEntry>               sAchievementCriteriaStore(LookupFormat(AchievementCriteriafmt));
DATAStorage <AreaTriggerEntry>                       sAreaTriggerStore(LookupFormat(AreaTriggerEntryfmt));
DATAStorage <ArmorLocationEntry>                     sArmorLocationStore(LookupFormat(ArmorLocationfmt));
DATAStorage <AuctionHouseEntry>                      sAuctionHouseStore(LookupFormat(AuctionHouseEntryfmt));
DATAStorage <BankBagSlotPricesEntry>                 sBankBagSlotPricesStore(LookupFormat(BankBagSlotPricesEntryfmt));
DATAStorage <BattlemasterListEntry>                  sBattlemasterListStore(LookupFormat(BattlemasterListEntryfmt));
DATAStorage <BarberShopStyleEntry>                   sBarberShopStyleStore(LookupFormat(BarberShopStyleEntryfmt));
DATAStorage <CharStartOutfitEntry>                   sCharStartOutfitStore(LookupFormat(CharStartOutfitEntryfmt));
DATAStorage <CharTitlesEntry>                        sCharTitlesStore(LookupFormat(CharTitlesEntryfmt));
DATAStorage <ChatChannelsEntry>                      sChatChannelsStore(LookupFormat(ChatChannelsEntryfmt));
DATAStorage <ChrClassesEntry>                        sChrClassesStore(LookupFormat(ChrClassesEntryfmt));
DATAStorage <ChrRacesEntry>                          sChrRacesStore(LookupFormat(ChrRacesEntryfmt));
DATAStorage <CinematicSequencesEntry>                sCinematicSequencesStore(LookupFormat(CinematicSequencesEntryfmt));
DATAStorage <CreatureDisplayInfoEntry>               sCreatureDisplayInfoStore(LookupFormat(CreatureDisplayInfofmt));
DATAStorage <CreatureFamilyEntry>                    sCreatureFamilyStore(LookupFormat(CreatureFamilyfmt));
DATAStorage <CreatureSpellDataEntry>                 sCreatureSpellDataStore(LookupFormat(CreatureSpellDatafmt));
DATAStorage <CreatureTypeEntry>                      sCreatureTypeStore(LookupFormat(CreatureTypefmt));
DATAStorage <CurrencyTypesEntry>                     sCurrencyTypesStore(LookupFormat(CurrencyTypesfmt));
DATAStorage <DestructibleModelDataEntry>             sDestructibleModelDataStore(LookupFormat(DestructibleModelDatafmt));
DATAStorage <DungeonEncounterEntry>                  sDungeonEncounterStore(LookupFormat(DungeonEncounterfmt));
DATAStorage <DurabilityQualityEntry>                 sDurabilityQualityStore(LookupFormat(DurabilityQualityfmt));
DATAStorage <DurabilityCostsEntry>                   sDurabilityCostsStore(LookupFormat(DurabilityCostsfmt));
DATAStorage <EmotesEntry>                            sEmotesStore(LookupFormat(EmotesEntryfmt));
DATAStorage <EmotesTextEntry>                        sEmotesTextStore(LookupFormat(EmotesTextEntryfmt));
DATAStorage <FactionEntry>                           sFactionStore(LookupFormat(FactionEntryfmt));
DATAStorage <FactionTemplateEntry>                   sFactionTemplateStore(LookupFormat(FactionTemplateEntryfmt));
DATAStorage <GameObjectDisplayInfoEntry>             sGameObjectDisplayInfoStore(LookupFormat(GameObjectDisplayInfofmt));
DATAStorage <GemPropertiesEntry>                     sGemPropertiesStore(LookupFormat(GemPropertiesEntryfmt));
DATAStorage <GlyphPropertiesEntry>                   sGlyphPropertiesStore(LookupFormat(GlyphPropertiesfmt));
DATAStorage <GlyphSlotEntry>                         sGlyphSlotStore(LookupFormat(GlyphSlotfmt));
DATAStorage <GtBarberShopCostBaseEntry>              sGtBarberShopCostBaseStore(LookupFormat(GtBarberShopCostBasefmt));
DATAStorage <GtCombatRatingsEntry>                   sGtCombatRatingsStore(LookupFormat(GtCombatRatingsfmt));
DATAStorage <GtChanceToMeleeCritBaseEntry>           sGtChanceToMeleeCritBaseStore(LookupFormat(GtChanceToMeleeCritBasefmt));
DATAStorage <GtChanceToMeleeCritEntry>               sGtChanceToMeleeCritStore(LookupFormat(GtChanceToMeleeCritfmt));
DATAStorage <GtChanceToSpellCritBaseEntry>           sGtChanceToSpellCritBaseStore(LookupFormat(GtChanceToSpellCritBasefmt));
DATAStorage <GtChanceToSpellCritEntry>               sGtChanceToSpellCritStore(LookupFormat(GtChanceToSpellCritfmt));
DATAStorage <GtOCTClassCombatRatingScalarEntry>      sGtOCTClassCombatRatingScalarStore(LookupFormat(GtOCTClassCombatRatingScalarfmt));
DATAStorage <GtRegenMPPerSptEntry>                   sGtRegenMPPerSptStore(LookupFormat(GtRegenMPPerSptfmt));
DATAStorage <GtOCTHpPerStaminaEntry>                 sGtOCTHpPerStaminaStore(LookupFormat(GtOCTHpPerStaminafmt));
DATAStorage <gtSpellScaling>                         sGtSpellScalingStore(LookupFormat(GtSpellScalingfmt));
DATAStorage <HolidaysEntry>                          sHolidaysStore(LookupFormat(Holidaysfmt));
DATAStorage <ItemArmorQualityEntry>                  sItemArmorQualityStore(LookupFormat(ItemArmorQualityfmt));
DATAStorage <ItemArmorShieldEntry>                   sItemArmorShieldStore(LookupFormat(ItemArmorShieldfmt));
DATAStorage <ItemArmorTotalEntry>                    sItemArmorTotalStore(LookupFormat(ItemArmorTotalfmt));
DATAStorage <ItemBagFamilyEntry>                     sItemBagFamilyStore(LookupFormat(ItemBagFamilyfmt));
DATAStorage <ItemDamageEntry>                        sItemDamageAmmoStore(LookupFormat(ItemDamagefmt));
DATAStorage <ItemDamageEntry>                        sItemDamageOneHandStore(LookupFormat(ItemDamagefmt));
DATAStorage <ItemDamageEntry>                        sItemDamageOneHandCasterStore(LookupFormat(ItemDamagefmt));
DATAStorage <ItemDamageEntry>                        sItemDamageRangedStore(LookupFormat(ItemDamagefmt));
DATAStorage <ItemDamageEntry>                        sItemDamageThrownStore(LookupFormat(ItemDamagefmt));
DATAStorage <ItemDamageEntry>                        sItemDamageTwoHandStore(LookupFormat(ItemDamagefmt));
DATAStorage <ItemDamageEntry>                        sItemDamageTwoHandCasterStore(LookupFormat(ItemDamagefmt));
DATAStorage <ItemDamageEntry>                        sItemDamageWandStore(LookupFormat(ItemDamagefmt));
DATAStorage <ItemLimitCategoryEntry>                 sItemLimitCategoryStore(LookupFormat(ItemLimitCategoryEntryfmt));
DATAStorage <ItemRandomPropertiesEntry>              sItemRandomPropertiesStore(LookupFormat(ItemRandomPropertiesfmt));
DATAStorage <ItemRandomSuffixEntry>                  sItemRandomSuffixStore(LookupFormat(ItemRandomSuffixfmt));
DATAStorage <ItemSetEntry>                           sItemSetStore(LookupFormat(ItemSetEntryfmt));
DATAStorage <LFGDungeonEntry>                        sLFGDungeonStore(LookupFormat(LFGDungeonEntryfmt));
DATAStorage <LockEntry>                              sLockStore(LookupFormat(LockEntryfmt));
DATAStorage <MailTemplateEntry>                      sMailTemplateStore(LookupFormat(MailTemplateEntryfmt));
DATAStorage <MapEntry>                               sMapStore(LookupFormat(MapEntryfmt));
// DBC used only for initialization sMapDifficultyMap at startup.
DATAStorage <MapDifficultyEntry>                     sMapDifficultyStore(LookupFormat(MapDifficultyEntryfmt)); // only for loading
DATAStorage <MovieEntry>                             sMovieStore(LookupFormat(MovieEntryfmt));
DATAStorage <MountCapabilityEntry>                   sMountCapabilityStore(LookupFormat(MountCapabilityfmt));
DATAStorage <MountTypeEntry>                         sMountTypeStore(LookupFormat(MountTypefmt));
DATAStorage <OverrideSpellDataEntry>                 sOverrideSpellDataStore(LookupFormat(OverrideSpellDatafmt));
DATAStorage <PvPDifficultyEntry>                     sPvPDifficultyStore(LookupFormat(PvPDifficultyfmt));
DATAStorage <QuestSortEntry>                         sQuestSortStore(LookupFormat(QuestSortEntryfmt));
DATAStorage <QuestXPEntry>                           sQuestXPStore(LookupFormat(QuestXPfmt));
DATAStorage <QuestFactionRewEntry>                   sQuestFactionRewardStore(LookupFormat(QuestFactionRewardfmt));
DATAStorage <RandomPropertiesPointsEntry>            sRandomPropertiesPointsStore(LookupFormat(RandomPropertiesPointsfmt));
DATAStorage <ScalingStatDistributionEntry>           sScalingStatDistributionStore(LookupFormat(ScalingStatDistributionfmt));
DATAStorage <ScalingStatValuesEntry>                 sScalingStatValuesStore(LookupFormat(ScalingStatValuesfmt));
DATAStorage <SkillLineEntry>                         sSkillLineStore(LookupFormat(SkillLinefmt));
DATAStorage <SkillLineAbilityEntry>                  sSkillLineAbilityStore(LookupFormat(SkillLineAbilityfmt));
DATAStorage <SoundEntriesEntry>                      sSoundEntriesStore(LookupFormat(SoundEntriesfmt));
DATAStorage <SpellItemEnchantmentEntry>              sSpellItemEnchantmentStore(LookupFormat(SpellItemEnchantmentfmt));
DATAStorage <SpellItemEnchantmentConditionEntry>     sSpellItemEnchantmentConditionStore(LookupFormat(SpellItemEnchantmentConditionfmt));
DATAStorage <SpellEntry>                             sSpellStore(LookupFormat(True_SpellEntryfmt));
DATAStorage <SpellEntry_n>                           sTrueSpellStore(LookupFormat(SpellEntryfmt));
DATAStorage <SpellAuraOptionsEntry>                  sSpellAuraOptionsStore(LookupFormat(SpellAuraOptionsEntryfmt));
DATAStorage <SpellAuraRestrictionsEntry>             sSpellAuraRestrictionsStore(LookupFormat(SpellAuraRestrictionsEntryfmt));
DATAStorage <SpellCastingRequirementsEntry>          sSpellCastingRequirementsStore(LookupFormat(SpellCastingRequirementsEntryfmt));
DATAStorage <SpellCategoriesEntry>                   sSpellCategoriesStore(LookupFormat(SpellCategoriesEntryfmt));
DATAStorage <SpellClassOptionsEntry>                 sSpellClassOptionsStore(LookupFormat(SpellClassOptionsEntryfmt));
DATAStorage <SpellCooldownsEntry>                    sSpellCooldownsStore(LookupFormat(SpellCooldownsEntryfmt));
DATAStorage <SpellEffectEntry>                       sSpellEffectStore(LookupFormat(SpellEffectEntryfmt));
DATAStorage <SpellEquippedItemsEntry>                sSpellEquippedItemsStore(LookupFormat(SpellEquippedItemsEntryfmt));
DATAStorage <SpellInterruptsEntry>                   sSpellInterruptsStore(LookupFormat(SpellInterruptsEntryfmt));
DATAStorage <SpellLevelsEntry>                       sSpellLevelsStore(LookupFormat(SpellLevelsEntryfmt));
DATAStorage <SpellPowerEntry>                        sSpellPowerStore(LookupFormat(SpellPowerEntryfmt));
DATAStorage <SpellReagentsEntry>                     sSpellReagentsStore(LookupFormat(SpellReagentsEntryfmt));
DATAStorage <SpellScalingEntry>                      sSpellScalingStore(LookupFormat(SpellScalingEntryfmt));
DATAStorage <SpellShapeshiftEntry>                   sSpellShapeshiftStore(LookupFormat(SpellShapeshiftEntryfmt));
DATAStorage <SpellTargetRestrictionsEntry>           sSpellTargetRestrictionsStore(LookupFormat(SpellTargetRestrictionsEntryfmt));
DATAStorage <SpellTotemsEntry>                       sSpellTotemsStore(LookupFormat(SpellTotemsEntryfmt));
DATAStorage <SpellCastTimesEntry>                    sSpellCastTimesStore(LookupFormat(SpellCastTimefmt));
DATAStorage <SpellDifficultyEntry>                   sSpellDifficultyStore(LookupFormat(SpellDifficultyfmt));
DATAStorage <SpellDurationEntry>                     sSpellDurationStore(LookupFormat(SpellDurationfmt));
DATAStorage <SpellFocusObjectEntry>                  sSpellFocusObjectStore(LookupFormat(SpellFocusObjectfmt));
DATAStorage <SpellRadiusEntry>                       sSpellRadiusStore(LookupFormat(SpellRadiusfmt));
DATAStorage <SpellRangeEntry>                        sSpellRangeStore(LookupFormat(SpellRangefmt));
DATAStorage <SpellRuneCostEntry>                     sSpellRuneCostStore(LookupFormat(SpellRuneCostfmt));
DATAStorage <SpellShapeshiftFormEntry>               sSpellShapeshiftFormStore(LookupFormat(SpellShapeshiftFormfmt));
DATAStorage <SummonPropertiesEntry>                  sSummonPropertiesStore(LookupFormat(SummonPropertiesfmt));
DATAStorage <GuildPerksEntry>                        sGuildPerksStore(LookupFormat(GuildPerksfmt));
DATAStorage <TalentEntry>                            sTalentStore(LookupFormat(TalentEntryfmt));
DATAStorage <TalentTabEntry>                         sTalentTabStore(LookupFormat(TalentTabEntryfmt));
DATAStorage <TalentTreePrimarySpellsEntry>           sTalentTreePrimarySpellsStore(LookupFormat(TalentTreePrimarySpellsfmt));
DATAStorage <ItemEntry>                              sItemStore(LookupFormat(Itemfmt));
DATAStorage <ItemCurrencyCostEntry>                  sItemCurrencyCostStore(LookupFormat(ItemCurrencyCostfmt));
DATAStorage <ItemExtendedCostEntry>                  sItemExtendedCostStore(LookupFormat(ItemExtendedCostEntryfmt));

typedef std::map<WMOAreaTableTripple, WMOAreaTableEntry const *> WMOAreaInfoByTripple;
typedef std::map<uint32,SimpleFactionsList>                      FactionTeamMap;

static AreaFlagByAreaID       sAreaFlagByAreaID;
static AreaFlagByMapID        sAreaFlagByMapID;                    // for instances without generated *.map files

static WMOAreaInfoByTripple   sWMOAreaInfoByTripple;

static FactionTeamMap         sFactionTeamMap;

MapDifficultyMap              sMapDifficultyMap;
SpellEffectMap                sSpellEffectMap;
SpellReagentMap               sSpellReagentMap;
TalentSpellPosMap             sTalentSpellPosMap;
SpellCategoryStore            sSpellCategoryStore;
PetFamilySpellsStore          sPetFamilySpellsStore;

// store absolute bit position for first rank for talent inspect
static uint32 sTalentTabPages[MAX_CLASSES][3];

DATAStorage <TaxiNodesEntry>                        sTaxiNodesStore(LookupFormat(TaxiNodesEntryfmt));
TaxiMask sTaxiNodesMask;
TaxiMask sOldContinentsNodesMask;
TaxiMask sHordeTaxiNodesMask;
TaxiMask sAllianceTaxiNodesMask;
TaxiMask sDeathKnightTaxiNodesMask;

// DBC used only for initialization sTaxiPathSetBySource at startup.
TaxiPathSetBySource sTaxiPathSetBySource;
DATAStorage <TaxiPathEntry>                         sTaxiPathStore(LookupFormat(TaxiPathEntryfmt));

// DBC used only for initialization sTaxiPathNodeStore at startup.
TaxiPathNodesByPath sTaxiPathNodesByPath;
static DATAStorage <TaxiPathNodeEntry>              sTaxiPathNodeStore(LookupFormat(TaxiPathNodeEntryfmt));

DATAStorage <TotemCategoryEntry>                    sTotemCategoryStore(LookupFormat(TotemCategoryEntryfmt));
DATAStorage <VehicleEntry>                          sVehicleStore(LookupFormat(VehicleEntryfmt));
DATAStorage <VehicleSeatEntry>                      sVehicleSeatStore(LookupFormat(VehicleSeatEntryfmt));
DATAStorage <WMOAreaTableEntry>                     sWMOAreaTableStore(LookupFormat(WMOAreaTableEntryfmt));
DATAStorage <WorldMapAreaEntry>                     sWorldMapAreaStore(LookupFormat(WorldMapAreaEntryfmt));
DATAStorage <WorldMapOverlayEntry>                  sWorldMapOverlayStore(LookupFormat(WorldMapOverlayEntryfmt));
DATAStorage <WorldSafeLocsEntry>                    sWorldSafeLocsStore(LookupFormat(WorldSafeLocsEntryfmt));
DATAStorage <PhaseEntry>                            sPhaseStores(LookupFormat(PhaseEntryfmt));

typedef std::list<std::string> StoreProblemList;

uint32 DATAFileCount = 0;

static bool LoadData_assert_print(uint32 fsize, uint32 rsize, const std::string& filename)
{
    sLog->outError("Size of '%s' setted by format string (%u) not equal size of C++ structure (%u).", filename.c_str(), fsize, rsize);

    // ASSERT must fail after function call
    return false;
}

template<class T>
inline void LoadDATA(uint32& availableDataLocales, StoreProblemList& errors, DATAStorage<T>& storage, std::string const& dataPath, std::string const& filename, std::string const* customFormat = NULL, std::string const* customIndexName = NULL)
{
    // compatibility format and C++ structure sizes
    if(!(StorageLoader::GetFormatRecordSize(storage.GetFormat()) == sizeof(T) || LoadData_assert_print(StorageLoader::GetFormatRecordSize(storage.GetFormat()), sizeof(T), filename)))
        return;

    ++DATAFileCount;
    std::string dataFilename = dataPath + filename;
    SqlData* sql = NULL;
    if (customFormat)
        sql = new SqlData(&filename, customFormat, customIndexName, storage.GetFormat());

    if (storage.LoadDBCStorage(dataFilename.c_str(), sql) || storage.LoadDB2Storage(dataFilename.c_str(), sql))
    {
        for (uint8 i = 0; i < TOTAL_LOCALES; ++i)
        {
            if (!(availableDataLocales & (1 << i)))
                continue;

            std::string localizedName = dataPath + localeNames[i] + "/" + filename;
            if (!storage.LoadStringsFrom(localizedName.c_str()))
                availableDataLocales &= ~(1<<i);             // mark as not available for speedup next checks
        }
    }
    else
    {
        // sort problematic dbc to (1) non compatible and (2) non-existed
        if (FILE* f = fopen(dataFilename.c_str(), "rb"))
        {
            char buf[100];
            snprintf(buf, 100, " (exists, but has %d fields instead of " SIZEFMTD ") Possible wrong client version.", storage.GetFieldCount(), strlen(storage.GetFormat()));
            errors.push_back(dataFilename + buf);
            fclose(f);
        }
        else
            errors.push_back(dataFilename);
    }

    delete sql;
}

// Correspondence between format formats and their names/handlers
/*FormatHandler formatTable[MAX_FORMAT_TYPES];
char* formatEnumToFormat[LastFormatData];
char const* formatEnumToName[LastFormatData];

typedef UNORDERED_MAP< std::string, std::string> DataFormatsMap;
DataFormatsMap DataFormats;

bool LoadDATAFormats()
{
    uint32 oldMSTime = getMSTime();

    uint16 build = 14480;
    QueryResult result = WorldDatabase.PQuery("SELECT name, format FROM dataformats WHERE `version`=%u", build);

    if (!result)
    {
        sLog->outString();
        sLog->outErrorDb(">> Failed to load data formats from the DB. Cannot continue.");
        return false;
    }

    uint32 count = 0;
    do
    {
        Field *fields = result->Fetch();
        
        std::string name   = fields[0].GetString();
        const char* format = fields[1].GetCString();

        DataFormats[name] = format;
        ++count;
    } while (result->NextRow());

    sLog->outString();
    sLog->outString(">> Loaded %u data formats in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));

    return true;
}
*/
template<class TT>
static void DefineFormat(/*DATAFormats*/const char* enumId, const char* name, uint32& availableDataLocales, StoreProblemList& errlist, DATAStorage<TT>& storage, const std::string& dataPath, const std::string& filename)
{
    /*formatEnumToName[enumId] = name;
    DataFormatsMap::iterator itr = DataFormats.find(std::string(name));
    if (itr != DataFormats.end())
    {
        char *format = new char[itr->second.size() + 1];
        std::strcpy (format, itr->second.c_str());
        formatEnumToFormat[enumId] = format;
        if(format == "")
            return;

        formatTable[enumId].name = name;
        formatTable[enumId].enumValue = enumId;
        DATAStorage <TT> storage(format);
        storage.SetFormat(format);*/
        LoadDATA(availableDataLocales, errlist, storage, dataPath, filename);
    /*}
    else
    {
        sLog->outError("Format not found in the DB %s.", name);
        char *format = "";
        formatEnumToFormat[enumId] = format;
    }*/
}

#define FORMAT( name, availableDataLocales, errlist, storage, dataPath, filename ) DefineFormat( name, #name , availableDataLocales, errlist, storage, dataPath, filename)

void LoadDATAStores()
{
    const std::string& dataPath = sWorld->GetDataPath();
    //LoadDATAFormats();

    uint32 oldMSTime = getMSTime();

    std::string storesPath = dataPath+"dbc/";

    StoreProblemList bad_db2_files;
    StoreProblemList bad_dbc_files;
    uint32 availableDataLocales = 0xFFFFFFFF;

    //DB2 Files
    FORMAT(Itemfmt, availableDataLocales, bad_db2_files, sItemStore,                   storesPath, "Item.db2");

    for (uint32 i = 0; i < sItemStore.GetNumRows(); ++i)
    {
        const ItemEntry* itemEntry = sItemStore.LookupEntry(i);
        if (!itemEntry)
            continue;

        if (itemEntry->Class >= MAX_ITEM_CLASS)
        {
            sLog->outErrorDb("Item (Entry: %u) in Item.db2 has too high class value %u", itemEntry->ID, itemEntry->Class);
            const_cast<ItemEntry*>(itemEntry)->Class = 0;
        }
        if (itemEntry->SubClass >= MaxItemSubclassValues[itemEntry->Class])
        {
            sLog->outErrorDb("Item (Entry: %u) in Item.db2 has too high subclass value %u for class %u", itemEntry->ID, itemEntry->SubClass, itemEntry->Class);
            const_cast<ItemEntry*>(itemEntry)->SubClass = 0;
        }
    }

    FORMAT(ItemCurrencyCostfmt, availableDataLocales , bad_db2_files, sItemCurrencyCostStore,           storesPath, "ItemCurrencyCost.db2");
    FORMAT(ItemExtendedCostEntryfmt, availableDataLocales, bad_db2_files, sItemExtendedCostStore,       storesPath, "ItemExtendedCost.db2");

    //DBC Files
    FORMAT(AreaTableEntryfmt, availableDataLocales,bad_dbc_files, sAreaStore, storesPath, "AreaTable.dbc");
    // must be after sAreaStore loading
    for (uint32 i = 0; i < sAreaStore.GetNumRows(); ++i)           // areaflag numbered from 0
    {
        if (AreaTableEntry const* area = sAreaStore.LookupEntry(i))
        {
            // fill AreaId->DBC records
            sAreaFlagByAreaID.insert(AreaFlagByAreaID::value_type(uint16(area->ID),area->exploreFlag));

            // fill MapId->DBC records (skip sub zones and continents)
            if (area->zone == 0 && area->mapid != 0 && area->mapid != 1 && area->mapid != 530 && area->mapid != 571)
                sAreaFlagByMapID.insert(AreaFlagByMapID::value_type(area->mapid,area->exploreFlag));
        }
    }

    FORMAT(Achievementfmt, availableDataLocales,bad_dbc_files,sAchievementStore,              storesPath,"Achievement.dbc");
    FORMAT(AchievementCriteriafmt, availableDataLocales,bad_dbc_files,sAchievementCriteriaStore,      storesPath,"Achievement_Criteria.dbc");
    FORMAT(AreaTriggerEntryfmt, availableDataLocales,bad_dbc_files,sAreaTriggerStore,              storesPath,"AreaTrigger.dbc");
    FORMAT(ArmorLocationfmt, availableDataLocales,bad_dbc_files,sArmorLocationStore,            storesPath,"ArmorLocation.dbc");
    FORMAT(AreaGroupEntryfmt, availableDataLocales,bad_dbc_files,sAreaGroupStore,                storesPath,"AreaGroup.dbc");
    FORMAT(AreaPOIEntryfmt, availableDataLocales,bad_dbc_files,sAreaPOIStore,                  storesPath,"AreaPOI.dbc");
    FORMAT(AuctionHouseEntryfmt, availableDataLocales,bad_dbc_files,sAuctionHouseStore,             storesPath,"AuctionHouse.dbc");
    FORMAT(BankBagSlotPricesEntryfmt, availableDataLocales,bad_dbc_files,sBankBagSlotPricesStore,        storesPath,"BankBagSlotPrices.dbc");
    FORMAT(BattlemasterListEntryfmt, availableDataLocales,bad_dbc_files,sBattlemasterListStore,         storesPath,"BattlemasterList.dbc");
    FORMAT(BarberShopStyleEntryfmt, availableDataLocales,bad_dbc_files,sBarberShopStyleStore,          storesPath,"BarberShopStyle.dbc");
    FORMAT(CharStartOutfitEntryfmt, availableDataLocales,bad_dbc_files,sCharStartOutfitStore,          storesPath,"CharStartOutfit.dbc");
    FORMAT(CharTitlesEntryfmt, availableDataLocales,bad_dbc_files,sCharTitlesStore,               storesPath,"CharTitles.dbc");
    FORMAT(ChatChannelsEntryfmt, availableDataLocales,bad_dbc_files,sChatChannelsStore,             storesPath,"ChatChannels.dbc");
    FORMAT(ChrClassesEntryfmt, availableDataLocales,bad_dbc_files,sChrClassesStore,               storesPath,"ChrClasses.dbc");
    FORMAT(ChrRacesEntryfmt, availableDataLocales,bad_dbc_files,sChrRacesStore,                 storesPath,"ChrRaces.dbc");
    FORMAT(CinematicSequencesEntryfmt, availableDataLocales,bad_dbc_files,sCinematicSequencesStore,       storesPath,"CinematicSequences.dbc");
    FORMAT(CreatureDisplayInfofmt, availableDataLocales,bad_dbc_files,sCreatureDisplayInfoStore,      storesPath,"CreatureDisplayInfo.dbc");
    FORMAT(CreatureFamilyfmt, availableDataLocales,bad_dbc_files,sCreatureFamilyStore,           storesPath,"CreatureFamily.dbc");
    FORMAT(CreatureSpellDatafmt, availableDataLocales,bad_dbc_files,sCreatureSpellDataStore,        storesPath,"CreatureSpellData.dbc");
    FORMAT(CreatureTypefmt, availableDataLocales,bad_dbc_files,sCreatureTypeStore,             storesPath,"CreatureType.dbc");
    FORMAT(CurrencyTypesfmt, availableDataLocales,bad_dbc_files,sCurrencyTypesStore,            storesPath,"CurrencyTypes.dbc");
    FORMAT(DungeonEncounterfmt, availableDataLocales,bad_dbc_files,sDungeonEncounterStore,         storesPath,"DungeonEncounter.dbc");
    FORMAT(DurabilityCostsfmt, availableDataLocales,bad_dbc_files,sDurabilityCostsStore,          storesPath,"DurabilityCosts.dbc");
    FORMAT(DurabilityQualityfmt, availableDataLocales,bad_dbc_files,sDurabilityQualityStore,        storesPath,"DurabilityQuality.dbc");
    FORMAT(EmotesEntryfmt, availableDataLocales,bad_dbc_files,sEmotesStore,                   storesPath,"Emotes.dbc");
    FORMAT(EmotesTextEntryfmt, availableDataLocales,bad_dbc_files,sEmotesTextStore,               storesPath,"EmotesText.dbc");
    FORMAT(FactionEntryfmt, availableDataLocales,bad_dbc_files,sFactionStore,                  storesPath,"Faction.dbc");
    for (uint32 i=0; i<sFactionStore.GetNumRows(); ++i)
    {
        FactionEntry const * faction = sFactionStore.LookupEntry(i);
        if (faction && faction->team)
        {
            SimpleFactionsList &flist = sFactionTeamMap[faction->team];
            flist.push_back(i);
        }
    }

    FORMAT(FactionTemplateEntryfmt, availableDataLocales,bad_dbc_files,sFactionTemplateStore,          storesPath,"FactionTemplate.dbc");
    FORMAT(GameObjectDisplayInfofmt, availableDataLocales,bad_dbc_files,sGameObjectDisplayInfoStore,    storesPath,"GameObjectDisplayInfo.dbc");
    for (uint32 i = 0; i < sGameObjectDisplayInfoStore.GetNumRows(); ++i)
    {
        if (GameObjectDisplayInfoEntry const * info = sGameObjectDisplayInfoStore.LookupEntry(i))
        {
            if (info->maxX < info->minX)
                std::swap(*(float*)(&info->maxX), *(float*)(&info->minX));
            if (info->maxY < info->minY)
                std::swap(*(float*)(&info->maxY), *(float*)(&info->minY));
            if (info->maxZ < info->minZ)
                std::swap(*(float*)(&info->maxZ), *(float*)(&info->minZ));
        }
    }

    /*FORMAT(GtOCTRegenHPfmt, availableDataLocales,bad_dbc_files,sGtOCTRegenHPStore,             storesPath,"gtOCTRegenHP.dbc");
    FORMAT(GtRegenHPPerSptfmt, GtOCTRegenMPfmt, availableDataLocales,bad_dbc_files,sGtOCTRegenMPStore,             storesPath,"gtOCTRegenMP.dbc");       -- not used currently
    FORMAT(availableDataLocales,bad_dbc_files,sGtRegenHPPerSptStore,          storesPath,"gtRegenHPPerSpt.dbc");
    FORMAT(ItemDisplayTemplateEntryfmt, availableDataLocales,bad_dbc_files,sItemDisplayInfoStore,          storesPath,"ItemDisplayInfo.dbc");     -- not used currently
    FORMAT(ItemCondExtCostsEntryfmt, availableDataLocales,bad_dbc_files,sItemCondExtCostsStore,         storesPath,"ItemCondExtCosts.dbc");*/
    FORMAT(GemPropertiesEntryfmt, availableDataLocales,bad_dbc_files,sGemPropertiesStore,            storesPath,"GemProperties.dbc");
    FORMAT(GlyphPropertiesfmt, availableDataLocales,bad_dbc_files,sGlyphPropertiesStore,          storesPath,"GlyphProperties.dbc");
    FORMAT(GlyphSlotfmt, availableDataLocales,bad_dbc_files,sGlyphSlotStore,                storesPath,"GlyphSlot.dbc");
    FORMAT(GtBarberShopCostBasefmt, availableDataLocales,bad_dbc_files,sGtBarberShopCostBaseStore,     storesPath,"gtBarberShopCostBase.dbc");
    FORMAT(GtCombatRatingsfmt, availableDataLocales,bad_dbc_files,sGtCombatRatingsStore,          storesPath,"gtCombatRatings.dbc");
    FORMAT(GtChanceToMeleeCritBasefmt, availableDataLocales,bad_dbc_files,sGtChanceToMeleeCritBaseStore,  storesPath,"gtChanceToMeleeCritBase.dbc");
    FORMAT(GtChanceToMeleeCritfmt, availableDataLocales,bad_dbc_files,sGtChanceToMeleeCritStore,      storesPath,"gtChanceToMeleeCrit.dbc");
    FORMAT(GtChanceToSpellCritBasefmt, availableDataLocales,bad_dbc_files,sGtChanceToSpellCritBaseStore,  storesPath,"gtChanceToSpellCritBase.dbc");
    FORMAT(GtChanceToSpellCritfmt, availableDataLocales,bad_dbc_files,sGtChanceToSpellCritStore,      storesPath,"gtChanceToSpellCrit.dbc");
    FORMAT(GtOCTClassCombatRatingScalarfmt, availableDataLocales, bad_dbc_files, sGtOCTClassCombatRatingScalarStore,    storesPath, "gtOCTClassCombatRatingScalar.dbc");
    FORMAT(GtOCTHpPerStaminafmt, availableDataLocales,bad_dbc_files, sGtOCTHpPerStaminaStore,       storesPath, "gtOCTHpPerStamina.dbc");
    FORMAT(GtRegenMPPerSptfmt, availableDataLocales,bad_dbc_files,sGtRegenMPPerSptStore,          storesPath,"gtRegenMPPerSpt.dbc");
    FORMAT(GtSpellScalingfmt, availableDataLocales,bad_dbc_files,sGtSpellScalingStore,           storesPath,"gtSpellScaling.dbc");
    FORMAT(Holidaysfmt, availableDataLocales,bad_dbc_files,sHolidaysStore,                 storesPath,"Holidays.dbc");
    FORMAT(ItemBagFamilyfmt, availableDataLocales,bad_dbc_files,sItemBagFamilyStore,            storesPath,"ItemBagFamily.dbc");
    FORMAT(ItemLimitCategoryEntryfmt, availableDataLocales,bad_dbc_files,sItemLimitCategoryStore,        storesPath,"ItemLimitCategory.dbc");
    FORMAT(ItemRandomPropertiesfmt, availableDataLocales,bad_dbc_files,sItemRandomPropertiesStore,     storesPath,"ItemRandomProperties.dbc");
    FORMAT(ItemRandomSuffixfmt, availableDataLocales,bad_dbc_files,sItemRandomSuffixStore,         storesPath,"ItemRandomSuffix.dbc");
    FORMAT(ItemSetEntryfmt, availableDataLocales,bad_dbc_files,sItemSetStore,                  storesPath,"ItemSet.dbc");
    FORMAT(ItemArmorQualityfmt, availableDataLocales,bad_dbc_files,sItemArmorQualityStore,         storesPath,"ItemArmorQuality.dbc");
    FORMAT(ItemArmorShieldfmt, availableDataLocales,bad_dbc_files,sItemArmorShieldStore,          storesPath,"ItemArmorShield.dbc");
    FORMAT(ItemArmorTotalfmt, availableDataLocales,bad_dbc_files,sItemArmorTotalStore,           storesPath,"ItemArmorTotal.dbc");
    FORMAT(ItemDamagefmt, availableDataLocales,bad_dbc_files,sItemDamageAmmoStore,           storesPath,"ItemDamageAmmo.dbc");
    FORMAT(ItemDamagefmt, availableDataLocales,bad_dbc_files,sItemDamageOneHandStore,        storesPath,"ItemDamageOneHand.dbc");
    FORMAT(ItemDamagefmt, availableDataLocales,bad_dbc_files,sItemDamageOneHandCasterStore,  storesPath,"ItemDamageOneHandCaster.dbc");
    FORMAT(ItemDamagefmt, availableDataLocales,bad_dbc_files,sItemDamageRangedStore,         storesPath,"ItemDamageRanged.dbc");
    FORMAT(ItemDamagefmt, availableDataLocales,bad_dbc_files,sItemDamageThrownStore,         storesPath,"ItemDamageThrown.dbc");
    FORMAT(ItemDamagefmt, availableDataLocales,bad_dbc_files,sItemDamageTwoHandStore,        storesPath,"ItemDamageTwoHand.dbc");
    FORMAT(ItemDamagefmt, availableDataLocales,bad_dbc_files,sItemDamageTwoHandCasterStore,  storesPath,"ItemDamageTwoHandCaster.dbc");
    FORMAT(ItemDamagefmt, availableDataLocales,bad_dbc_files,sItemDamageWandStore,           storesPath,"ItemDamageWand.dbc");
    FORMAT(LFGDungeonEntryfmt, availableDataLocales,bad_dbc_files,sLFGDungeonStore,               storesPath,"LFGDungeons.dbc");
    FORMAT(LockEntryfmt, availableDataLocales,bad_dbc_files,sLockStore,                     storesPath,"Lock.dbc");
    FORMAT(PhaseEntryfmt, availableDataLocales,bad_dbc_files, sPhaseStores,                  storesPath, "Phase.dbc");
    FORMAT(MailTemplateEntryfmt, availableDataLocales,bad_dbc_files,sMailTemplateStore,             storesPath,"MailTemplate.dbc");
    FORMAT(MapEntryfmt, availableDataLocales,bad_dbc_files,sMapStore,                      storesPath,"Map.dbc");
    FORMAT(MapDifficultyEntryfmt, availableDataLocales,bad_dbc_files,sMapDifficultyStore,            storesPath,"MapDifficulty.dbc");
    // fill data
    for (uint32 i = 1; i < sMapDifficultyStore.GetNumRows(); ++i)
        if (MapDifficultyEntry const* entry = sMapDifficultyStore.LookupEntry(i))
            sMapDifficultyMap[MAKE_PAIR32(entry->MapId, entry->Difficulty)] = MapDifficulty(entry->resetTime, entry->maxPlayers, entry->areaTriggerText > 0);
    sMapDifficultyStore.Clear();

    FORMAT(MovieEntryfmt, availableDataLocales,bad_dbc_files,sMovieStore,                    storesPath,"Movie.dbc");
    FORMAT(MountCapabilityfmt, availableDataLocales,bad_dbc_files,sMountCapabilityStore,          storesPath,"MountCapability.dbc");
    FORMAT(MountTypefmt, availableDataLocales,bad_dbc_files,sMountTypeStore,                storesPath,"MountType.dbc");
    FORMAT(OverrideSpellDatafmt, availableDataLocales,bad_dbc_files,sOverrideSpellDataStore,        storesPath,"OverrideSpellData.dbc");
    FORMAT(QuestSortEntryfmt, availableDataLocales,bad_dbc_files,sQuestSortStore,                storesPath,"QuestSort.dbc");
    FORMAT(PvPDifficultyfmt, availableDataLocales,bad_dbc_files,sPvPDifficultyStore,            storesPath,"PvpDifficulty.dbc");
    for (uint32 i = 0; i < sPvPDifficultyStore.GetNumRows(); ++i)
        if (PvPDifficultyEntry const* entry = sPvPDifficultyStore.LookupEntry(i))
            if (entry->bracketId > MAX_BATTLEGROUND_BRACKETS)
                ASSERT(false && "Need update MAX_BATTLEGROUND_BRACKETS by DBC data");

    FORMAT(QuestXPfmt, availableDataLocales,bad_dbc_files,sQuestXPStore,                  storesPath,"QuestXP.dbc");
    FORMAT(QuestFactionRewardfmt, availableDataLocales,bad_dbc_files,sQuestFactionRewardStore,       storesPath,"QuestFactionReward.dbc");
    FORMAT(RandomPropertiesPointsfmt, availableDataLocales,bad_dbc_files,sRandomPropertiesPointsStore,   storesPath,"RandPropPoints.dbc");
    FORMAT(ScalingStatDistributionfmt, availableDataLocales,bad_dbc_files,sScalingStatDistributionStore,  storesPath,"ScalingStatDistribution.dbc");
    FORMAT(ScalingStatValuesfmt, availableDataLocales,bad_dbc_files,sScalingStatValuesStore,        storesPath,"ScalingStatValues.dbc");
    FORMAT(SkillLinefmt, availableDataLocales,bad_dbc_files,sSkillLineStore,                storesPath,"SkillLine.dbc");
    FORMAT(SkillLineAbilityfmt, availableDataLocales,bad_dbc_files,sSkillLineAbilityStore,         storesPath,"SkillLineAbility.dbc");
    FORMAT(SoundEntriesfmt, availableDataLocales,bad_dbc_files,sSoundEntriesStore,             storesPath,"SoundEntries.dbc");
    FORMAT(SpellAuraOptionsEntryfmt, availableDataLocales,bad_dbc_files,sSpellAuraOptionsStore,         storesPath,"SpellAuraOptions.dbc");
    FORMAT(SpellAuraRestrictionsEntryfmt, availableDataLocales,bad_dbc_files,sSpellAuraRestrictionsStore,    storesPath,"SpellAuraRestrictions.dbc");
    FORMAT(SpellCastingRequirementsEntryfmt, availableDataLocales,bad_dbc_files,sSpellCastingRequirementsStore, storesPath,"SpellCastingRequirements.dbc");
    FORMAT(SpellCategoriesEntryfmt, availableDataLocales,bad_dbc_files,sSpellCategoriesStore,          storesPath,"SpellCategories.dbc");
    FORMAT(SpellClassOptionsEntryfmt, availableDataLocales,bad_dbc_files,sSpellClassOptionsStore,        storesPath,"SpellClassOptions.dbc");
    FORMAT(SpellCooldownsEntryfmt, availableDataLocales,bad_dbc_files,sSpellCooldownsStore,           storesPath,"SpellCooldowns.dbc");
    FORMAT(SpellEffectEntryfmt, availableDataLocales,bad_dbc_files,sSpellEffectStore,              storesPath,"SpellEffect.dbc");
    FORMAT(SpellEquippedItemsEntryfmt, availableDataLocales,bad_dbc_files,sSpellEquippedItemsStore,       storesPath,"SpellEquippedItems.dbc");
    FORMAT(SpellInterruptsEntryfmt, availableDataLocales,bad_dbc_files,sSpellInterruptsStore,          storesPath,"SpellInterrupts.dbc");
    FORMAT(SpellLevelsEntryfmt, availableDataLocales,bad_dbc_files,sSpellLevelsStore,              storesPath,"SpellLevels.dbc");
    FORMAT(SpellPowerEntryfmt, availableDataLocales,bad_dbc_files,sSpellPowerStore,               storesPath,"SpellPower.dbc");
    FORMAT(SpellReagentsEntryfmt, availableDataLocales,bad_dbc_files,sSpellReagentsStore,            storesPath,"SpellReagents.dbc");
    FORMAT(SpellScalingEntryfmt, availableDataLocales,bad_dbc_files,sSpellScalingStore,             storesPath,"SpellScaling.dbc");
    FORMAT(SpellShapeshiftEntryfmt, availableDataLocales,bad_dbc_files,sSpellShapeshiftStore,          storesPath,"SpellShapeshift.dbc");
    FORMAT(SpellTargetRestrictionsEntryfmt, availableDataLocales,bad_dbc_files,sSpellTargetRestrictionsStore,  storesPath,"SpellTargetRestrictions.dbc");
    FORMAT(SpellTotemsEntryfmt, availableDataLocales,bad_dbc_files,sSpellTotemsStore,              storesPath,"SpellTotems.dbc");
    FORMAT(SpellEntryfmt, availableDataLocales,bad_dbc_files,sTrueSpellStore,                storesPath,"Spell.dbc");

    for(uint32 i = 1; i < sSpellEffectStore.GetNumRows(); ++i)
    {
        if(SpellEffectEntry const *spellEffect = sSpellEffectStore.LookupEntry(i))
            sSpellEffectMap[spellEffect->EffectSpellId].effects[spellEffect->EffectIndex] = spellEffect;
    }

    sSpellStore.Clear();
    /*sSpellStore.SetNCount((sTrueSpellStore.nCount));
    DataFormatsMap::const_iterator SpellStoreFormat = DataFormats.find("SpellEntryfmt");
    sSpellStore.SetFieldCount((strlen(SpellStoreFormat->second.c_str())));
    sSpellStore.SetIndexTable(new SpellEntry*[sSpellStore.GetFieldCount()]);*/
    sSpellStore.nCount = sTrueSpellStore.nCount;
    sSpellStore.fieldCount = strlen(sSpellStore.fmt);
    sSpellStore.indexTable = new SpellEntry*[sSpellStore.nCount];
    for (uint32 i = 0; i < sTrueSpellStore.GetNumRows(); ++i)
    {
        SpellEntry_n* spell = sTrueSpellStore.LookupEntryNoConst(i);
        if(spell)
        {
            SpellEntry *newspell = new SpellEntry(spell);
            sSpellStore.SetEntry(i, newspell);
        
            if (newspell->Category)
                sSpellCategoryStore[newspell->Category].insert(i);
        }
        else
            sSpellStore.SetIndexTable(i, NULL);
    }

    for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
    {
        SkillLineAbilityEntry const *skillLine = sSkillLineAbilityStore.LookupEntry(j);

        if (!skillLine)
            continue;

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(skillLine->spellId);

        if (spellInfo && IsPassiveSpell(spellInfo->Id))
        {
            for (uint32 i = 1; i < sCreatureFamilyStore.GetNumRows(); ++i)
            {
                CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(i);
                if (!cFamily)
                    continue;

                if (skillLine->skillId != cFamily->skillLine[0] && skillLine->skillId != cFamily->skillLine[1])
                    continue;
                if (spellInfo->spellLevel)
                    continue;

                if (skillLine->learnOnGetSkill != ABILITY_LEARNED_ON_GET_RACE_OR_CLASS_SKILL)
                    continue;

                sPetFamilySpellsStore[i].insert(spellInfo->Id);
            }
        }
    }

    FORMAT(SpellCastTimefmt, availableDataLocales,bad_dbc_files,sSpellCastTimesStore,           storesPath,"SpellCastTimes.dbc");
    FORMAT(SpellDifficultyfmt, availableDataLocales,bad_dbc_files,sSpellDifficultyStore,          storesPath,"SpellDifficulty.dbc");
    FORMAT(SpellDurationfmt, availableDataLocales,bad_dbc_files,sSpellDurationStore,            storesPath,"SpellDuration.dbc");
    FORMAT(SpellFocusObjectfmt, availableDataLocales,bad_dbc_files,sSpellFocusObjectStore,         storesPath,"SpellFocusObject.dbc");
    FORMAT(SpellItemEnchantmentfmt, availableDataLocales,bad_dbc_files,sSpellItemEnchantmentStore,     storesPath,"SpellItemEnchantment.dbc");
    FORMAT(SpellItemEnchantmentConditionfmt, availableDataLocales,bad_dbc_files,sSpellItemEnchantmentConditionStore,storesPath,"SpellItemEnchantmentCondition.dbc");
    FORMAT(SpellRadiusfmt, availableDataLocales,bad_dbc_files,sSpellRadiusStore,              storesPath,"SpellRadius.dbc");
    FORMAT(SpellRangefmt, availableDataLocales,bad_dbc_files,sSpellRangeStore,               storesPath,"SpellRange.dbc");
    FORMAT(SpellRuneCostfmt, availableDataLocales,bad_dbc_files,sSpellRuneCostStore,            storesPath,"SpellRuneCost.dbc");
    FORMAT(SpellShapeshiftFormfmt, availableDataLocales,bad_dbc_files,sSpellShapeshiftFormStore,      storesPath,"SpellShapeshiftForm.dbc");
    FORMAT(SummonPropertiesfmt, availableDataLocales,bad_dbc_files,sSummonPropertiesStore,         storesPath,"SummonProperties.dbc");
    FORMAT(TalentEntryfmt, availableDataLocales,bad_dbc_files,sTalentStore,                   storesPath,"Talent.dbc");
    FORMAT(GuildPerksfmt, availableDataLocales,bad_dbc_files,sGuildPerksStore,               storesPath,"GuildPerkSpells.dbc");

    // Create Spelldifficulty searcher
    for (uint32 i = 0; i < sSpellDifficultyStore.GetNumRows(); ++i)
    {
        SpellDifficultyEntry const *spellDiff = sSpellDifficultyStore.LookupEntry(i);
        if (!spellDiff)
            continue;

        SpellDifficultyEntry newEntry;
        memset(newEntry.SpellID, 0, 4*sizeof(uint32));
        for (int x = 0; x < MAX_DIFFICULTY; ++x)
        {
            if (spellDiff->SpellID[x] <= 0 || !sSpellStore.LookupEntry(spellDiff->SpellID[x]))
            {
                if (spellDiff->SpellID[x] > 0)//don't show error if spell is <= 0, not all modes have spells and there are unknown negative values
                    sLog->outDebug("spelldifficulty_dbc: spell %i at field id:%u at spellid%i does not exist in SpellStore (spell.dbc), loaded as 0", spellDiff->SpellID[x], spellDiff->ID, x);
                newEntry.SpellID[x] = 0;//spell was <= 0 or invalid, set to 0
            }
            else
                newEntry.SpellID[x] = spellDiff->SpellID[x];
        }
        if (newEntry.SpellID[0] <= 0 || newEntry.SpellID[1] <= 0)//id0-1 must be always set!
            continue;

        for (int x = 0; x < MAX_DIFFICULTY; ++x)
            sSpellMgr->SetSpellDifficultyId(uint32(newEntry.SpellID[x]), spellDiff->ID);
    }

    // create talent spells set
    for (unsigned int i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const *talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        for (int j = 0; j < MAX_TALENT_RANK; j++)
            if (talentInfo->RankID[j])
                sTalentSpellPosMap[talentInfo->RankID[j]] = TalentSpellPos(i,j);
    }

    FORMAT(TalentTabEntryfmt, availableDataLocales,bad_dbc_files,sTalentTabStore,                storesPath,"TalentTab.dbc");
    FORMAT(TalentTreePrimarySpellsfmt, availableDataLocales,bad_dbc_files,sTalentTreePrimarySpellsStore,  storesPath, "TalentTreePrimarySpells.dbc");
    
    // prepare fast data access to bit pos of talent ranks for use at inspecting
    {
        // now have all max ranks (and then bit amount used for store talent ranks in inspect)
        for (uint32 talentTabId = 1; talentTabId < sTalentTabStore.GetNumRows(); ++talentTabId)
        {
            TalentTabEntry const *talentTabInfo = sTalentTabStore.LookupEntry(talentTabId);
            if (!talentTabInfo)
                continue;

            // prevent memory corruption; otherwise cls will become 12 below
            if ((talentTabInfo->ClassMask & CLASSMASK_ALL_PLAYABLE) == 0)
                continue;

            // store class talent tab pages
            uint32 cls = 1;
            for (uint32 m=1; !(m & talentTabInfo->ClassMask) && cls < MAX_CLASSES; m <<= 1, ++cls) {}

            sTalentTabPages[cls][talentTabInfo->tabpage]=talentTabId;
        }
    }

    FORMAT(TaxiNodesEntryfmt, availableDataLocales,bad_dbc_files,sTaxiNodesStore,                storesPath,"TaxiNodes.dbc");
    FORMAT(TaxiPathEntryfmt, availableDataLocales,bad_dbc_files,sTaxiPathStore,                 storesPath,"TaxiPath.dbc");
    for (uint32 i = 1; i < sTaxiPathStore.GetNumRows(); ++i)
        if (TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i))
            sTaxiPathSetBySource[entry->from][entry->to] = TaxiPathBySourceAndDestination(entry->ID,entry->price);
    uint32 pathCount = sTaxiPathStore.GetNumRows();

    //## TaxiPathNode.dbc ## Loaded only for initialization different structures
    FORMAT(TaxiPathNodeEntryfmt, availableDataLocales,bad_dbc_files,sTaxiPathNodeStore,             storesPath,"TaxiPathNode.dbc");
    // Calculate path nodes count
    std::vector<uint32> pathLength;
    pathLength.resize(pathCount);                           // 0 and some other indexes not used
    for (uint32 i = 1; i < sTaxiPathNodeStore.GetNumRows(); ++i)
        if (TaxiPathNodeEntry const* entry = sTaxiPathNodeStore.LookupEntry(i))
        {
            if (pathLength[entry->path] < entry->index + 1)
                pathLength[entry->path] = entry->index + 1;
        }
    // Set path length
    sTaxiPathNodesByPath.resize(pathCount);                 // 0 and some other indexes not used
    for (uint32 i = 1; i < sTaxiPathNodesByPath.size(); ++i)
        sTaxiPathNodesByPath[i].resize(pathLength[i]);
    // fill data
    for (uint32 i = 1; i < sTaxiPathNodeStore.GetNumRows(); ++i)
        if (TaxiPathNodeEntry const* entry = sTaxiPathNodeStore.LookupEntry(i))
            sTaxiPathNodesByPath[entry->path].set(entry->index, entry);

    // Initialize global taxinodes mask
    // include existed nodes that have at least single not spell base (scripted) path
    {
        std::set<int32> spellPaths;
        for (uint32 i = 1; i < sSpellStore.GetNumRows(); ++i)
            if (SpellEntry const* sInfo = sSpellStore.LookupEntry(i))
                for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
                    if (sInfo->Effect[j] == SPELL_EFFECT_SEND_TAXI)
                        spellPaths.insert(uint32(sInfo->EffectMiscValue[j]));

        ASSERT(((sTaxiNodesStore.GetNumRows()-1)/32) < TaxiMaskSize && "TaxiMaskSize needs to be increased");
        memset(sTaxiNodesMask,0,sizeof(sTaxiNodesMask));
        memset(sOldContinentsNodesMask,0,sizeof(sOldContinentsNodesMask));
        memset(sHordeTaxiNodesMask,0,sizeof(sHordeTaxiNodesMask));
        memset(sAllianceTaxiNodesMask,0,sizeof(sAllianceTaxiNodesMask));
        memset(sDeathKnightTaxiNodesMask,0,sizeof(sDeathKnightTaxiNodesMask));
        for (uint32 i = 1; i < sTaxiNodesStore.GetNumRows(); ++i)
        {
            TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(i);
            if (!node)
                continue;

            TaxiPathSetBySource::const_iterator src_i = sTaxiPathSetBySource.find(i);
            if (src_i != sTaxiPathSetBySource.end() && !src_i->second.empty())
            {
                bool ok = false;
                for (TaxiPathSetForSource::const_iterator dest_i = src_i->second.begin(); dest_i != src_i->second.end(); ++dest_i)
                {
                    // not spell path
                    if (spellPaths.find(dest_i->second.ID) == spellPaths.end())
                    {
                        ok = true;
                        break;
                    }
                }

                if (!ok)
                    continue;
            }

            // valid taxi network node
            uint8  field   = (uint8)((i - 1) / 32);
            uint32 submask = 1<<((i-1)%32);
            sTaxiNodesMask[field] |= submask;

            if (node->MountCreatureID[0] && node->MountCreatureID[0] != 32981)
                sHordeTaxiNodesMask[field] |= submask;
            if (node->MountCreatureID[1] && node->MountCreatureID[1] != 32981)
                sAllianceTaxiNodesMask[field] |= submask;
            if (node->MountCreatureID[0] == 32981 || node->MountCreatureID[1] == 32981)
                sDeathKnightTaxiNodesMask[field] |= submask;

            // old continent node (+ nodes virtually at old continents, check explicitly to avoid loading map files for zone info)
            if (node->map_id < 2 || i == 82 || i == 83 || i == 93 || i == 94)
                sOldContinentsNodesMask[field] |= submask;

            // fix DK node at Ebon Hold
            if (i == 315) {
                ((TaxiNodesEntry*)node)->MountCreatureID[1] = 32981;
            }
        }
    }

    FORMAT(TotemCategoryEntryfmt, availableDataLocales,bad_dbc_files,sTotemCategoryStore,            storesPath,"TotemCategory.dbc");
    FORMAT(VehicleEntryfmt, availableDataLocales,bad_dbc_files,sVehicleStore,                  storesPath,"Vehicle.dbc");
    FORMAT(VehicleSeatEntryfmt, availableDataLocales,bad_dbc_files,sVehicleSeatStore,              storesPath,"VehicleSeat.dbc");
    FORMAT(WMOAreaTableEntryfmt, availableDataLocales,bad_dbc_files,sWMOAreaTableStore,             storesPath,"WMOAreaTable.dbc");
    for(uint32 i = 0; i < sWMOAreaTableStore.GetNumRows(); ++i)
    {
        if(WMOAreaTableEntry const* entry = sWMOAreaTableStore.LookupEntry(i))
        {
            sWMOAreaInfoByTripple.insert(WMOAreaInfoByTripple::value_type(WMOAreaTableTripple(entry->rootId, entry->adtId, entry->groupId), entry));
        }
    }
    FORMAT(WorldMapAreaEntryfmt, availableDataLocales,bad_dbc_files,sWorldMapAreaStore,             storesPath,"WorldMapArea.dbc");
    FORMAT(WorldMapOverlayEntryfmt, availableDataLocales,bad_dbc_files,sWorldMapOverlayStore,          storesPath,"WorldMapOverlay.dbc");
    FORMAT(WorldSafeLocsEntryfmt, availableDataLocales,bad_dbc_files,sWorldSafeLocsStore,            storesPath,"WorldSafeLocs.dbc");

    // error checks
    if (bad_dbc_files.size() >= DATAFileCount)
    {
        sLog->outError("\nIncorrect DataDir value in worldserver.conf or ALL required *.dbc files (%d) not found by path: %sdbc",DATAFileCount,dataPath.c_str());
        exit(1);
    }
    else if (!bad_dbc_files.empty())
    {
        std::string str;
        for (std::list<std::string>::iterator i = bad_dbc_files.begin(); i != bad_dbc_files.end(); ++i)
            str += *i + "\n";

        sLog->outError("\nSome required *.dbc files (%u from %d) not found or not compatible:\n%s",(uint32)bad_dbc_files.size(),DATAFileCount,str.c_str());
        exit(1);
    }

    // Check loaded DATA files proper version
    if (//!sAreaStore.LookupEntry(5839)              ||       // last area (areaflag) added in 4.2.0a (14480)
        !sCharTitlesStore.LookupEntry(279)         ||       // last char title added in 4.2.0a (14480)
        !sGemPropertiesStore.LookupEntry(1860)     ||       // last gem property added in 4.2.0a (14480)
        !sItemExtendedCostStore.LookupEntry(3652)  ||       // last item extended cost added in 4.2.0a (14480)
        !sMapStore.LookupEntry(968)                ||       // last map added in 4.2.0a (14480)
        !sSpellStore.LookupEntry(102129)            )       // last added spell in 4.2.0a (14480)
    {
        sLog->outError("\nYou have outdated DATA files. Please extract correct versions from current using client.");
        exit(1);
    }

    sLog->outString(">> Initialized %d data stores in %u ms", DATAFileCount, GetMSTimeDiffToNow(oldMSTime));
    sLog->outString();

    //DataFormats.clear();
}

SimpleFactionsList const* GetFactionTeamList(uint32 faction)
{
    FactionTeamMap::const_iterator itr = sFactionTeamMap.find(faction);
    if (itr != sFactionTeamMap.end())
        return &itr->second;

    return NULL;
}

char const* GetPetName(uint32 petfamily, uint32 dbclang)
{
    if (!petfamily)
        return NULL;
    CreatureFamilyEntry const *pet_family = sCreatureFamilyStore.LookupEntry(petfamily);
    if (!pet_family)
        return NULL;
    std::stringstream s;
    s << pet_family->Name[dbclang];
    std::string name = s.str();
    return name.c_str() ? name.c_str() : NULL;
}

TalentSpellPos const* GetTalentSpellPos(uint32 spellId)
{
    TalentSpellPosMap::const_iterator itr = sTalentSpellPosMap.find(spellId);
    if (itr == sTalentSpellPosMap.end())
        return NULL;

    return &itr->second;
}

SpellEffectEntry const* GetSpellEffectEntry(uint32 spellId, uint32 effect)
{
    SpellEffectMap::const_iterator itr = sSpellEffectMap.find(spellId);
    if(itr == sSpellEffectMap.end())
        return NULL;
    
    return itr->second.effects[effect];
}

SpellReagentsEntry const* GetSpellReagentEntry(uint32 spellId, uint8 reagent)
{
    SpellReagentMap::const_iterator itr = sSpellReagentMap.find(spellId);
    if(itr == sSpellReagentMap.end())
        return NULL;

    return itr->second.reagents[reagent];
}

uint32 GetTalentSpellCost(uint32 spellId)
{
    if (TalentSpellPos const* pos = GetTalentSpellPos(spellId))
        return pos->rank+1;

    return 0;
}

int32 GetAreaFlagByAreaID(uint32 area_id)
{
    AreaFlagByAreaID::iterator i = sAreaFlagByAreaID.find(area_id);
    if (i == sAreaFlagByAreaID.end())
        return -1;

    return i->second;
}

WMOAreaTableEntry const* GetWMOAreaTableEntryByTripple(int32 rootid, int32 adtid, int32 groupid)
{
    WMOAreaInfoByTripple::iterator i = sWMOAreaInfoByTripple.find(WMOAreaTableTripple(rootid, adtid, groupid));
        if(i == sWMOAreaInfoByTripple.end())
            return NULL;
        return i->second;
}

MapEntry const* GetMapEntry(uint32 map_id)
{
    if (MapEntry const* mapEntry = sMapStore.LookupEntry(map_id))
        return mapEntry;

    return NULL;
}

AreaTableEntry const* GetAreaEntryByAreaID(uint32 area_id)
{
    int32 areaflag = GetAreaFlagByAreaID(area_id);
    if (areaflag < 0)
        return NULL;

    return sAreaStore.LookupEntry(areaflag);
}

AreaTableEntry const* GetAreaEntryByAreaFlagAndMap(uint32 area_flag,uint32 map_id)
{
    if (area_flag)
        return sAreaStore.LookupEntry(area_flag);

    if (MapEntry const* mapEntry = sMapStore.LookupEntry(map_id))
        return GetAreaEntryByAreaID(mapEntry->linked_zone);

    return NULL;
}

uint32 GetAreaFlagByMapId(uint32 mapid)
{
    AreaFlagByMapID::iterator i = sAreaFlagByMapID.find(mapid);
    if (i == sAreaFlagByMapID.end())
        return 0;
    else
        return i->second;
}

uint32 GetVirtualMapForMapAndZone(uint32 mapid, uint32 zoneId)
{
    if (mapid != 530 && mapid != 571)                        // speed for most cases
        return mapid;

    if (WorldMapAreaEntry const* wma = sWorldMapAreaStore.LookupEntry(zoneId))
        return wma->virtual_map_id >= 0 ? wma->virtual_map_id : wma->map_id;

    return mapid;
}

ContentLevels GetContentLevelsForMapAndZone(uint32 mapid, uint32 zoneId)
{
    mapid = GetVirtualMapForMapAndZone(mapid,zoneId);
    if (mapid < 2 || mapid == 648 || mapid == 654 || mapid == 638 || mapid == 655 || mapid == 656 || mapid == 661 || mapid == 659)
        return CONTENT_1_60;

    if (zoneId == 5034 || zoneId == 4922 || zoneId == 616 || zoneId == 5146 || zoneId == 5042)
        return CONTENT_81_85;

    MapEntry const* mapEntry = sMapStore.LookupEntry(mapid);
    if (!mapEntry)
        return CONTENT_1_60;

    switch(mapEntry->Expansion())
    {
        case 1:
            return CONTENT_61_70;
        case 2:
            return CONTENT_71_80;
        case 3:
            return CONTENT_81_85;
        default:
            return CONTENT_1_60;
    }
}

bool IsTotemCategoryCompatiableWith(uint32 itemTotemCategoryId, uint32 requiredTotemCategoryId)
{
    if (requiredTotemCategoryId == 0)
        return true;
    if (itemTotemCategoryId == 0)
        return false;

    TotemCategoryEntry const* itemEntry = sTotemCategoryStore.LookupEntry(itemTotemCategoryId);
    if (!itemEntry)
        return false;
    TotemCategoryEntry const* reqEntry = sTotemCategoryStore.LookupEntry(requiredTotemCategoryId);
    if (!reqEntry)
        return false;

    if (itemEntry->categoryType != reqEntry->categoryType)
        return false;

    return (itemEntry->categoryMask & reqEntry->categoryMask) == reqEntry->categoryMask;
}

void Zone2MapCoordinates(float& x,float& y,uint32 zone)
{
    WorldMapAreaEntry const* maEntry = sWorldMapAreaStore.LookupEntry(zone);

    // if not listed then map coordinates (instance)
    if (!maEntry)
        return;

    std::swap(x,y);                                         // at client map coords swapped
    x = x*((maEntry->x2-maEntry->x1)/100)+maEntry->x1;
    y = y*((maEntry->y2-maEntry->y1)/100)+maEntry->y1;      // client y coord from top to down
}

void Map2ZoneCoordinates(float& x,float& y,uint32 zone)
{
    WorldMapAreaEntry const* maEntry = sWorldMapAreaStore.LookupEntry(zone);

    // if not listed then map coordinates (instance)
    if (!maEntry)
        return;

    x = (x-maEntry->x1)/((maEntry->x2-maEntry->x1)/100);
    y = (y-maEntry->y1)/((maEntry->y2-maEntry->y1)/100);    // client y coord from top to down
    std::swap(x,y);                                         // client have map coords swapped
}

MapDifficulty const* GetMapDifficultyData(uint32 mapId, Difficulty difficulty)
{
    MapDifficultyMap::const_iterator itr = sMapDifficultyMap.find(MAKE_PAIR32(mapId,difficulty));
    return itr != sMapDifficultyMap.end() ? &itr->second : NULL;
}

MapDifficulty const* GetDownscaledMapDifficultyData(uint32 mapId, Difficulty &difficulty)
{
    uint32 tmpDiff = difficulty;
    MapDifficulty const* mapDiff = GetMapDifficultyData(mapId, Difficulty(tmpDiff));
    if (!mapDiff)
    {
        if (tmpDiff > RAID_DIFFICULTY_25MAN_NORMAL) // heroic, downscale to normal
            tmpDiff -= 2;
        else
            tmpDiff -= 1;   // any non-normal mode for raids like tbc (only one mode)

        // pull new data
        mapDiff = GetMapDifficultyData(mapId, Difficulty(tmpDiff)); // we are 10 normal or 25 normal
        if (!mapDiff)
        {
            tmpDiff -= 1;
            mapDiff = GetMapDifficultyData(mapId, Difficulty(tmpDiff)); // 10 normal
        }
    }

    difficulty = Difficulty(tmpDiff);
    return mapDiff;
}

PvPDifficultyEntry const* GetBattlegroundBracketByLevel(uint32 mapid, uint32 level)
{
    PvPDifficultyEntry const* maxEntry = NULL;              // used for level > max listed level case
    for (uint32 i = 0; i < sPvPDifficultyStore.GetNumRows(); ++i)
    {
        if (PvPDifficultyEntry const* entry = sPvPDifficultyStore.LookupEntry(i))
        {
            // skip unrelated and too-high brackets
            if (entry->mapId != mapid || entry->minLevel > level)
                continue;

            // exactly fit
            if (entry->maxLevel >= level)
                return entry;

            // remember for possible out-of-range case (search higher from existed)
            if (!maxEntry || maxEntry->maxLevel < entry->maxLevel)
                maxEntry = entry;
        }
    }

    return maxEntry;
}

PvPDifficultyEntry const* GetBattlegroundBracketById(uint32 mapid, BattlegroundBracketId id)
{
    for (uint32 i = 0; i < sPvPDifficultyStore.GetNumRows(); ++i)
        if (PvPDifficultyEntry const* entry = sPvPDifficultyStore.LookupEntry(i))
            if (entry->mapId == mapid && entry->GetBracketId() == id)
                return entry;

    return NULL;
}

uint32 const* GetTalentTabPages(uint8 cls)
{
    return sTalentTabPages[cls];
}

float GetGtSpellScalingValue(int8 class_, uint8 level)
{
    if(class_ == -1)
        class_ = MAX_CLASSES; // General distribution
    if(class_ == 0)
        return -1.0f; // shouldn't scale
    
    //They really wants that players reach level 100... in the 5th expansion.
    const gtSpellScaling * spellscaling = sGtSpellScalingStore.LookupEntry( (class_-1)*100 + level - 1 );
    if(spellscaling)
        return spellscaling->coef;
    else
        return -1.0f;
}

// script support functions
 DATAStorage <SoundEntriesEntry>  const* GetSoundEntriesStore()   { return &sSoundEntriesStore;   }
 DATAStorage <SpellEntry>         const* GetSpellStore()          { return &sSpellStore;          }
 DATAStorage <SpellRangeEntry>    const* GetSpellRangeStore()     { return &sSpellRangeStore;     }
 DATAStorage <FactionEntry>       const* GetFactionStore()        { return &sFactionStore;        }
 DATAStorage <CreatureDisplayInfoEntry> const* GetCreatureDisplayStore() { return &sCreatureDisplayInfoStore; }
 DATAStorage <EmotesEntry>        const* GetEmotesStore()         { return &sEmotesStore;         }
 DATAStorage <EmotesTextEntry>    const* GetEmotesTextStore()     { return &sEmotesTextStore;     }
 DATAStorage <AchievementEntry>   const* GetAchievementStore()    { return &sAchievementStore;    }