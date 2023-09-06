# ethcoreminer

> EtherCore miner with OpenCL, CUDA and stratum support

**ethcoreminer** is an EtherCore GPU mining worker: with ethcoreminer you can mine EtherCore which relies on an ProgPoW mining algorithm. This is the actively maintained version of ethcoreminer. It originates from [ethminer](https://github.com/ethereum-mining/ethminer) project. Check the original [ProgPoW](https://github.com/ifdefelse/progpow) implementation and [EIP-1057](https://eips.ethereum.org/EIPS/eip-1057) for specification.

## Features

* Open source ProgPoW miner software for miners.
* OpenCL mining
* Nvidia CUDA mining
* realistic benchmarking against arbitrary epoch/DAG/blocknumber
* on-GPU DAG generation
* stratum mining without proxy
* OpenCL devices picking
* farm failover (getwork + stratum)
* Ethereum-based ProgPoW implementations supported only, doesn't support previous ethash version or Bitcoin-based forks.


## Table of Contents

* [Install](#install)
* [Usage](#usage)
    * [Examples connecting to pools](#examples-connecting-to-pools)
* [Build](#build)
    * [Continuous Integration and development builds](#continuous-integration-and-development-builds)
    * [Building from source](#building-from-source)
* [Maintainers & Authors](#maintainers--authors)
* [Contribute](#contribute)

## Install

## Usage

The **ethcoreminer** is a command line program. This means you launch it either
from a Windows command prompt or Linux console, or create shortcuts to
predefined command lines using a Linux Bash script or Windows batch/cmd file.
For a full list of available command, please run:

```sh
ethcoreminer --help
```

### Examples connecting to pools

Connecting to [pool.ethercore.io](https://pool.ethercore.io):

`./ethcoreminer -P stratum1+tcp://0xaa16a61dec2d3e260cd1348e48cd259a5fb03f49.test@pool.ethercore.io:8008` or

`ethcoreminer.exe -P stratum1+tcp://0xaa16a61dec2d3e260cd1348e48cd259a5fb03f49.test@pool.ethercore.io:8008`

## Build

### Continuous Integration and development builds

| CI          | OS       | Status  | Development builds |
| ----------- | -------- | -----   | -----------------  |
| AppVeyor    | Windows  | [![Build status](https://ci.appveyor.com/api/projects/status/9gknb76px6t455rf/branch/master?svg=true)](https://ci.appveyor.com/project/ethcoreorg/ethcoreminer/branch/master) | ✓ Build artifacts available for all PRs and branches |

The AppVeyor system automatically builds a Windows .exe for every commit. The latest version is always available [on the landing page](https://ci.appveyor.com/project/ethercoreorg/ethcoreminer) or you can [browse the history](https://ci.appveyor.com/project/ethercoreorg/ethcoreminer/history) to access previous builds.

To download the .exe on a build under `Job name` select the CUDA version you use, choose `Artifacts` then download the zip file.

### Building from source

See [docs/BUILD.md](docs/BUILD.md) for build/compilation details.

## Maintainers & Authors

## Contribute

All bug reports, pull requests and code reviews are very much welcome.


## License

Licensed under the [GNU General Public License, Version 3](LICENSE).
