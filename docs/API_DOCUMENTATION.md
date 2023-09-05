# ethcoreminer's API documentation

## Table of Contents

* [Introduction](#introduction)
* [Activation and Security](#activation-and-security)
* [Usage](#usage)
* [List of requests](#list-of-requests)
    * [api_authorize](#api_authorize)
    * [miner_ping](#miner_ping)
    * [miner_getstatdetail](#miner_getstatdetail)
    * [miner_getstat1](#miner_getstat1)
    * [miner_restart](#miner_restart)
    * [miner_reboot](#miner_reboot)
    * [miner_shuffle](#miner_shuffle)
    * [miner_getconnections](#miner_getconnections)
    * [miner_setactiveconnection](#miner_setactiveconnection)
    * [miner_addconnection](#miner_addconnection)
    * [miner_removeconnection](#miner_removeconnection)
    * [miner_getscramblerinfo](#miner_getscramblerinfo)
    * [miner_setscramblerinfo](#miner_setscramblerinfo)
    * [miner_pausegpu](#miner_pausegpu)
    * [miner_setverbosity](#miner_setverbosity)

## Introduction

ethcoreminer implements an API (Application Programming Interface) interface which allows to monitor/control some of the run-time values endorsed by this miner. The API interface is available under the following circumstances:

* If you're using a binary release downloaded from the [releases](https://github.com/gangnamtestnet/ethcoreminer/releases) section of this repository
* If you build the application from source ensuring you add the compilation switch `-D APICORE=ON`

## Activation and Security

Whenever the above depicted conditions are met you can take advantage of the API support by adding the `--api-bind` argument to the command line used to launch ethcoreminer. The format of this argument is `--api-bind address:port` where `nnnn` is any valid TCP port number (1-65535) and is required, and the `address` dictates what ip the api will listen on, and is optional, and defaults to "all ipv4 addresses". Examples:

```shell
./ethcoreminer [...] --api-bind 3333
```

This example puts the API interface listening on port 3333 of **any** local IPv4 address which means the loop-back interface (127.0.0.1/127.0.1.1) and any configured IPv4 address of the network card(s). To only listen to localhost connections (which may be a more secure setting),

```shell
./ethcoreminer [...] --api-bind 127.0.0.1:3333
```
and likewise, to only listen on a specific address, replace `127.0.0.1` accordingly.



The API interface not only offers monitoring queries but also implements some methods which may affect the functioning of the miner. These latter operations are named _write_ actions: if you want to inhibit the invocation of such methods you may want to put the API interface in **read-only** mode which means only query to **get** data will be allowed and no _write_ methods will be allowed. To do this simply add the - (minus) sign in front of the port number thus transforming the port number into a negative number. Example for read-only mode:

```shell
./ethcoreminer [...] --api-bind -3333
```

_Note. The port number in this examples is taken randomly and does not imply a suggested value. You can use any port number you wish while it's not in use by other applications._

To gain further security you may wish to password protect the access to your API interface simply by adding the `--api-password` argument to the command line sequence, followed by the password you wish. Password may be composed by any printable char and **must not** have spaces. Password checking is **case sensitive**. Example for password protected API interface:

```shell
./ethcoreminer [...] --api-bind -3333 --api-password MySuperSecurePassword!!#123456
```

At the time of writing of this document ethcoreminer's API interface does not implement any sort of data encryption over SSL secure channel so **be advised your passwords will be sent as plain text over plain TCP sockets**.

## Usage

Access to API interface is performed through a TCP socket connection to the API endpoint (which is the IP address of the computer running ethcoreminer's API instance at the configured port). For instance if your computer address is 192.168.1.1 and have configured ethcoreminer to run with `--api-bind 3333` your endpoint will be 192.168.1.1:3333.

Messages exchanged through this channel must conform to the [JSON-RPC 2.0 specification](http://www.jsonrpc.org/specification) so basically you will issue **requests** and will get back **responses**. At the time of writing this document do not expect any **notification**. All messages must be line feed terminated.

To quickly test if your ethcoreminer's API instance is working properly you can issue this simple command:

```shell
echo '{"id":0,"jsonrpc":"2.0","method":"miner_ping"}' | netcat 192.168.1.1 3333
```

and will get back a response like this:

```shell
{"id":0,"jsonrpc":"2.0","result":"pong"}
```

This shows the API interface is live and listening on the configured endpoint.

## List of requests

|   Method  | Description  | Write Protected |
| --------- | ------------ | --------------- |
| [api_authorize](#api_authorize) | Issues the password to authenticate the session | No |
| [miner_ping](#miner_ping) | Responds back with a "pong" | No |
| [miner_getstatdetail](#miner_getstatdetail) | Request the retrieval of operational data in most detailed form | No
| [miner_getstat1](#miner_getstat1) | Request the retrieval of operational data in compatible format | No
| [miner_restart](#miner_restart) | Instructs ethcoreminer to stop and restart mining | Yes |
| [miner_reboot](#miner_reboot) | Try to launch reboot.bat (on Windows) or reboot.sh (on Linux) in the ethcoreminer executable directory | Yes
| [miner_shuffle](#miner_shuffle) | Initializes a new random scramble nonce | Yes
| [miner_getconnections](#miner_getconnections) | Returns the list of connections held by ethcoreminer | No
| [miner_setactiveconnection](#miner_setactiveconnection) | Instruct ethcoreminer to immediately connect to the specified connection | Yes
| [miner_addconnection](#miner_addconnection) | Provides ethcoreminer with a new connection to use | Yes
| [miner_removeconnection](#miner_removeconnection) | Removes the given connection from the list of available so it won't be used again | Yes
| [miner_getscramblerinfo](#miner_getscramblerinfo) | Retrieve information about the nonce segments assigned to each GPU | No
| [miner_setscramblerinfo](#miner_setscramblerinfo) | Sets information about the nonce segments assigned to each GPU | Yes
| [miner_pausegpu](#miner_pausegpu) | Pause/Start mining on specific GPU | Yes

### api_authorize

If your API instance is password protected by the usage of `--api-password` any remote trying to interact with the API interface **must** send this method immediately after connection to get authenticated. The message to send is:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "api_authorize",
  "params": {
    "psw": "MySuperSecurePassword!!#123456"
  }
}
```

where the member `psw` **must** contain the very same password configured with `--api-password` argument. As expected result you will get a JSON-RPC 2.0 response with positive or negative values. For instance if the password matches you will get a response like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true,
}
```

or, in case of any error:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "error": {
    "code": -401,
    "message": "Invalid password"
  }
}
```

### miner_ping

This method is primarily used to check the liveness of the API interface.

To invoke the action:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_ping"
}
```

and expect back a result like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": "pong"
}
```

which confirms the action has been performed.

If you get no response or the socket timeouts it's likely your ethcoreminer's instance has become unresponsive (or in worst cases the OS of your mining rig is unresponsive) and needs to be re-started/re-booted.

### miner_getstatdetail

With this method you expect back a detailed collection of statistical data. To issue a request:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_getstatdetail"
}
```

and expect back a response like this:

```js
{
  "id": 0,
  "jsonrpc": "2.0",
  "result": {
    "connection": {                                     // Current active connection
      "connected": true,
      "switches": 1,
      "uri": "stratum1+tls12://<ethaddress>.wworker@eu1.ethermine.org:5555"
    },
    "devices": [                                        // Array subscribed of devices
      {
        "_index": 0,                                    // Miner ordinal 
        "_mode": "CUDA",                                // Miner mode : "OpenCL" / "CUDA"
        "hardware": {                                   // Device hardware info
          "name": "GeForce GTX 1050 Ti 3.95 GB",        // Name
          "pci": "01:00.0",                             // Pci Id
          "sensors": [                                  // An array made of ...
            47,                                         //  + Detected temp
            70,                                         //  + Fan percent
            0                                           //  + Power drain in watts
          ],
          "type": "GPU"                                 // Device Type : "CPU" / "GPU" / "ACCELERATOR"
        },
        "mining": {                                     // Mining info
          "hashrate": "0x0000000000e3fcbb",             // Current hashrate in hashes per second
          "pause_reason": null,                         // If the device is paused this contains the reason
          "paused": false,                              // Wheter or not the device is paused
          "segment": [                                  // The search segment of the device
            "0xbcf0a663bfe75dab",                       //  + Lower bound
            "0xbcf0a664bfe75dab"                        //  + Upper bound
          ],
          "shares": [                                   // Shares / Solutions stats
            1,                                          //  + Found shares
            0,                                          //  + Rejected (by pool) shares
            0,                                          //  + Failed shares (always 0 if --no-eval is set)
            15                                          //  + Time in seconds since last found share
          ]
        }
      },
      { ... }                                           // Another device
      { ... }                                           // And another ...
    ],
    "host": {
      "name": "miner01",                                // Host name of the computer running ethcoreminer
      "runtime": 121,                                   // Duration time (in seconds)
      "version": "ethcoreminer-0.18.0-alpha.1+commit.70c7cdbe.dirty"
    },
    "mining": {                                         // Mining info for the whole instance
      "difficulty": 3999938964,                         // Actual difficulty in hashes
      "epoch": 227,                                     // Current epoch
      "epoch_changes": 1,                               // How many epoch changes occurred during the run
      "hashrate": "0x00000000054a89c8",                 // Overall hashrate (sum of hashrate of all devices)
      "shares": [                                       // Shares / Solutions stats
        2,                                              //  + Found shares
        0,                                              //  + Rejected (by pool) shares
        0,                                              //  + Failed shares (always 0 if --no-eval is set)
        15                                              //  + Time in seconds since last found share
      ]
    },
    "monitors": {                                       // A nullable object which may contain some triggers
      "temperatures": [                                 // Monitor temperature
        60,                                             //  + Resume mining if device temp is <= this threshold
        75                                              //  + Suspend mining if device temp is >= this threshold
      ]
    }
  }
}
```

### miner_getstat1

With this method you expect back a collection of statistical data. To issue a request:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_getstat1"
}
```

and expect back a response like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": [
    "ethcoreminer-0.16.0.dev0+commit.41639944", // Running ethcoreminer's version
    "48",                                   // Total running time in minutes
    "87221;54;0",                           // ETH hashrate in KH/s, submitted shares, rejected shares
    "14683;14508;14508;14508;14508;14508",  // Detailed ETH hashrate in KH/s per GPU
    "0;0;0",                                // DCR hashrate in KH/s, submitted shares, rejected shares (not used)
    "off;off;off;off;off;off",              // Detailed DCR hashrate in KH/s per GPU (not used)
    "53;90;50;90;56;90;58;90;61;90;60;90",  // Temp and fan speed pairs per GPU
    "eu1.ethermine.org:4444",               // Mining pool currently active
    "0;0;0;0"                               // ETH invalid shares, ETH pool switches, DCR invalid shares, DCR pool switches
  ]
}
```

Some of the arguments here expressed have been set for compatibility with other miners so their values are not set. For instance, ethcoreminer **does not** support dual (ETH/DCR) mining.

### miner_restart

With this method you instruct ethcoreminer to _restart_ mining. Restarting means:

* Stop actual mining work
* Unload generated DAG files
* Reset devices (GPU)
* Regenerate DAG files
* Restart mining

The invocation of this method **_may_** be useful if you detect one or more GPUs are in error, but in a recoverable state (eg. no hashrate but the GPU has not fallen off the bus). In other words, this method works like stopping ethcoreminer and restarting it **but without loosing connection to the pool**.

To invoke the action:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_restart"
}
```

and expect back a result like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true
}
```

which confirms the action has been performed.

**Note**: This method is not available if the API interface is in read-only mode (see above).

### miner_reboot

With this method you instruct ethcoreminer to execute reboot.bat (on Windows) or reboot.sh (on Linux) script which must exists and being executable in the ethcoreminer directory.
As ethcoreminer has no idea what's going on in the script, ethcoreminer continues with it's normal work.
If you invoke this function `api_miner_reboot` is passed to the script as first parameter.

To invoke the action:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_reboot"
}
```

and expect back a result like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true
}
```

which confirms an executable file was found and ethcoreminer tried to start it.

**Note**: This method is not available if the API interface is in read-only mode (see above).

### miner_shuffle

The mining process is nothing more that finding the right number (nonce) which, applied to an algorithm (ethash) and some data, gives a result which is below or equal to a given target. This is very very (very) short!
The range of nonces to be searched is a huge number: 2^64 = 18446744073709600000~ possible values. Each one has the same probability to be the _right_ one.

Every time ethcoreminer receives a job from a pool you'd expect the miner to begin searching from the first, but that would be boring. So the concept of scramble nonce has been introduced to achieve these goals:

* Start the searching from a random point within the range
* Ensure all GPUs do not search the same data, or, in other words, ensure each GPU searches its own range of numbers without overlapping with the same numbers of the other GPUs

All `miner_shuffle` method does is to re-initialize a new random scramble nonce to start from in next jobs.

To invoke the action:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_shuffle"
}
```

and expect back a result like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true
}
```

which confirms the action has been performed.

### miner_getconnections

When you launch ethcoreminer you provide a list of connections specified by the `-P` argument. If you want to remotely check which is the list of connections ethcoreminer is using, you can issue this method:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_getconnections"
}
```

and expect back a result like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": [
    {
      "active": false,
      "index": 0,
      "uri": "stratum+tcp://<omitted-ethereum-address>.worker@eu1.ethermine.org:4444"
    },
    {
      "active": true,
      "index": 1,
      "uri": "stratum+tcp://<omitted-ethereum-address>.worker@eu1.ethermine.org:14444"
    },
    {
      "active": false,
      "index": 2,
      "uri": "stratum+tcp://<omitted-ethereum-classic-address>.worker@eu1-etc.ethermine.org:4444"
    }
  ]
}
```

The `result` member contains an array of objects, each one with the definition of the connection (in the form of the URI entered with the `-P` argument), its ordinal index and the indication if it's the currently active connetion.

### miner_setactiveconnection

Given the example above for the method [miner_getconnections](#miner_getconnections) you see there is only one active connection at a time. If you want to control remotely your mining facility and want to force the switch from one connection to another you can issue this method:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_setactiveconnection",
  "params": {
    "index": 0
  }
}
```
or
```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_setactiveconnection",
  "params": {
    "URI": ".*etc.*"
  }
}
```

You have to pass the `params` member as an object which has member `index` valued to the ordinal index of the connection you want to activate. Alternatively, you can pass a regular expression to be matched against the connection URIs. As a result you expect one of the following:

* Nothing happens if the provided index is already bound to an _active_ connection
* If the selected index is not of an active connection then ethcoreminer will disconnect from currently active connection and reconnect immediately to the newly selected connection
* An error result if the index is out of bounds or the request is not properly formatted

**Please note** that this method changes the runtime behavior only. If you restart ethcoreminer from a batch file the active connection will become again the first one of the `-P` arguments list.

### miner_addconnection

If you want to remotely add a new connection to the running instance of ethcoreminer you can use this this method by sending a message like this

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_addconnection",
  "params": {
    "uri": "stratum+tcp://<ethaddress>.<workername>@eu1.ethermine.org:4444"
  }
}
```

You have to pass the `params` member as an object which has member `uri` valued exactly the same way you'd add a connection using the `-P` argument. As a result you expect one of the following:

* An error if the uri is not properly formatted
* An error if you try to _mix_ stratum mode with getwork mode (which begins with `http://`)
* A success message if the newly defined connection has been properly added

Eventually you may want to issue [miner_getconnections](#miner_getconnections) method to identify which is the ordinal position assigned to the newly added connection and make use of [miner_setactiveconnection](#miner_setactiveconnection) method to instruct ethcoreminer to use it immediately.

**Please note** that this method changes the runtime behavior only. If you restart ethcoreminer from a batch file the added connection won't be available if not present in the `-P` arguments list.

### miner_removeconnection

Recall once again the example for the method [miner_getconnections](#miner_getconnections). If you wish to remove the third connection (the Ethereum classic one) from the list of connections (so it won't be used in case of failover) you can send this method:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_removeconnection",
  "params": {
    "index": 2
  }
}
```

You have to pass the `params` member as an object which has member `index` valued to the ordinal index (zero based) of the connection you want to remove. As a result you expect one of the following:

* An error if the index is out of bounds **or if the index corresponds to the currently active connection**
* A success message. In such case you can later reissue [miner_getconnections](#miner_getconnections) method to check the connection has been effectively removed.

**Please note** that this method changes the runtime behavior only. If you restart ethcoreminer from a batch file the removed connection will become again again available if provided in the `-P` arguments list.

### miner_getscramblerinfo

When searching for a valid nonce the miner has to find (at least) 1 of possible 2^64 solutions. This would mean that a miner who claims to guarantee to find a solution in the time of 1 block (15 seconds for Ethereum) should produce 1230 PH/s (Peta hashes) which, at the time of writing, is more than 4 thousands times the whole hashing power allocated worldwide for Ethereum.
This gives you an idea of numbers in play. Luckily a couple of factors come in our help: difficulty and time. We can imagine difficulty as a sort of judge who determines how many of those possible solutions are valid. And the block time which allows the miner to stay longer on a sequence of numbers to find the solution.
This all said it's however impossible for any miner (no matter if CPU or GPU or even ASIC) to cover the most part of this huge range in reasonable amount of time. So we need to resign to examine and test only a small fraction of this range.

ethcoreminer, at start, randomly chooses a scramble_nonce, a random number picked in the 2^64 range to start checking nonces from. In addition ethcoreminer gives each GPU a unique, non overlapping, range of nonces called _segment_. Segments ensure no GPU does the same job of another GPU thus avoiding two GPU find the same result.
To accomplish this each segment has a range 2^40 nonces by default. If you want to check which is the scramble_nonce and which are the segments assigned to each GPU you can issue this method:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_getscramblerinfo"
}
```

and expect a result like this:

```js
{
  "id": 0,
  "jsonrpc": "2.0",
  "result": {
    "device_count": 6,                          // How many devices are mining
    "device_width": 32,                         // The width (as exponent of 2) of each device segment
    "start_nonce": "0xd3719cef9dd02322"         // The start nonce of the segment
  }
}
```
To compute the effective start_nonce assigned to each device you can use this simple math : `start_nonce + ((2^segment_width) * device_index))`
The information hereby exposed may be used in large mining operations to check whether or not two (or more) rigs may result having overlapping segments. The possibility is very remote ... but is there.

### miner_setscramblerinfo

To approach this method you have to read carefully the method [miner_getscrambleinfo](#miner_getscrambleinfo) and what it reports. By the use of this method you can set a new scramble_nonce and/or set a new segment width:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_setscramblerinfo",
  "params": {
    "noncescrambler": 16704043538687679721,      // At least one of these two members
    "segmentwidth": 38                           // or both.
  }
}
```
or, if you prefer the hexadecimal notation,
```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_setscramblerinfo",
  "params": {
    "noncescrambler": "0x6f3ab2803cfeea12",      // At least one of these two members
    "segmentwidth": 38                           // or both.
  }
}
```

This will adjust nonce scrambler and segment width assigned to each GPU. This method is intended only for highly skilled people who do a great job in math to determine the optimal values for large mining operations.
**Use at your own risk**

### miner_pausegpu

Pause or (restart) mining on specific GPU.
This ONLY (re)starts mining if GPU was paused via a previous API call and not if GPU pauses for other reasons.

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_pausegpu",
  "params": {
    "index": 0,
    "pause": true
  }
}
```

and expect a result like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true
}
```

which confirms the action has been performed.
Again: This ONLY (re)starts mining if GPU was paused via a previous API call and not if GPU pauses for other reasons.

### miner_setverbosity

Set the verbosity level of ethcoreminer.

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "method": "miner_setverbosity",
  "params": {
    "verbosity": 9
  }
}
```

and expect a result like this:

```js
{
  "id": 1,
  "jsonrpc": "2.0",
  "result": true
}
```
