# Our IOT Project :)

## Basic requirements

This project is based on platformio, an add-on for visual studio code.

## Setup

In order to get access to secret tokens used in this project, do this:

1. Setup your doppler account (you may login via github oauth)
2. Get invitation to hsp workspace
3. Authorize your machine, by using `doppler login` (follow instructions)
4. Run `doppler setup -p hsp-iot-secrets -c dev`
5. Generate `secrets.h` header file `./generate_secret_header.py`

## OTA ("over the air" programming)

In order for our script to work properly, one must execute it in `platformio` environment. You can acquire it by clicking "terminal" icon in VSCode's bottom bar.

In this terminal, run `upload_ota.sh <target device IP>`

Take care, OTA may not work if your computer has firewall active.

## Serial monitoring issues

If monitoring keeps the target in reset state, manually perform DTR/RTS sequence by typing following keys in order:

`CTRL + [T,D,T,R]`