package generator

import (
	"math/rand"
	"time"
	"simulator/domain"
)

type SliceGenerator struct{}

func (g SliceGenerator) Generate() domain.Event {
	return domain.SliceResourceUtilization{
		SliceID: "001-1001",
		CPU:     rand.Float64()*80 + 10,
		Mem:     rand.Float64()*80 + 10,
		TS:      time.Now().UnixMilli(),
	}
}
