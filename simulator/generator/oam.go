package generator

import (
	"math/rand"
	"time"
	"simulator/domain"
)

type OAMGenerator struct{}

func (g OAMGenerator) Generate() domain.Event {
	return domain.OAMHistoricalLoad{
		NodeID: "gNB-101",
		AvgCPU: rand.Float64()*75 + 5,
		AvgMem: rand.Float64()*75 + 5,
		Window: rand.Intn(55) + 5,
		TS:     time.Now().UnixMilli(),
	}
}
