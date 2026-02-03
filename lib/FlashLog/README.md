# FlashLog
you're not supposed to directly use the functions here. most of them is QOL or is used in by each other. the config ones havent been done yet, and `getData()` will likely have bt functions as the parameters. look at [log transfer](#log-transfer-idea) section to see how it will be implemented.

## how the memory is structured
The entire flash memory is 2MB. this is how its structured. the config will not be 4KB. it will likely be 18 bytes

```
┌──────────────────────────────┐
│ 0x000000                     │
│ CONFIG                       │
│ 4 KB (1 sector)              │
└──────────────────────────────┘
┌──────────────────────────────┐
│ 0x001000                     │
│ LOGS                         │
│ 2,092 KB                     │
│ (511 sectors / ~31 blocks)   │
└──────────────────────────────┘
│ 0x200000 (end)               │
```

## log transfer idea
either bt functions will be passed down to `getData()` or `getData()` will call the bt functions itself. there should be two data transfer characteristics. sensor data and debug codes. as the functions scans through the log section, it will update and notify the corresponding characteristic. the hope is that it wont be too fast. if there is a better idea lmk or implement it.