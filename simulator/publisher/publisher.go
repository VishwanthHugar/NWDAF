package publisher

import (
	"simulator/domain"
	"simulator/generator"
	"math/rand"
	"time"
)

type Publisher struct {
	generators []generator.Generator
	outputs    []func(domain.Event)
	interval   time.Duration
}

func NewPublisher(gens []generator.Generator, outs []func(domain.Event), interval time.Duration) *Publisher {
	return &Publisher{gens, outs, interval}
}

func (p *Publisher) Run(stop <-chan struct{}) {
	for {
		select {
		case <-stop:
			return
		default:
			gen := p.generators[rand.Intn(len(p.generators))]
			ev := gen.Generate()
			for _, out := range p.outputs {
				out(ev)
			}
			time.Sleep(p.interval)
		}
	}
}
