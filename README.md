# NWDAF


### Database 

Create database:
CREATE DATABASE telemetry;

Create table:
psql -U postgres -d telemetry -f telemetry_setup.sql

Verify:
 psql -U postgres -d telemetry
 \dt telemetry.*