package main

import (
	"bytes"
	"fmt"
	"log"
	"net/http"

	jsoniter "github.com/json-iterator/go"
)

const channelWebhook = "https://discordapp.com/api/webhooks/532935596260458497/PHa2RhPwLXRqb3jGWuuvx-vK7HIvMLecU82iepP7aj74hOgLwtLF23cmLFxlfPpvf9kQ"

type crashReport struct {
	Name     string `json:"name"`
	Dump     string `json:"dump"`
	HostName string `json:"host"`
}

type webhookRequest struct {
	URL      string `json:"url"`
	Content  string `json:"content"`
	Username string `json:"username"`
	Avatar   string `json:"avatar_url"`
}

func handleCrashReport(w http.ResponseWriter, r *http.Request) {
	var data crashReport
	decoder := jsoniter.NewDecoder(r.Body)
	err := decoder.Decode(&data)

	if err != nil {
		log.Printf("Invalid request received from %s!\n", r.Host)
		return
	}

	log.Printf("New crash report has been received from %s@%s!\n", data.Name, r.Host)

	respData := webhookRequest{
		URL:      channelWebhook,
		Content:  fmt.Sprintf("Crash has been detected for user **%s** playing at **%s**!\n```\n%s```", data.Name, data.HostName, data.Dump),
		Username: "Crash Reporter",
		Avatar:   "https://cdn.discordapp.com/attachments/233249310727340032/532897486751268874/MafiaHub.png",
	}

	js, _ := jsoniter.Marshal(respData)
	req, _ := http.NewRequest("POST", channelWebhook, bytes.NewBuffer(js))
	req.Header.Set("Content-Type", "application/json")
	cli := &http.Client{}
	resp, rerr := cli.Do(req)

	if rerr != nil {
		log.Printf("Could not reach Discord servers:\n%v", rerr)
		return
	}
	resp.Body.Close()
}

func main() {
	http.HandleFunc("/push", handleCrashReport)

	fmt.Printf("Starting crash report server...\n")
	if err := http.ListenAndServe(":8001", nil); err != nil {
		log.Fatal(err)
	}
}
