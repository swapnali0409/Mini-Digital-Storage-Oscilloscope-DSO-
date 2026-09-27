<img width="845" height="716" alt="image" src="https://github.com/user-attachments/assets/1b44fd2b-d292-484a-b7f0-53829ac23832" />
# Mini Digital Storage Oscilloscope (DSO)

A compact, low-cost **Mini Digital Storage Oscilloscope (DSO)** developed using an **ESP8266 microcontroller** and a **TFT display** for real-time signal acquisition, waveform visualization, and basic frequency-domain analysis using **FFT**.

The project was developed to understand the practical concepts behind oscilloscopes, digital signal sampling, ADC-based signal acquisition, SPI communication, waveform rendering, and signal processing.

---

## 📌 Project Overview

An oscilloscope is an electronic test instrument used to observe and analyze electrical signals.

In this project, the **ESP8266** samples an input analog signal through its ADC, processes the acquired samples, and displays the waveform on a **TFT LCD**.

The project also includes an **FFT-based frequency analysis mode**, which allows the dominant frequency components of the input signal to be observed.

### Main Functions

* Real-time analog signal acquisition
* Waveform visualization on TFT display
* Adjustable time/division and voltage/division
* Frequency analysis using FFT
* Signal sampling using ADC
* SPI communication with TFT display
* Embedded programming using Arduino IDE

---

## ✨ Features

* 📈 Real-time waveform display
* 🔌 ADC-based signal acquisition
* 🖥️ TFT graphical display
* 📊 FFT frequency spectrum analysis
* ⏱️ Adjustable time scale
* ⚡ Adjustable voltage scale
* 🔄 Continuous signal sampling
* 🎛️ Multiple display/control modes
* 💻 ESP8266-based embedded implementation

---

## 🛠️ Hardware Used

| Component                      | Purpose                            |
| ------------------------------ | ---------------------------------- |
| ESP8266                        | Main microcontroller               |
| ST7789 TFT Display             | Waveform and data visualization    |
| Signal Input                   | Analog signal source               |
| Resistors / Passive Components | Signal conditioning and protection |
| Connecting Wires / PCB         | Hardware connections               |

---

## 🔧 Software & Tools

* **Arduino IDE**
* **Embedded C/C++**
* **ESP8266 Arduino Core**
* **TFT_eSPI / TFT display library**
* **arduinoFFT library**
* Serial Monitor / PuTTY for debugging

---

## 🔌 Hardware Connections

### ESP8266 → ST7789 TFT

| TFT Pin    | ESP8266     |
| ---------- | ----------- |
| VCC        | 3.3V        |
| GND        | GND         |
| SCLK / SCK | GPIO14 (D5) |
| MOSI       | GPIO13 (D7) |
| CS         | GPIO15 (D8) |
| DC         | GPIO4 (D2)  |
| RST        | GPIO2 (D4)  |

> **Note:** Pin assignments may vary depending on the hardware version and code configuration.

### Analog Input

The analog signal is connected to the **ADC input of the ESP8266**.

**Important:** The ESP8266 ADC has a limited input voltage range. The input signal should therefore be properly scaled/conditioned before connecting it to the ADC.

---

## ⚙️ Working Principle

The overall operation can be understood in the following steps:

```text
        Input Signal
             │
             ▼
      Signal Conditioning
             │
             ▼
       ESP8266 ADC
             │
             ▼
       Sample Acquisition
             │
       ┌─────┴─────┐
       │           │
       ▼           ▼
 Waveform Mode   FFT Mode
       │           │
       ▼           ▼
   TFT Display   Frequency
                  Spectrum
```

### 1. Signal Acquisition

The analog input signal is sampled using the **ADC of the ESP8266**.

Multiple samples are collected at a defined sampling rate.

For example:

```text
Analog Signal
     │
     │      /\        /\
     │     /  \      /  \
     │____/    \____/    \____
          ↓ ↓ ↓ ↓ ↓ ↓ ↓
         ADC Samples
```

The quality of the displayed waveform depends on the sampling rate and number of samples collected.

---

### 2. Waveform Processing

The acquired ADC values are converted into display coordinates.

The values are scaled according to the selected voltage/division setting and then plotted on the TFT display.

The horizontal axis represents **time**, while the vertical axis represents **signal amplitude**.

---

### 3. TFT Display

The TFT display communicates with the ESP8266 using **SPI communication**.

The display is used to show:

* Input waveform
* Grid
* Voltage scale
* Time scale
* Signal information
* FFT spectrum

The **CS** and **DC** pins are used to control communication with the display.

---

### 4. FFT Analysis

The project uses the **arduinoFFT library** to perform Fast Fourier Transform analysis.

FFT converts the sampled signal from the **time domain** into the **frequency domain**.

```text
Time Domain
    │
    │  /\      /\
    │ /  \    /  \
    │/    \__/    \
    │
    └──────────────► Time

             FFT
              │
              ▼

Frequency Domain
    │
    │        │
    │        │
    │        │
    │   │    │
    │___│____│________► Frequency
```

This makes it possible to identify the dominant frequency components present in the input signal.

---

## 📊 Signals Tested

The system can be tested using different types of signals such as:

### Sine Wave

A sine wave mainly produces a strong fundamental frequency component.

### Square Wave

A square wave contains the fundamental frequency along with odd harmonics.

For an ideal square wave:

```text
f, 3f, 5f, 7f, ...
```

### Triangle Wave

A triangle wave also contains mainly odd harmonics, but their amplitudes decrease faster than those of a square wave.

```text
f, 3f, 5f, 7f, ...
```

These signals were useful for understanding the relationship between waveform shape and its frequency spectrum.

---

## 📐 Important Oscilloscope Concepts

### Time/Division

Time/division determines how much time is represented by each horizontal grid division.

For example:

```text
Time/Div = 100 µs/div
```

means each horizontal division represents 100 microseconds.

---

### Voltage/Division

Voltage/division determines the voltage represented by each vertical grid division.

For example:

```text
Voltage/Div = 0.5 V/div
```

means each vertical division represents 0.5 V.

---

### Sampling Frequency

The sampling frequency determines how frequently the analog signal is sampled.

According to the **Nyquist sampling principle**, the sampling frequency should be at least twice the highest frequency component that needs to be measured:

```text
Fs ≥ 2 × Fmax
```

In practical systems, a higher sampling frequency is generally preferred to provide better signal representation.

---

## 🧠 Key Technical Concepts Learned

Through this project, we gained practical experience with:

* ADC and analog signal acquisition
* Sampling theory
* Nyquist theorem
* Time-domain signal representation
* Frequency-domain representation
* Fast Fourier Transform (FFT)
* SPI communication
* TFT display interfacing
* Embedded C/C++ programming
* Signal scaling and visualization
* Real-time data processing
* Hardware-software integration
* Circuit debugging

---

## 🧩 Project Challenges

Some of the important challenges encountered during development included:

### ADC Voltage Limitation

The ESP8266 ADC has a limited input voltage range, so the input signal must be kept within the safe ADC range.

### Real-Time Display

The ESP8266 needs to acquire samples, process them, and update the TFT display efficiently without causing excessive delays.

### Sampling and FFT

The sampling rate and number of samples directly affect the quality and frequency resolution of FFT analysis.

### SPI Display Communication

Correct configuration of **SCK, MOSI, CS, DC, and RST** pins was required for reliable TFT communication.

---

## 🚀 Future Improvements

The project can be further improved by adding:

* Higher-speed ADC
* Better input signal conditioning
* AC/DC coupling
* Automatic voltage scaling
* Automatic frequency measurement
* Trigger functionality
* Peak-to-peak voltage measurement
* RMS voltage measurement
* Frequency measurement
* More accurate time-base control
* Data logging
* PC-based waveform visualization
* Improved PCB design
* Higher bandwidth signal acquisition

---

## 📁 Project Structure

```text
Mini-DSO/
│
├── Mini_DSO/
│   ├── Mini_DSO.ino
│   └── ...
│
├── Circuit/
│   ├── circuit_diagram.png
│   └── ...
│
├── Images/
│   ├── hardware.jpg
│   ├── waveform.jpg
│   └── fft.jpg
│
├── Documentation/
│   └── project_report.pdf
│
└── README.md
```

## 📸 Project Demonstration
<img width="555" height="713" alt="image" src="https://github.com/user-attachments/assets/48b39445-4e6f-4f1d-afa3-b3021fe0588c" />


<img width="845" height="716" alt="image" src="https://github.com/user-attachments/assets/73c559bf-d362-4c60-8ae3-24770fd9ce08" />


[<img width="837" height="710" alt="image" src="https://github.com/user-attachments/assets/038edc8a-a339-4ac9-9f6c-808c74c0e2de" />

```

## 🎯 Project Outcome

This project provided hands-on experience in designing and implementing a small embedded measurement system.

It helped us understand how **analog signals are sampled, processed, and converted into useful visual information** using a microcontroller and display.

The project also strengthened our understanding of **embedded systems, signal processing, hardware interfacing, debugging, and teamwork**.

---

## 📜 License

This project is intended for educational and learning purposes.

Feel free to explore, modify, and improve the project.

---

## ⭐ Acknowledgement

We would like to thank everyone who supported and guided us during the development of this project.

The project helped us strengthen our practical knowledge of **embedded systems, electronics, signal processing, and microcontroller programming**.
