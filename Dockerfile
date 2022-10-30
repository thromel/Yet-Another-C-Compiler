# Use Ubuntu as base image
FROM ubuntu:20.04

# Avoid timezone prompt during installation
ENV DEBIAN_FRONTEND=noninteractive

# Set working directory
WORKDIR /app

# Install required packages for building and running the compiler
RUN apt-get update && apt-get install -y \
    g++ \
    flex \
    bison \
    make \
    dos2unix \
    && rm -rf /var/lib/apt/lists/*

# Copy source files to container
COPY . .

# Fix line endings and make script executable
RUN dos2unix script.sh || true && \
    chmod +x script.sh

# Set default entrypoint and command
# This will run the compiler with the input file provided as an argument
ENTRYPOINT ["./script.sh"]
CMD ["input.c"]