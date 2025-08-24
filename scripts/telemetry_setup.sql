-- ==========================================================
-- Create Database
-- ==========================================================
-- CREATE DATABASE telemetry;

-- \c telemetry;  -- connect to the telemetry database

-- ==========================================================
-- Schema for telemetry data
-- ==========================================================
CREATE SCHEMA IF NOT EXISTS telemetry;

-- ==========================================================
-- Table: SliceResourceUtilization
-- ==========================================================
CREATE TABLE IF NOT EXISTS telemetry.slice_resource_utilization (
    id BIGSERIAL PRIMARY KEY,
    slice_id TEXT NOT NULL,
    cpu DOUBLE PRECISION NOT NULL,
    mem DOUBLE PRECISION NOT NULL,
    ts BIGINT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

-- Index for slice_id and timestamp
CREATE INDEX IF NOT EXISTS idx_slice_resource_ts 
    ON telemetry.slice_resource_utilization(slice_id, ts);

-- ==========================================================
-- Table: SMFQoSFlowStats
-- ==========================================================
CREATE TABLE IF NOT EXISTS telemetry.smf_qos_flow_stats (
    id BIGSERIAL PRIMARY KEY,
    pdu_session_id TEXT NOT NULL,
    qfi INT NOT NULL,
    dl_bps BIGINT NOT NULL,
    ul_bps BIGINT NOT NULL,
    ts BIGINT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_smf_qos_ts
    ON telemetry.smf_qos_flow_stats(pdu_session_id, ts);

-- ==========================================================
-- Table: OAMHistoricalLoad
-- ==========================================================
CREATE TABLE IF NOT EXISTS telemetry.oam_historical_load (
    id BIGSERIAL PRIMARY KEY,
    node_id TEXT NOT NULL,
    avg_cpu DOUBLE PRECISION NOT NULL,
    avg_mem DOUBLE PRECISION NOT NULL,
    time_window INT NOT NULL,
    ts BIGINT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_oam_load_ts
    ON telemetry.oam_historical_load(node_id, ts);

-- ==========================================================
-- Table: AMFUserConnectionInfo
-- ==========================================================
CREATE TABLE IF NOT EXISTS telemetry.amf_user_connection_info (
    id BIGSERIAL PRIMARY KEY,
    supi TEXT NOT NULL,
    region TEXT NOT NULL,
    connected BOOLEAN NOT NULL,
    ts BIGINT NOT NULL,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_amf_conn_ts
    ON telemetry.amf_user_connection_info(region, ts);

-- Create the analytics_events table
CREATE TABLE IF NOT EXISTS analytics_events (
    id BIGSERIAL PRIMARY KEY,          -- Unique auto-increment ID
    event_type TEXT NOT NULL,          -- Event type
    subscription_id TEXT NOT NULL,     -- Subscription identifier
    payload JSONB,                     -- Event payload stored as JSON
    ts BIGINT NOT NULL,                -- Timestamp in epoch milliseconds
    created_at TIMESTAMPTZ DEFAULT NOW() -- Record creation time
);

-- Optional index for faster lookups by subscription and timestamp
CREATE INDEX IF NOT EXISTS idx_analytics_subscription_ts
ON analytics_events(subscription_id, ts);   