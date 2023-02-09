package util

import (
	"bufio"
	"encoding/json"
	"errors"
	"io"
	"log"
	"net"
	"net/netip"
	"sync"

	"github.com/INFURA/go-ethlibs/jsonrpc"

	"github.com/dominant-strategies/go-quai-stratum/rpc"
	"github.com/dominant-strategies/go-quai/core/types"
)

type MinerSession struct {
	proto string
	ip    netip.Addr
	port  int
	conn  *net.TCPConn
	enc   *json.Encoder

	// Stratum
	sync.Mutex
	latestId uint64
}

const (
	c_Max_Req_Size = 4096
)

func NewMinerConn(endpoint string) (*MinerSession, error) {
	remoteaddr, err := net.ResolveTCPAddr("tcp", endpoint)
	if err != nil {
		log.Fatalf("Error: %v", err)
		panic(err)
	}

	server, err := net.DialTCP("tcp", nil, remoteaddr)
	if err != nil {
		log.Fatalf("Error: %v", err)
		panic(err)
	}
	
	log.Printf("New TCP client made to: %v", server.RemoteAddr().String())

	return &MinerSession{proto: "tcp", ip: remoteaddr.AddrPort().Addr(), port: remoteaddr.Port, conn: server, latestId: 0, enc: json.NewEncoder(server)}, nil
}

// Reads raw data from TCP connection expecting a header to unmarshal.
// Puts received header into updateCh.
func (miner *MinerSession) ListenTCP(updateCh chan *types.Header) error {
	connbuff := bufio.NewReaderSize(miner.conn, c_Max_Req_Size)

	for {
		data, isPrefix, err := connbuff.ReadLine()
		if isPrefix {
			log.Printf("Socket flood detected from %s", miner.ip)
			return err
		} else if err == io.EOF {
			log.Printf("Client %s disconnected", miner.ip)
			return nil
		} else if err != nil {
			log.Printf("Error reading from socket: %v", err)
			return err
		}

		if len(data) > 1 {
			var rpcResp *rpc.JsonRPCResponse
			err := json.Unmarshal(data, &rpcResp)
			if err != nil {
				log.Printf("Unable to decode RPC Response: %v", err)
				return err
			}
			if rpcResp.Error != nil {
				log.Printf("Error received from proxy: %v", rpcResp.Error.Message)
				return errors.New(rpcResp.Error.Message)
			}

			var header *types.Header
			err = json.Unmarshal(*rpcResp.Result, &header)
			if err != nil {
				log.Printf("Unable to decode header: %v", err)
				return err
			}

			updateCh <- header
		}
	}
}

func (ms *MinerSession) SendTCPRequest(msg jsonrpc.Request) error {
	ms.Lock()
	defer ms.Unlock()

	return ms.enc.Encode(msg)
}
