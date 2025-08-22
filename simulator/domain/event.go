package domain

import (
	"encoding/json"
)

// Event interface
type Event interface {
	Type() string
	ToJSON() string
}

// Helper function to encode event with "type" field
func encode(v interface{}, typ string) string {
	m := map[string]interface{}{}
	b, _ := json.Marshal(v)
	json.Unmarshal(b, &m)
	m["type"] = typ
	out, _ := json.Marshal(m)
	return string(out)
}

// -------- Event Structs --------

type SliceResourceUtilization struct {
	SliceID string  `json:"sliceId"`
	CPU     float64 `json:"cpu"`
	Mem     float64 `json:"mem"`
	TS      int64   `json:"ts"`
}

func (e SliceResourceUtilization) Type() string   { return "SliceResourceUtilization" }
func (e SliceResourceUtilization) ToJSON() string { return encode(e, e.Type()) }

type AMFUserConnectionInfo struct {
	SUPI      string `json:"supi"`
	Region    string `json:"region"`
	Connected bool   `json:"connected"`
	TS        int64  `json:"ts"`
}

func (e AMFUserConnectionInfo) Type() string   { return "AMFUserConnectionInfo" }
func (e AMFUserConnectionInfo) ToJSON() string { return encode(e, e.Type()) }

type SMFQoSFlowStats struct {
	PDUSessionID string `json:"pduSessionId"`
	ULBps        int64  `json:"ul_bps"`
	DLBps        int64  `json:"dl_bps"`
	QFI          int    `json:"qfi"`
	TS           int64  `json:"ts"`
}

func (e SMFQoSFlowStats) Type() string   { return "SMFQoSFlowStats" }
func (e SMFQoSFlowStats) ToJSON() string { return encode(e, e.Type()) }

type OAMHistoricalLoad struct {
	NodeID string  `json:"nodeId"`
	AvgCPU float64 `json:"avg_cpu"`
	AvgMem float64 `json:"avg_mem"`
	Window int     `json:"window"`
	TS     int64   `json:"ts"`
}

func (e OAMHistoricalLoad) Type() string   { return "OAMHistoricalLoad" }
func (e OAMHistoricalLoad) ToJSON() string { return encode(e, e.Type()) }
