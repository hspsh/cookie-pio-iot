#pragma once

{% if cookiecutter.add_mac_to_hostname == 'y' %}
#define APPEND_MAC_TO_HOSTNAME
{% endif %}
const char* const hostname = "{{ cookiecutter.device_hostname }}";
const char* const friendlyName = "{{ cookiecutter.device_friendly_name }}";
const char* const mqttHost = "{{ cookiecutter.device_mqtt_host }}";
const char* const fallbackMqttIp = "{{ cookiecutter.fallback_mqtt_ip }}";

