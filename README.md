# CH32V003 Smart Home Hub (Bare-Metal)

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Platform](https://img.shields.io/badge/platform-CH32V003-blue)]()
[![Status](https://img.shields.io/badge/status-WIP--Alpha-orange)]()

A lightweight, bare-metal firmware implementation for a distributed smart home sensor network. This project leverages the **WCH CH32V003** (RISC-V QingKe V2 core) to create sub-$1.00 edge nodes that communicate sensor data to a central gateway (or MQTT broker).

> **Current Status:** *Foundational HAL and peripheral drivers are being laid out. The architecture is defined, but application logic is in active development.*

---

## 🎯 Project Vision
The goal is to move away from heavy ESP8266/ESP32 overhead for simple tasks (temperature, light, motion) and instead use the CH32V003 for its extreme low cost and low power consumption. The firmware is written **entirely in C without an RTOS** (bare-metal) to minimize latency and memory footprint (2KB RAM / 16KB Flash).
