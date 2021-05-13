select count(*) as "COUNT", iso_region from airport where iso_region like 'DE%' group by iso_region order by "COUNT"
