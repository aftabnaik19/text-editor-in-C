
# Terminal-Based Text Editor

A lightweight, terminal-based text editor implemented in C. Inspired by minimalistic text editors like `vim`, this editor provides basic text manipulation and editing features.

---

## Features

- **Insert Mode and Normal Mode**: Switch between input mode for editing and normal mode for navigation.
- **Undo/Redo Stack**: Supports undo and redo operations for efficient editing.
- **File Saving**: Save your edits to a file directly from the editor.
- **Line Operations**: Add, delete, or navigate through lines of text.
- **Cursor Navigation**: Move the cursor up, down, left, and right using arrow keys or `hjkl`.
- **Header and Footer Display**: Provides real-time file and cursor position information.

---

## Requirements

- A C compiler (GCC recommended).
- A terminal that supports ANSI escape codes.
- Any Linux Distro 

---

## Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```
2. Navigate to the project directory:
   ```bash
   cd terminal-text-editor
   ```
3. Compile the program:
   ```bash
   gcc text_editor.c -o text_editor
   ```
4. Run the editor:
   ```bash
   ./text_editor <filename>
   ```

---

## Usage

### Modes
- **Normal Mode**: Default mode. Used for navigation and commands.
  - Enter `i` to switch to Insert Mode.
  - Use `hjkl` for navigation.
  - Use `u` to undo and `U` to redo changes.
  - Use `d` to delete a line.
- **Insert Mode**: Used for typing and editing text.
  - Press `Esc` to return to Normal Mode.

### Commands
- **Save File**: `:w`
- **Quit**: `:q`

### Example
1. Start editing a file:
   ```bash
   ./text_editor example.txt
   ```
2. Make changes in Insert Mode (`i`), then press `Esc` to return to Normal Mode.
3. Save changes with `:w` and exit with `:q`.

---

## Key Bindings

| Key    | Action                          |
|--------|---------------------------------|
| `i`    | Switch to Insert Mode          |
| `Esc`  | Switch to Normal Mode          |
| `hjkl` | Move left, down, up, right     |
| `u`    | Undo the last action           |
| `U`    | Redo the last undone action    |
| `d`    | Delete the current line        |
| `:`    | Command prompt (save, quit)    |

---

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Commit your changes (`git commit -m 'Add new feature'`).
4. Push to the branch (`git push origin feature-branch`).
5. Open a pull request.

---

## Contact

For issues or suggestions, please open an issue in the repository or reach out via email. [aftabnaik1419@gmail.com].
