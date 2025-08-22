package generator

import (
	"math/rand"
	"time"
	"simulator/domain"
)

type SMFGenerator struct{}

func (g SMFGenerator) Generate() domain.Event {
	return domain.SMFQoSFlowStats{
		PDUSessionID: "pdu-42",
		ULBps:        rand.Int63n(900_000_000) + 10_000_000,
		DLBps:        rand.Int63n(900_000_000) + 10_000_000,
		QFI:          rand.Intn(15) + 1,
		TS:           time.Now().UnixMilli(),
	}
}
