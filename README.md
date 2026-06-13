# EfM_algorithms_and_data_structures
Practice for the module 3 (Algorithms and data structures)

Here's the English translation of your `README.md`:

## Building the Project and Running Tests

The project uses **CMake Presets** to automate cross-platform builds and testing. All configurations use the **Ninja** generator and **vcpkg** package manager.

### 1. Requirements (Dependencies)

Before building, ensure that the following are installed and available in your `PATH` environment variable:
* **CMake** version **4.2.0** or higher
* **Ninja** (build generator)
* **A compiler** with C++20 support (e.g., GCC/Clang for Linux, MSVC for Windows)

---

### 2. Cloning the Repository

Since the repository already includes **vcpkg** as a submodule, use:

```bash
git clone --recurse-submodules -b develop https://github.com/IDJovenko/EfM_algorithms_and_data_structures.git
```

If you've already cloned without submodules:

```bash
git submodule update --init --recursive
```

---

### 3. Installing vcpkg (one-time setup)

```bash
# Linux / macOS
./vcpkg/bootstrap-vcpkg.sh

# Windows
.\vcpkg\bootstrap-vcpkg.bat
```

---

### 4. Viewing Available Presets

To see the list of available configurations for your current operating system, run:

```bash
cmake --list-presets
```

---

### 4. Building the Project

The process consists of two steps: configuration (generating files for Ninja) and the actual compilation. Choose the commands based on your OS and desired build type (Debug or Release).

#### For Windows:
```bash
# Debug
cmake --preset windows-debug
cmake --build --preset build-windows-debug

# Release
cmake --preset windows-release
cmake --build --preset build-windows-release
```

#### For Linux:
```bash
# Debug
cmake --preset linux-debug
cmake --build --preset build-linux-debug

# Release
cmake --preset linux-release
cmake --build --preset build-linux-release
```

*Build artifacts will be saved to the `build/windows-debug`, `build/linux-release`, etc. directories accordingly.*

---

### 5. Running Tests

Tests are run using the `CTest` utility by selecting the corresponding test preset.

#### On Windows:
```bash
# Tests for Debug
ctest --preset test-windows-debug

# Tests for Release
ctest --preset test-windows-release
```

#### On Linux:
```bash
# Tests for Debug
ctest --preset test-linux-debug

# Tests for Release
ctest --preset test-linux-release
```

---

### Working in an IDE

The preset file is automatically supported by modern development environments:
* **Visual Studio Code:** Install the `CMake Tools` extension. It will pick up the presets. Select the desired configuration in the Status Bar.
* **CLion / Visual Studio 2022:** Presets are imported when opening the project. You can switch between them in the top build profile control panel.

---

## Task 5. STL Container Benchmark

Performance comparison:`std::set` vs `std::unordered_set` (insert, search, delete) - `std::map` vs `std::unordered_map` - `std::vector` vs `std::list` (insert into middle, delete), introduced in [src\task5_benchmarks\README.md](src\task5_benchmarks\README.md)