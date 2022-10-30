#pragma once

#define APPEND_MAC_TO_HOSTNAME
const char* hostname = "{{ cookiecutter.device_hostnome }}";
const char* friendlyName = "{{ cookiecutter.device_friendly_name }}";
const char* mqttHost = "{{ cookiecutter.device_mqtt_host }}";
