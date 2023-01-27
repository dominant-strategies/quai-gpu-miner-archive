# This Makefile is meant to be used by people that do not usually work
# with Go source code. If you know what GOPATH is then you probably
# don't need to bother with make.

GOBIN = ./build/bin
GO ?= latest
GORUN = env GO111MODULE=on go run

quai-cpu-miner:
	go build -o ./build/bin/quai-cpu-miner main.go  
	@echo "Done building."
	@echo "Run \"$(GOBIN)\" to launch quai-cpu-miner"	

# to manually select a location to mine
run-mine:
	./build/bin/quai-cpu-miner $(region) $(zone)

