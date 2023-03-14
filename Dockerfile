FROM golang:1.19-alpine as builder
RUN apk add --no-cache gcc musl-dev linux-headers git

ADD . /quai-cpu-miner

WORKDIR /quai-cpu-miner

RUN env GO111MODULE=on go build -o ./build/bin/quai-cpu-miner main.go

# Stage 2
FROM golang:1.19-alpine

COPY --from=builder /quai-cpu-miner/build/bin ./build/bin

WORKDIR ./

CMD ./build/bin/quai-cpu-miner $REGION $ZONE 1
