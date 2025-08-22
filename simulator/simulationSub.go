package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net"
	"net/http"
	"os"
)

func getFreePort() string {
	ln, err := net.Listen("tcp", ":0") // ask OS to provide a free port
	if err != nil {
		panic(err)
	}
	defer ln.Close()
	addr := ln.Addr().(*net.TCPAddr)
	return fmt.Sprintf("%d", addr.Port)
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: go run simulationSub.go <subscriptionId>")
		return
	}

	subId := os.Args[1]
	port := getFreePort() // dynamically assigned free port
	notifUri := fmt.Sprintf("http://localhost:%s/nwdaf-analytics/v1/Nnwdaf_AnalyticsSubscription_Notify", port)

	// 1️⃣ Start Notify listener
	go func() {
		http.HandleFunc("/nwdaf-analytics/v1/Nnwdaf_AnalyticsSubscription_Notify",
			func(w http.ResponseWriter, r *http.Request) {
				if r.Method != http.MethodPost {
					w.WriteHeader(http.StatusMethodNotAllowed)
					return
				}
				body, _ := ioutil.ReadAll(r.Body)
				defer r.Body.Close()

				var notifyData map[string]interface{}
				if err := json.Unmarshal(body, &notifyData); err != nil {
					fmt.Println("[Simulator] Invalid Notify JSON")
					w.WriteHeader(http.StatusBadRequest)
					return
				}
				fmt.Printf("[Simulator] Received Notify for subscription %s: %v\n",
					notifyData["subscriptionId"], notifyData)

				w.WriteHeader(http.StatusOK) // ACK to NWDAF
			})

		fmt.Printf("[Simulator] Notify server listening on :%s\n", port)
		if err := http.ListenAndServe(":"+port, nil); err != nil {
			fmt.Println("Notify server error:", err)
		}
	}()

	// 2️⃣ Send Subscribe request
	subBody := map[string]interface{}{
		"subscriptionId": subId,
		"eventId":        "SLICE_LOAD_CHANGE",
		"nfId":           "pcf-001",
		"notifUri":       notifUri,
		"analyticsFilter": map[string]interface{}{
			"sliceId": "1-0101",
		},
	}

	data, _ := json.Marshal(subBody)
	resp, err := http.Post("http://localhost:8080/nwdaf-analytics/v1/Nnwdaf_AnalyticsSubscription_Subscribe",
		"application/json", bytes.NewBuffer(data))
	if err != nil {
		fmt.Println("[Simulator] Error subscribing:", err)
		return
	}
	defer resp.Body.Close()
	fmt.Printf("Subscribe status for %s : %d %s\n", subId, resp.StatusCode, resp.Status)
	fmt.Printf("[Simulator] Subscription %s waiting for notifications on port %s...\n", subId, port)

	// Keep simulator alive
	select {}
}
