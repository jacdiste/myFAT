# myFAT – FAT File System Simulation

A virtual file system based on FAT (File Allocation Table), written in C.

## 📌 Description

**myFAT** is an academic project that simulates a custom FAT-like file system, entirely implemented in C. It emulates the basic behavior of a real file system, allowing file and directory management in a simulated environment.

The system supports:
- Creating and removing files and directories
- Navigating through a hierarchical structure
- Dynamic allocation and deallocation of blocks
- Managing the FAT (File Allocation Table)
- A textual interface for user interaction

## ⚙️ Main Features

- 🌲 Hierarchical structure: nested directories, absolute and relative paths
- 💾 FAT management: block allocation and deallocation on a virtual disk
- 📝 Interactive commands: `mkdir`, `touch`, `cd`, `ls`, `rm`, `write`, `read`, etc.
- 📦 Disk simulation: read/write to binary files simulating physical storage

## 🧱 Project Structure

```
myFAT/
├── src/               // C source files
├── include/           // Header files
├── disk/              // Simulated virtual disk
├── Makefile           // Build automation
└── README.md          // Project documentation
```

## 🚀 Usage

### Requirements
- Unix-like system (Linux or macOS)
- GCC (C compiler)
- `make` installed

### Build
```bash
make
```

### Run the file system
```bash
./myfat
```

After launching, a custom textual shell will appear where you can use commands to interact with the virtual file system.

## 🎓 Educational Goals

This project was developed to explore:
- The internals of real file systems
- Low-level memory management
- Use of complex data structures in C
- Modular system software design

## 👤 Author

**Jacopo Di Stefano**  
Computer Engineering student – Sapienza University of Rome  
[GitHub – @jacdiste](https://github.com/jacdiste)

## 📄 License

Distributed under the MIT License.  
See the `LICENSE` file for more details.