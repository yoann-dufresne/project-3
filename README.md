# project^3

## CLI interactions

We suppose that the nano is linked to the computer via /dev/ttyUSB0

```bash
  # Compilation
  arduino-cli compile --fqbn arduino:avr:nano .
  # Upload
  arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano . -v
  # Serial read configuration
  stty -F /dev/ttyUSB0 raw 115200
  # serial read
  cat /dev/ttyUSB0
```

