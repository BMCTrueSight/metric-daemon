# Metric Daemon

Daemon the generates random metric values for each of the different unit types and makes them available via a UDP channel.

## Platforms

Should work on all POSIX platforms including windows using cygwin

## Prerequistes

- gcc (Compile package)
- wget (Download package)

## Installation

1. Download the current release:

    ```bash
    wget https://github.com/BMCTrueSight/metric-daemon/releases/download/v0.2.0/metric-daemon-0.2.tar.gz
    ```

2. Extract the contents:

    ```bash
    tar xvf metric-daemon-0.2.tar.gz
    ```

3. Change directory to distribution

    ```bash
    cd metric-daemon-0.2
    ``` 

4. Configure:

    ```bash
    $ ./configure
    ```

5. Compile the distribution

    ```bash
    $ make
    ```

6. Install the binaries

    ```bash
    $ sudo make install
    ```

## Operation

You can start the daemon in the foreground by running the following:

1. Start the daemon:
    ```bash
    $ metricd
    The server UDP port number is 12345
    ```

2. Run the client:
   ```bash
   $ metric-client 127.0.0.1 12345 bytecount
   request: BYTECOUNT, response: 95514.000
   ```

## TODO

- Script to run as true daemon via init.d or systemd.


