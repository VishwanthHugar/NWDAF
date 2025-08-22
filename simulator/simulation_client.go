package main

import (
	"bufio"
	"bytes"
	"math/rand"
	"encoding/json"
	"fmt"
	"net/http"
	"os"
	"strings"
	"time"
)

// EventFilterRequest represents the GET request body for NWDAF analytics
type EventFilterRequest struct {
	RequestId string `json:"requestId"`
	CallerNF  string `json:"callerNF"`          // PCF or NSSF
	EventId   string `json:"eventId"`           // LOAD_LEVEL_INFORMATION
	Snssai    string `json:"snssai,omitempty"`  // Specific slice
	AnySlice  bool   `json:"anySlice,omitempty"`// All slices
}

// Generate a simple UUID using math/	rand
func generateUUID() string {
	u := make([]byte, 16)
	_, err := rand.Read(u)
	if err != nil {
		return fmt.Sprintf("%d", time.Now().UnixNano()) // fallback
	}
	// Set version (4) and variant bits per RFC 4122
	u[6] = (u[6] & 0x0f) | 0x40
	u[8] = (u[8] & 0x3f) | 0x80
	return fmt.Sprintf("%x-%x-%x-%x-%x", u[0:4], u[4:6], u[6:8], u[8:10], u[10:])
}

// Choose NF interactively
func chooseNF() string {
	reader := bufio.NewReader(os.Stdin)
	for {
		fmt.Println("Select NF to simulate:")
		fmt.Println("1. PCF")
		fmt.Println("2. NSSF")
		fmt.Print("Enter 1 or 2: ")
		input, _ := reader.ReadString('\n')
		input = strings.TrimSpace(input)
		switch input {
		case "1":
			return "PCF"
		case "2":
			return "NSSF"
		default:
			fmt.Println("Invalid input. Try again.")
		}
	}
}

// Choose slice 
func chooseSlice() (string) {

	slices := []string {"slice-eMBB-001", "slice-URLLC-002", "slice-mMTC-003"}
	return slices[rand.Intn(len(slices))]
}

// Choose all slices
func chooseAllSlice() (bool) {
	return true
}

func main() {
	nwdafURL := "http://localhost:8080/nnwdaf-analyticsinfo/v1/analytics"
	rand.Seed(time.Now().UnixNano())

	callerNF := chooseNF()
	sliceId := chooseSlice()
	anySlice := chooseAllSlice()

	request := EventFilterRequest{
		RequestId: generateUUID(),
		CallerNF:  callerNF,
		EventId:   "LOAD_LEVEL_INFORMATION",
		Snssai:    sliceId,
		AnySlice:  anySlice,
	}

	// Marshal JSON body
	jsonBody, err := json.Marshal(request)
	if err != nil {
		fmt.Println("Error creating JSON:", err)
		os.Exit(1)
	}

	// Create GET request with JSON body
	req, err := http.NewRequest("GET", nwdafURL, bytes.NewBuffer(jsonBody))
	if err != nil {
		fmt.Println("Error creating request:", err)
		os.Exit(1)
	}
	req.Header.Set("Content-Type", "application/json")

	client := &http.Client{Timeout: 5 * time.Second}

	// Send request
	resp, err := client.Do(req)
	if err != nil {
		fmt.Println("HTTP request error:", err)
		os.Exit(1)
	}
	defer resp.Body.Close()

	// Read response
	buf := new(bytes.Buffer)
	buf.ReadFrom(resp.Body)

	fmt.Println("\n--- NWDAF Response ---")
	fmt.Println("Status Code:", resp.StatusCode)
	if buf.Len() > 0 {
		var prettyJSON bytes.Buffer
		err := json.Indent(&prettyJSON, buf.Bytes(), "", "  ")
		if err != nil {
			fmt.Println(buf.String())
		} else {
			fmt.Println(prettyJSON.String())
		}
	} else {
		fmt.Println("No Content")
	}
}
