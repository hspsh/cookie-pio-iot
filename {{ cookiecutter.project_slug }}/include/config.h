#pragma once

{% if cookiecutter.add_pyup_badge == 'y' %}
#define APPEND_MAC_TO_HOSTNAME
{% endif %}
const char* hostname = "{{ cookiecutter.device_hostname }}";
const char* friendlyName = "{{ cookiecutter.device_friendly_name }}";
const char* mqttHost = "{{ cookiecutter.device_mqtt_host }}";
