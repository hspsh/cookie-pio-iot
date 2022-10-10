# Our IOT Project :)

## Setup

In order to get access to secret tokens used in this project, do this:

1. Setup your doppler account (you may login via github oauth)
2. Get invitation to hsp workspace
3. Authorize your machine, by using `doppler login` (follow instructions)
4. Run `doppler setup -p hsp-iot-secrets -c dev`
5. Generate `secrets.h` header file `./generate_secret_header.py`

## OTA

Disable firewall
