update quest_template set rewcurrencyid1 = 241, rewcurrencycount1 = 1, rewitemid1 = 0, rewitemcount1=0 where rewitemid1 = 44990; -- champion seal quest reward
insert into spell_script_names values (-45529,"spell_dk_blood_tap"); -- blood tap
update item_template set requiredreputationfaction=0,requiredreputationrank=0 where entry = 67119; -- 1 item
replace into conditions(sourcetypeorreferenceid,sourceentry,conditiontypeorreference,conditionvalue1,conditionvalue2) values (18,56798,24,1,41360); -- cant be used on players
insert into playercreateinfo_spell values(0,11,5421,"Aquatic Form Speed Buff");