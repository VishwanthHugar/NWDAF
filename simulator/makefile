# Makefile for Go Simulator

# Binary name
BINARY=simulator_exe

# Go files
SRC=$(shell find . -name '*.go')

# Default: build and run
all: build run

# Build the binary
build:
	go build -o $(BINARY) main.go

# Run the binary
run:
	./$(BINARY)

# Clean binary
clean:
	rm -f $(BINARY)

# Run without building
run-dev:
	go run main.go

# Compile for Linux 64-bit (cross-compile)
build-linux:
	GOOS=linux GOARCH=amd64 go build -o $(BINARY)_linux main.go
