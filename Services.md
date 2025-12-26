# Device Information
| Characteristic Name | Description | UUID |
| ------------------- | ----------- | ---- |
Firmware| Do not set. it will take the bootloader version|
Software | set with GH tag. must update in [platform.ini](/platformio.ini)
Hardware | whatever revision of hardware we are using. i set it to 0.0.1 for a barebones MCU setup

# Data
includes sensor data and debug messages

# configuration data
## Time

| Characteristic Name | Description | UUID |
| ------------------- | ----------- | ---- |
| `BLETis`| Timer Service | `043f0000-0ff5-45d1-9502-db9d40757da2`|
| `time` | Time in Unix | `043f0001-0ff5-45d1-9502-db9d40757da2`|
| `tz` | timezone adjustment (+-5 or so) | `043f0002-0ff5-45d1-9502-db9d40757da2` |
