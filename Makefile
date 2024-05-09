# This Makefile is meant to be used by people that do not usually work
# with Go source code. If you know what GOPATH is then you probably
# don't need to bother with make.

GOBIN = ./build/bin
GO ?= latest
GORUN = env GO111MODULE=on go run

clean:
	env GO111MODULE=on go clean -cache
	rm -fr build/_workspace/pkg/ $(GOBIN)/*

debug:
	go build -gcflags="all=-N -l" -o ./build/bin/quai-cpu-miner main.go  
	@echo "Done building."
	@echo "Run \"$(GOBIN)\" to launch quai-cpu-miner"

quai-cpu-miner:
	go build -o ./build/bin/quai-cpu-miner main.go
	@echo "Done building."
	@echo "Run \"$(GOBIN)\" to launch quai-cpu-miner"

# to manually select a location to mine
run-mine:
	sudo cpulimit --limit 200 ./build/bin/quai-cpu-miner $(region) $(zone)

# to run in the background (manually set location)
run-mine-background:
ifeq (,$(wildcard logs))
	mkdir logs
endif
	@nohup sudo cpulimit --limit 200 ./build/bin/quai-cpu-miner $(region) $(zone) 1 >> logs/slice-$(region)-$(zone).log 2>&1 &

stop:
ifeq ($(shell uname -s),Darwin)
	@if pgrep quai-cpu-miner; then pkill -f ./build/bin/quai-cpu-miner; fi
	@while pgrep quai-cpu-miner >/dev/null; do \
		echo "Stopping all Quai Network CPU Miners, please wait until terminated."; \
		sleep 3; \
	done;
else
	@echo "Stopping all Quai Network CPU Miners, please wait until terminated.";
	@if pgrep quai-cpu-miner; then killall -w ./build/bin/quai-cpu-miner; fi
endif
