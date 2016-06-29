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
    wget https://github.com/BMCTrueSight/metric-daemon/archive/v0.0.1.tar.gz
    ```
2. Extract the contents:
    ```bash
    tar xvf v0.0.1.tar.gz

3. Change directory to distribution
    ```bash
    cd blah
    ``` 
4. Configure:
    ```
    $ ./configure
    ```
5. Compile the distribution
    ```
    $ make
    ```
6. Install the binaries
    ```
    $ sudo make install
    ```

## Operation

You can start the daemon in the foreground by running the following:

    ```
    $ metricd
    The server UDP port number is 12345
    ```

You can test that the daemon is working correctly by using the accompanying client command:

    ```
    $ metric-client 127.0.0.1 12345 bytecount
    request: BYTECOUNT, response: 95514.000
    ```

## TODO

- Script to run as true daemon via init.d or systemd.


