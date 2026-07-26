---
title: Arduino
summary: AI on dev board
---

Remember, **libspot** is a modest `C` library that could run on modest systems.
Since version `3.0.1`, we ship a dedicated [Arduino library](https://docs.arduino.cc/libraries/), called **Spot**.
You can then directly import library code into your next IoT project.

!!! info ""
    **libspot** source code is licensed under `LGPLv3`. As the code is likely to be directly compiled into your final program,
    you should probably share it in a way. The simplest way to comply is to just include a copy of this library's source (unmodified or with your changes clearly marked) alongside your project.

## Install

Just grab the archive from the latest release, and extract it to the default libraries directory.

//// tab | Linux

```bash
curl -L -o /tmp/Spot.zip "https://github.com/asiffer/libspot/releases/latest/download/Spot.zip"
unzip -o /tmp/Spot.zip -d ~/Arduino/libraries/
```

////

//// tab | Windows

```powershell
Invoke-WebRequest -Uri "https://github.com/asiffer/libspot/releases/latest/download/Spot.zip" -OutFile "$env:TEMP\Spot.zip"; Expand-Archive -Path "$env:TEMP\Spot.zip" -DestinationPath "$env:USERPROFILE\Documents\Arduino\libraries" -Force
```

////

//// tab | MacOS

```bash
curl -L -o /tmp/Spot.zip "https://github.com/asiffer/libspot/releases/latest/download/Spot.zip"
unzip -o /tmp/Spot.zip -d ~/Documents/Arduino/libraries/
```

////


## Example

<!-- prettier-ignore -->
```arduino
--8<-- "arduino/Spot/examples/Basic/Basic.ino"
```
