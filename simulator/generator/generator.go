package generator

import "simulator/domain"

type Generator interface {
	Generate() domain.Event
}