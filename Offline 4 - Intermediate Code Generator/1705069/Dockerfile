# Use Ubuntu as base image
FROM ubuntu:20.04

# Avoid timezone prompt during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    g++ \
    flex \
    bison \
    make \
    dos2unix \
    && rm -rf /var/lib/apt/lists/*

# Create working directory
WORKDIR /app

# Copy your source files
COPY . .

# Fix line endings and make script executable
RUN dos2unix script.sh || true && \
    chmod +x script.sh

# Default command to run your script with an input file
ENTRYPOINT ["./script.sh"]
CMD ["input.c"]