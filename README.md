# EmbeddedSystems3
# Bare Metal OS: CLI, Multimedia, and Game Development

## Overview
This project implements a bare-metal operating system that includes:
1. A Command Line Interface (CLI) with features like autocomplete, command history, and basic system commands.
2. Multimedia capabilities, including image and video display.
3. A simple bubble shooter game inspired by "Dynomite."

This was developed as part of the **EEET2490 Embedded Systems: OS and Interfacing** course.

---

## Features

### Command Line Interface (CLI)
- **Welcome Screen:** Custom ASCII art displayed during boot.
- **Core Commands:**
  - `help`: Lists all commands or provides specific details for a command.
  - `clear`: Clears the terminal.
  - `showinfo`: Displays system or board information.
  - `baudrate` and `stopbits`: Configures UART settings.
  - `checkbaudrate` and `checkstopbits`: Displays current UART configurations.
  - `display image` and `play video`: Initiates multimedia tasks.
  - `game`: Launches the embedded game.
  - `exit`: Shuts down the system.
- **Enhancements:**
  - Command history buffer (up to 10 commands).
  - Autocomplete for standard and help commands.
  - Backspace and delete support.

### Multimedia Features
- **Image Display:**
  - Displays a background image with team member names overlayed.
  - Image can be moved using `W`, `A`, `S`, `D` keys.
- **Video Playback:**
  - Plays a 51-frame video smoothly on QEMU using frame buffering and timing.

### Game: Bubble Shooter
- **Gameplay Mechanics:**
  - Match 3+ balls of the same color to clear them.
  - Shooter control: Rotate with `A`/`D` keys and shoot with `Space`.
  - Scoring system based on accuracy and speed.
  - Increasing difficulty with faster ball drops as score increases.
- **Core Features:**
  - Collision detection with walls and targets.
  - Animated explosion effects for cleared balls.

---

## Dependencies
- **QEMU Emulator:** For running the OS.
- **Raspberry Pi ARM Toolchain:** For cross-compilation.
- **Frame Buffer Manipulation:** For image and video rendering.
- **UART Communication:** For CLI implementation.

---

## How to Run
1. Clone this repository:
   ```bash
   git clone <repository_url>
   ```
2. Build the project:
   ```bash
   make
   ```
3. Launch the OS using QEMU:
   ```bash
   make run
   ```

---

## Known Limitations
- **Command History:** Limited to 10 commands.
- **Game Mechanics:**
  - Balls do not fall when unsupported.
  - Inconsistent ball placement in the grid.
- **UI Design:** Minimalistic, with room for improvement.

---

## Future Enhancements
- Improve UI for a more engaging experience.
- Refine ball physics for better gameplay.
- Expand the game with additional levels and mechanics.

---

## Authors
- **Team 01:** Le Thien Son, Ho Quang Huy, Trinh Phuong Thao, Phung Thi Minh Anh

---

## Acknowledgments
- Lecturer: Dr. Linh Tran
- References:
  - [Online ASCII Art Generator](https://onlinetools.com/ascii/convert-text-to-ascii-art)
  - [Image to Pixel Converter](https://javl.github.io/image2cpp/)
  - [Video Frame Extractor](https://www.onlineconverter.com/extract-image-from-video)
