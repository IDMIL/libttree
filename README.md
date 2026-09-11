# libttree: C++ library for bulk T-Stick management over a serial connection

## About

Coming soon

## Installation

Coming soon

## Quick Start

Create a `TTree` object, set your config, and immediately start watching for T-Sticks in the background:

```
#include "TTree.h"

void main() {
    TTree myTTree({
        .ssid = "my_wifi",              // Name of the network you're using
        .pw = "password",               // Network password
        .destinationIp = "192.168.1.1"  // Destination IP (where you're sending T-Stick data to)
        .destinationPort = "8000"       // Destination port
    });

    // ... do some stuff here ...
}
```

## Documentation

Coming soon

## Credits

Coming soon
