FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B docker-build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build docker-build --parallel

FROM build AS test

CMD ["ctest", "--test-dir", "docker-build", "--output-on-failure"]

FROM ubuntu:24.04 AS runtime

COPY --from=build /src/docker-build/rv32emu /usr/local/bin/rv32emu
COPY examples /examples

ENTRYPOINT ["/usr/local/bin/rv32emu"]