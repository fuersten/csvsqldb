sql> select ident,airport.name,municipality,region.name,country.name from airport join region on airport.iso_region = region.code join country on region.iso_country = country.code where airport.name like '%Heliport%' order by region.name,airport.name limit 10
#IDENT,AIRPORT.NAME,MUNICIPALITY,REGION.NAME,COUNTRY.NAME
'BGAQ','Aappilattoq (Kujalleq) Heliport','Nanortalik','(unassigned)','Greenland'
'BGAG','Aappilattoq (Qaasuitsup) Heliport','Qaasuitsup','(unassigned)','Greenland'
'GL-QCU','Akunaq Heliport',NULL,'(unassigned)','Greenland'
'BGAP','Alluitsup Paa Heliport','Alluitsup Paa','(unassigned)','Greenland'
'BGAS','Ammassivik Heliport',NULL,'(unassigned)','Greenland'
'BGAR','Arsuk Heliport',NULL,'(unassigned)','Greenland'
'BGAT','Attu Heliport','Attu','(unassigned)','Greenland'
'PR16','Banco Popular Center Heliport','San Juan','(unassigned)','Puerto Rico'
'PR23','Baxter-Aibonito Heliport','Aibonito','(unassigned)','Puerto Rico'
'PR08','Baxter-San German Heliport','San German','(unassigned)','Puerto Rico'
