FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    cmake \
    git \
    libasio-dev \
    libcurl4-openssl-dev \
    libgmock-dev \
    libgtest-dev \
    libxml2-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_ASAN=OFF \
    && cmake --build build \
        --target energyPricesServer \
        --parallel


FROM ubuntu:22.04 AS runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    libcurl4 \
    libxml2 \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --no-create-home energyprices

WORKDIR /app

COPY --from=builder /src/build/bin/energyPricesServer .
COPY --chown=energyprices:energyprices static ./static

USER energyprices

EXPOSE 18080

HEALTHCHECK --interval=30s --timeout=5s --retries=3 \
    CMD curl --fail --silent http://localhost:18080/ >/dev/null || exit 1

ENTRYPOINT ["./energyPricesServer"]
