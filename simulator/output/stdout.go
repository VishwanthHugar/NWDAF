package output

import "simulator/domain"
import "fmt"

func StdoutOutput(ev domain.Event) {
	fmt.Println(ev.ToJSON())
}
