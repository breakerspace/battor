# BattOr: Mobile device power monitor

## Directory structure

* sw - PC software for collecting power measurements from the BattOr

## Installation

### Software

**Dependencies**
* automake
* autoconf
* make

The software should build and run on most variants of Linux, Mac, as well as Windows (in Cygwin). The build and install instructions are as follows:

    $ ./bootstrap
    $ ./configure
    $ make
    $ sudo make install

To run BattOr without root on Linux you will need to add your user to the "dialout" group. You need to logout and login for these settings to take effect.

    $ sudo usermod -a -G dialout <userName>

## Usage

### Desktop operation mode

**LED status codes:**
* Blinking *YELLOW*: Idle, waiting to start next trace.
* Blinking *RED*: Recording trace.
* Solid *RED*: Downloading trace.
* Blinking *GREEN*: Streaming trace.

**Usage**

There is only currently one mode of operation, USB mode from `/dev/ttyUSB0`.
Simply run `battor` or `sudo battor`.
