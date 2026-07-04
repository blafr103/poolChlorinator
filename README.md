# Cholomatic - Automated Liquid Chlorine Dispenser

An Arduino-based system that monitors an above-ground pool and doses liquid chlorine automatically, built as a client-driven accessibility project. The client has reduced mobility, so the goal was to remove the physical effort and manual chemistry-testing from routine pool maintenance.

> **🏆 Category winner - Accessibility 4, uOttawa Faculty of Engineering Design Day, Winter 2023 (Team Cholomatic).**
> Publicly listed at [uOttawa CEED - Past Winners](https://www.uottawa.ca/faculty-engineering/centre-entrepreneurship-engineering-design/past-winners) under the 2022-2023 year filter.

## Overview

The system sits inline with an existing pool filtration setup and runs an autonomous control loop:

1. Measure the chlorine level remaining in the reservoir (ultrasonic distance sensing).
2. Read pool chemistry from an analog sensor and compute the dose needed to reach the target level.
3. If a dose is warranted and the reservoir has enough chlorine, run the pump for a calculated time to inject it.
4. Drive status LEDs for low reservoir, active dispensing, and over-target chemistry.

A one-directional injection tee feeds chlorine into the existing pool plumbing. A reconfigurable valve path also lets the same pump refill the reservoir directly from a purchased chlorine container, so the user never has to lift or pour.

**Scope and status:** this was a second-year cornerstone design-course prototype (GNG2501), not a production product. It reached a working physical prototype with validated level-sensing and a full plumbed enclosure. See [Retrospective notes](#retrospective-notes) for known limitations.

## How it works

**Reservoir level.** An HC-SR04 ultrasonic sensor (via the `NewPing` library) measures the distance to the chlorine surface. `pingRes()` takes a 5-ping median to reject outliers, converts to a fill percentage, and returns -1 on a sensor fault rather than reporting a false "full."

**Dose calculation.** `getCapteurPpm()` averages 10 analog samples and converts to a chemistry reading. `getChloreNecessaire()` uses that reading, the pool volume, and a target level to compute the chlorine volume required, returned in ounces and converted to litres in the main loop.

**Actuation and timing.** The main loop uses a non-blocking `millis()` gate so the system evaluates a dose at most once per interval (default one hour) instead of blocking with `delay()`. Pump run time is derived from the required dose volume and the pump flow rate.

**Safety logic.**
- Pump only runs when the reservoir is above a minimum threshold, to avoid running the pump dry.
- Blue LED when the reservoir drops below the fill-warning percentage.
- Red LED while dispensing is active.
- Orange LED when the computed dose is negative beyond a margin (pool already over target).

## Hardware

| Component | Role |
| --- | --- |
| Arduino Uno | System controller |
| HC-SR04 ultrasonic sensor | Reservoir chlorine level |
| Analog chemistry sensor | Pool level reading |
| Relay module | Pump switching |
| 12V pump | Chlorine injection and reservoir refill |
| Status LEDs (blue / red / orange) | Reservoir, dispensing, over-target alerts |
| PEX plumbing, valves, tees | Distribution and refill paths |
| Reservoir + enclosure | Chlorine storage, weatherproofed |

Full bill of materials with quantities and costs is in [Deliverable C](docs/GNG2501%20-%20Livrable%20C.pdf).

## Firmware

Source: [`src/chlorine_dispenser.ino`](src/chlorine_dispenser/chlorine_dispenser.ino)

Dependencies: [`NewPing`](https://bitbucket.org/teckel12/arduino-new-ping/) for the ultrasonic sensor.

Key functions:
- `pingRes()` - reservoir level as a percentage, with fault detection
- `getCapteurPpm()` - averaged analog chemistry read
- `getChloreNecessaire()` - dose volume from current reading, pool volume, and target
- `allumerLumiere()` - low-reservoir LED
- `allumerPompe()` - timed pump run with dispensing LED
- `loop()` - non-blocking scheduler, safety gating, and LED state

Control-flow diagram: `images/code_flowchart.png` (export Figure 3 from Deliverable C).

## Prototypes and results

**Prototype 1 - breadboard.** Ultrasonic level sensing with LED alerts, validated against a ruler. Level accuracy stayed within roughly 4% at 10 cm and tightened to under 2% at 40 cm. Low-level alerting triggered within a few tenths of a percent of the 20% threshold.

**Prototype 2 - full build.** Plumbed enclosure with reservoir, PEX distribution and refill paths, and mounted electronics. Assemble/disassemble time was tested with independent users and came in well under the 30-minute target. Sealing and equivalent-length pressure-loss tests are documented in [Deliverable E](docs/GNG2501%20-%20Livrable%20E.pdf).

Photos:
- `images/prototype1_circuit.png` (Deliverable D)
- `images/prototype2_full.png` (Deliverable E, Figure 7)

## Repository structure

```
.
├── README.md
├── src/
│   └── chlorine_dispenser/
│       └── chlorine_dispenser.ino
├── experiments/
│   ├── pump_alone/
│   │   └── pump_alone.ino
│   └── chlore_level_detector/
│       └── Detecteur_niveau_chlore_copy_20230318203757.ino
├── docs/
│   ├── GNG2501 - Livrable B_mod.pdf
│   ├── GNG2501 - Livrable C.pdf
│   ├── GNG2501 - Livrable D.pdf
│   └── GNG2501 - Livrable E.pdf
└── images/
    ├── code_flowchart.png
    ├── prototype1_circuit.png
    └── prototype2_full.png
```

## Design documentation

Full design process, in French (uOttawa GNG2501):

- [Deliverable B](docs/GNG2501%20-%20Livrable%20B_mod.pdf) - problem definition, needs analysis, benchmarking, target specs, concept development
- [Deliverable C](docs/GNG2501%20-%20Livrable%20C.pdf) - detailed design, bill of materials, code-flow diagram
- [Deliverable D](docs/GNG2501%20-%20Livrable%20D.pdf) - prototype 1 and level-sensing test results
- [Deliverable E](docs/GNG2501%20-%20Livrable%20E.pdf) - design constraints and full physical prototype

## Retrospective notes

The firmware carries later annotations flagging issues found on review, kept in the source as a record of the fixes:

- A dispense-interval constant originally declared as `int` would overflow (16-bit signed, max 32767), silently turning a one-hour delay into about 32 seconds. Corrected to `unsigned long`.
- The dosing model reads an analog sensor and applies a placeholder calibration and conversion. The client target was specified in ppm, and reconciling the sensor reading to a calibrated ppm value is the main open item for a real deployment.

## Team and contributions

Four-person team for GNG2501 (Team Cholomatic): Bangali Traoré, Benoit Lafrance, Imad Eddin Tijani, Nyckolas Gagné.

My primary contributions were the embedded firmware and the Arduino/electronics integration.

## Course context

GNG2501 - Introduction à la gestion et au développement de produit en génie et en informatique - Engineering Design, Faculty of Engineering, University of Ottawa, Winter 2023. Client-driven accessibility project.
*NOTE* - PDF documents may have mis-typed year in date mentions. Any reference to 2022 should be 2023.
