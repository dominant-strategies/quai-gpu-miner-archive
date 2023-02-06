# Quai CPU Miner
Official Golang implementation of Quai's CPU Miner.

# Building the source
For prerequisites and detailed build instructions please read the Installation Instructions.

Building quai-cpu-miner requires both a Go (version 1.19 or later) and a C compiler. You can install them using your favourite package manager. Once the dependencies are installed, run

## Build via Makefile
```shell
make quai-cpu-miner
```

## Build via GoLang directly

```shell
go build -o ./build/bin/quai-cpu-miner main.go
```

Configuring the Manager
The config.yaml file
In the file config.yaml.dist you should see something like this:

# Config for node URLs
## Proxy Credentials
- ProxyURL: "tcp ip address+port"
- RewardAddress: "address"
- Password: "password"
- Proxy: boolean

## Connection details to Quai nodes
- PrimeURL: "url"
- RegionURLs: "urls"
- ZoneURLs: "urls"

This file is responsible for storing your settings. The settings saved in this file on starting the manager are what will be applied when it runs.

Location: this stores the Region and Zone values for setting the mining location manually. (Will only be used if Optimize is set to false.) Values must correspond to the current Quai Network Ontology. At mainnet launch, the values for Region will be 1-3 and for Zone 1-3. So, for example, to mine on Region 2 Zone 3 you would save the Location value like this:

Location: [2,3]

PrimeURL: stores the URL for the Prime chain. Should not be changed.

RegionURLs: stores the URLs for the Region chains. Should not be changed.

ZoneURLs: stores the URLs for the Zone chains. Should not be changed.

Note that some of the values supplied in the config.yaml file can be overridden with the appropriate command and arguments.

# Run the miner
Setting the region and zone flags for mining location

## Run via Makefile
```shell
make run-mine region=0 zone=0
```

## Run via command line
```shell
./build/bin/quai-cpu-miner 0 0
```

When the manager starts it should print something like:

To run in the background:

```shell
make run-background
```
It is also possible to manually set the location. Doing so will override the location value set in the config.yaml file. The appropriate arguments must also be supplied like this:

```shell
make run-mine region=1 zone=2
````
This will start the manager mining in Region 1 Zone 2. Location values must correspond to the current Quai Network ontology - at start, that ontology is 3x3, meaning Region values are 1-3 and Zone values are 1-3. So, for example, to mine in Region 3 Zone 1 you would enter:

```shell
make run-mine region=3 zone=1
```
If you start in manual mode the manager should print:

Set
Run via Go binary