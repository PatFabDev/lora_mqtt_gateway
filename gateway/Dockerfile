# ------------------------------------------------------------
# Build Stage
# ------------------------------------------------------------
FROM debian:trixie-slim AS builder

# Install build tools
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        libgpiod-dev \
        libmosquitto-dev && \
    rm -rf /var/lib/apt/lists/*

# Working directory
WORKDIR /app

# Copy project
COPY . .

# Build application
RUN make

# ------------------------------------------------------------
# Runtime Stage
# ------------------------------------------------------------
FROM debian:trixie-slim

# Install runtime libraries
RUN apt-get update && \
    apt-get install -y \
        libgpiod3 \
        libmosquitto1 && \
    rm -rf /var/lib/apt/lists/*

# Working directory
WORKDIR /app

# Copy executable
COPY --from=builder /app/lora_mqtt_gateway .

# Start gateway
CMD ["./lora_mqtt_gateway"]