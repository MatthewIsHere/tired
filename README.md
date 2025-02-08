# Tired

**Tired** is a command-line utility designed to manage screen brightness by gradually dimming the display after a specified period of inactivity. This tool replicates the grace period and sleep dimming behavior found in macOS systems.

## Acknowledgments

This project is inspired by Simon Ser's [chayang](https://git.sr.ht/~emersion/chayang), a tool that gradually dims the screen on Wayland. Through studying chayang, I gained valuable insights into Wayland development, which informed the creation of Tired.

## Features

- **Customizable Hold Time**: Define the duration of inactivity before the screen begins to dim.
- **Adjustable Dim Speed**: Control the rate at which the screen dims once the hold time has elapsed.
- **Configurable Dim Brightness**: Set the brightness level during the dim period.
- **Pre-Exit Command Execution**: Optionally execute a specific command before the program exits.

## Installation

To install **Tired**, follow these steps:

### 1. Install Dependencies

Ensure the following dependencies are installed:

- **CMake** (version 3.10 or higher): A cross-platform build system.
- **Wayland Client Libraries**: Required for Wayland support.

#### Debian/Ubuntu:
```bash
sudo apt-get update
sudo apt-get install cmake libwayland-client-dev
```

#### Arch Linux:
```bash
sudo pacman -S cmake wayland
```

### 2. Clone the Repository

```bash
git clone https://github.com/yourusername/tired.git
cd tired
```

### 3. Build the Project

Create a build directory and compile the project using CMake:

```bash
mkdir build
cd build
cmake ..
make
```

### 4. Install the Executable

After a successful build, install the `tired` executable:

```bash
sudo make install
```

By default, this installs `tired` to `/usr/local/bin/`. You can specify a different installation directory by setting the `CMAKE_INSTALL_PREFIX` during the CMake configuration step:

```bash
cmake -DCMAKE_INSTALL_PREFIX=/your/desired/path ..
```

## Usage

```bash
Usage: tired [options]

Options:
  -t, --hold-time <ms>         Set hold time in milliseconds (default: 15000)
  -d, --dim-speed <ms>         Set dim speed in milliseconds (default: 250)
  -b, --brightness <0-100>     Set the brightness of the display when in dim period (default: 30)
  -c, --command <cmd>          Command to execute before exiting
  -h, --help                   Show this help message
```

## Examples

**My personal usage**:

```bash
tired -d 20000 -c "swaymsg output eDP-1 power off; gtklock -d"
```

In this example:

- `-d 20000`: Sets the dim speed to 20,000 milliseconds.
- `-c "swaymsg output eDP-1 power off; gtklock -d"`: Executes the command to turn off the `eDP-1` display and then locks the screen using `gtklock` before exiting.

## Contributing

We welcome contributions! If you'd like to contribute to **Tired**, please follow these steps:

1. **Fork the Repository**: Click the "Fork" button at the top right of this page.
2. **Clone Your Fork**:

   ```bash
   git clone https://github.com/yourusername/tired.git
   cd tired
   ```

3. **Create a New Branch**:

   ```bash
   git checkout -b feature/your-feature-name
   ```

4. **Make Your Changes**: Implement your feature or fix.
5. **Commit Your Changes**:

   ```bash
   git commit -m "Add your commit message here"
   ```

6. **Push to Your Fork**:

   ```bash
   git push origin feature/your-feature-name
   ```

7. **Open a Pull Request**: Navigate to the original repository and open a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.