package main

import (
	"bytes"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"net/http"
	"os"
	"time"
	"net/url"
)

// ---------------- HTTP GET with query ----------------
func getWithQuery(baseURL string, params map[string]string) (*http.Response, []byte, error) {
	u, err := url.Parse(baseURL)
	if err != nil {
		return nil, nil, fmt.Errorf("invalid URL: %v", err)
	}

	q := u.Query()
	for k, v := range params {
		q.Set(k, v)
	}
	u.RawQuery = q.Encode()

	client := &http.Client{Timeout: 5 * time.Second}
	resp, err := client.Get(u.String())
	if err != nil {
		return nil, nil, err
	}

	body, err := io.ReadAll(resp.Body)
	resp.Body.Close()
	if err != nil {
		return resp, nil, fmt.Errorf("failed to read response body: %v", err)
	}

	return resp, body, nil
}

// ---------------- Analytics Request (GET) ----------------
func requestAnalytics(serverURL, subId string) {
	params := map[string]string{
		"subscriptionId": subId,
		"requestId":      fmt.Sprintf("req-%d", time.Now().UnixNano()),
	}

	resp, body, err := getWithQuery(serverURL, params)
	if err != nil {
		fmt.Println("HTTP request error:", err)
		os.Exit(1)
	}

	fmt.Println("\n--- Analytics Response ---")
	fmt.Println("Status Code:", resp.StatusCode)
	//fmt.Println("Raw body:", string(body))

	if len(body) > 0 {
		var prettyJSON bytes.Buffer
		if err := json.Indent(&prettyJSON, body, "", "  "); err != nil {
			fmt.Println(string(body))
		} else {
			fmt.Println(prettyJSON.String())
		}
	} else {
		fmt.Println("No Content")
	}
}

// ---------------- Unsubscribe (POST) ----------------
func unsubscribe(serverURL, subId string) {
	reqBody := map[string]string{"subscriptionId": subId}

	jsonBody, _ := json.Marshal(reqBody)
	client := &http.Client{Timeout: 5 * time.Second}
	resp, err := client.Post(serverURL, "application/json", bytes.NewBuffer(jsonBody))
	if err != nil {
		fmt.Println("HTTP request error:", err)
		os.Exit(1)
	}
	defer resp.Body.Close()

	body, _ := io.ReadAll(resp.Body)

	fmt.Println("\n--- Unsubscribe Response ---")
	fmt.Println("Status Code:", resp.StatusCode)
	if resp.StatusCode == 204 {
		fmt.Println("Subscription", subId, "successfully removed.")
	} else if len(body) > 0 {
		var prettyJSON bytes.Buffer
		if err := json.Indent(&prettyJSON, body, "", "  "); err != nil {
			fmt.Println(string(body))
		} else {
			fmt.Println(prettyJSON.String())
		}
	} else {
		fmt.Println("No Content")
	}
}

// ---------------- Main ----------------
func main() {
	action := flag.String("action", "", "analytics or unsub")
	subId := flag.String("subId", "", "Subscription ID")
	flag.Parse()

	if *action == "" || *subId == "" {
		fmt.Println("Usage: go run simulator.go -action=<analytics/unsub> -subId=<subscriptionId>")
		os.Exit(1)
	}

	analyticsURL := "http://localhost:8080/nwdaf-analytics/v1/Nnwdaf_AnalyticsInfo_Request"
	unsubURL := "http://localhost:8080/nwdaf-analytics/v1/Nnwdaf_AnalyticsSubscription_Unsubscribe"

	switch *action {
	case "analytics":
		requestAnalytics(analyticsURL, *subId)
	case "unsub":
		unsubscribe(unsubURL, *subId)
	default:
		fmt.Println("Invalid action. Choose analytics or unsub")
		os.Exit(1)
	}
}
