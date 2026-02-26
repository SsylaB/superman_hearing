# 🦸‍♂️ SUPERMAN EAR: Directional Audio Isolation System

**Developed by:** Khadija Boulifa & Haifa Chkoundali  
**Platform:** Teensy 4.0 + Audio Shield  

In noisy environments, overlapping sounds degrade speech intelligibility, making it difficult to isolate a target voice. **SUPERMAN EAR** is a real-time digital signal processing (DSP) system that combines **Spectral Filtering** and **Spatial Beamforming** to "aim" hearing toward a specific direction while suppressing background noise.

---

## ⚙️ How It Works

The project operates through a three-stage modular architecture:

### 1️⃣ Dynamic Voice Filtering

To isolate human speech from environmental noise, the system focuses on the **Speech Spectrum**.

- **Band-Pass Filtering:** A digital filter isolates frequencies between **300 Hz and 3400 Hz**.  
  This removes low-frequency rumble (traffic, AC) and high-frequency electronic hiss.
- **Clarity Control:** Adjusting the *Clarity* dial sharpens the filter focus and raises a **Noise Gate** threshold to ensure silence during pauses in speech.

---

### 2️⃣ Digital Beamforming

Using two microphones spaced exactly **15 cm** apart, the system estimates the microscopic time delay **τ** of incoming sound.

- **Constructive Interference:** By digitally delaying one signal to align with the other, the system amplifies sound from the selected steering direction.
- **Phase Cancellation:** Sounds coming from other directions are naturally suppressed.
- **Time Delay Formula:**

$$
\tau = \frac{d \cdot \sin(\theta)}{v}
$$

Where:
- $d = 15\ \text{cm}$ (microphone spacing)
- $\theta$ = steering angle
- $v$ = speed of sound
---

### 3️⃣ Mixing and FX Module

The processed signal is routed through the Teensy Audio Library graph, allowing:

- Mode switching (Clarity / Clarity + FX )  

An optional **Reverb effect** can be applied to add spatial depth.

---

## 📊 System Architecture

The logical flow of the audio processing chain:
<img width="2294" height="975" alt="SHEMA_SON" src="https://github.com/user-attachments/assets/ee8e846f-cf56-4cb3-80b9-4633b699ca0e" />



## 📂 Project Structure (`src/` Directory)

The core logic is modularized into the following files:

- **`superman.ino`**  
  Main entry point. Initializes Teensy hardware, manages the execution loop, handles UI interaction, and coordinates DSP updates.

- **`AudioLogic.cpp` / `AudioLogic.h`**  
  Contains the mathematical logic for beamforming. Computes steering delays using TDOA principles and updates clarity and FX parameters.

- **`AudioObjects.h`**  
  Defines the Teensy Audio Library signal graph (I2S input, delay taps, mixers, filters, reverb, output routing).

- **`ProjectDefines.h`**  
  Centralized configuration file containing pin mappings, physical constants (e.g., speed of sound), and system parameters.

- **`UIHelpers.cpp` / `UIHelpers.h`**  
  Utility functions for hardware interaction:
  - Button debouncing  
  - Potentiometer smoothing  
  - LED state management  

---

## 🛠️ Hardware Requirements

| Component                | Quantity | Purpose                          |
|--------------------------|----------|----------------------------------|
| **Teensy 4.0**           | 1        | Main DSP Microcontroller         |
| **Teensy Audio Shield**  | 1        | High-quality Audio I/O           |
| **Microphones**          | 2        | Spaced 15 cm apart               |
| **Potentiometers**       | 3        | Steering, Clarity, Volume        |
| **Push Buttons**         | 2        | Mode selection & Bypass          |
| **LEDs**                 | 3        | System status indicators         |

---

## 📚 Sources & References

Technical principles regarding broadband signals and speech acoustics were sourced from:

- [Beamforming for Broadband Signals](https://wirelesspi.com/beamforming-for-broadband-signals/)  
  Spatial filtering and delay-and-sum architecture logic.

- [ProAV Audio: Speech Levels](http://www.proav.de/index.html?http&&&www.proav.de/audio/speech-level.html)  
  Spectral distribution and decibel levels of the human voice.

---

## 🚀 Getting Started

1. Clone this repository into your Arduino/Teensy project folder.
2. Install the **Teensyduino** add-on in your Arduino IDE.
3. Open and upload `src/superman/superman.ino` to your Teensy 4.0.
4. Use the **Serial Monitor** to observe system feedback and calibrate microphone spacing if needed.
