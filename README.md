# Intro

An ESP32 temp sensor which uses MQTT to publish readings

[![Build Status](https://travis-ci.org/re-ops/re-core.png)](https://travis-ci.org/narkisr/temp-sensor)

# Usage

PlatformIO is used to build this project:

```bash
$ export PLATFORMIO_BUILD_FLAGS="-D WIFI_PASSWORD=\\\"\\\" -D WIFI_SSID=\\\"\\\" -D HOSTNAME=\\\"\\\" -D MQTT_PASSWORD=\\\"\\\" -D MQTT_USER=\\\"\\\" -D MQTT_SERVER=\\\"\\\"" 
$ pio run -t upload
```


# Copyright and license

Copyright [2020] [Ronen Narkis]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
