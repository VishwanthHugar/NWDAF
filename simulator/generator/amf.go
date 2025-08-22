package generator

import (
	"math/rand"
	"time"
	"simulator/domain"
)

type AMFGenerator struct{}

func (g AMFGenerator) Generate() domain.Event {
	return domain.AMFUserConnectionInfo{
		SUPI:      "imsi-***",
		Region:    "region-a",
		Connected: rand.Intn(2) == 1,
		TS:        time.Now().UnixMilli(),
	}
}
