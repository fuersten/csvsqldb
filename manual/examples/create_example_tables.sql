CREATE TABLE IF NOT EXISTS airport(id INTEGER PRIMARY KEY,ident CHAR(8),type VARCHAR(50),name VARCHAR(255),latitude_deg REAL,longitude_deg REAL,elevation_ft INTEGER,continent CHAR(2),iso_country CHAR(2),iso_region CHAR(6),municipality VARCHAR(255),scheduled_service CHAR(3),gps_code CHAR(4),iata_code CHAR(4),local_code CHAR(4),home_link VARCHAR(255),wikipedia_link VARCHAR(255),keywords VARCHAR(50));
CREATE TABLE IF NOT EXISTS region(id INTEGER PRIMARY KEY,code CHAR(6),local_code CHAR(4),name VARCHAR(255),continent CHAR(2),iso_country CHAR(2),wikipedia_link VARCHAR(255),keywords VARCHAR(50));
CREATE TABLE IF NOT EXISTS country(id INTEGER PRIMARY KEY,code CHAR(2),name VARCHAR(50),continent CHAR(2),wikipedia_link VARCHAR(255),keywords VARCHAR(50));
