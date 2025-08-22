package main

import (
	"encoding/json"
	"fmt"
	"log"
	"math/rand"
	"net/http"
	"time"

	"simulator/domain"
	"simulator/generator"
	"simulator/output"
	//"simulator/publisher"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	// Initialize generators
	sliceGen := generator.SliceGenerator{}
	amfGen := generator.AMFGenerator{}
	smfGen := generator.SMFGenerator{}
	oamGen := generator.OAMGenerator{}

	// Typed output slice
	eventOutputs := []func(domain.Event){
		output.StdoutOutput,
	}

	// Background publisher (optional)
	//gens := []generator.Generator{sliceGen, amfGen, smfGen, oamGen}
	//pub := publisher.NewPublisher(gens, eventOutputs, 200*time.Millisecond)
	//stop := make(chan struct{})
	//go pub.Run(stop)

	// Helper to handle event generation per endpoint
	handleEvent := func(gen generator.Generator) http.HandlerFunc {
		return func(w http.ResponseWriter, r *http.Request) {
			if r.Method != http.MethodGet && r.Method != http.MethodPost {
				http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
				return
			}

			event := gen.Generate()
			for _, out := range eventOutputs {
				out(event)
			}

			w.Header().Set("Content-Type", "application/json")
			if err := json.NewEncoder(w).Encode(event); err != nil {
				log.Println("Error encoding response:", err)
			}
		}
	}

	// Expose endpoints
	http.HandleFunc("/amf/v1/ue-connections", handleEvent(amfGen))
	http.HandleFunc("/smf/v1/qos-flows", handleEvent(smfGen))
	http.HandleFunc("/oam/v1/historical-load", handleEvent(oamGen))
	http.HandleFunc("/slice/v1/resource-usage", handleEvent(sliceGen))

	serverAddr := ":8081"
	fmt.Println("NWDAF HTTP server running on", serverAddr)
	if err := http.ListenAndServe(serverAddr, nil); err != nil {
		log.Fatal(err)
	}

	// Optional demo stop
	// time.Sleep(30 * time.Second)
	// close(stop)
}
