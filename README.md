# Clipboard Tool for Windows

This project provides a set of command-line utilities for interacting with the clipboard on Windows systems. Windows does not offer a comprehensive tool for reading the clipboard, with the default `clip.exe` utility only allowing you to copy content **to** the clipboard, but **not** read it. This set of tools includes the following:

- `win-clip.exe`: A full-featured tool that can both read from and write to the clipboard.
- `get-clip.exe`: A specialized tool for reading from the clipboard and outputting the contents to a file or stdout.
- `set-clip.exe`: A specialized tool for writing contents from a file or stdin to the clipboard.

Assumes the input/output text is UTF-8 encoded.

## Installation

There are two ways to install this tool:

1. **Download the provided binaries**:

   - You can simply copy the precompiled binaries into any directory which is in your system's `PATH`.

2. **Build the project**:

   - Run the `build.bat` script to compile the program. This will generate the necessary binaries.
   - Copy the binaries into any directory which is in your system's `PATH`.

Once installed, the tools should be accessible from the command line.

## Usage

### win-clip.exe

`win-clip.exe` is the base program that can both read from and write to the clipboard. It supports the following options:

#### Examples:

- To copy the contents of a file to the clipboard:

  ```batch
  win-clip -i myfile.txt
  ```

- To read the contents of stdin pass a "-" as filename

  ```batch
  echo hi | win-clip -i -
  ```

- To read the clipboard and save the content to a file:

  ```batch
  win-clip -o output.txt
  ```

- To write the contents of the clipboard to stdout pass a "-" as the filename
  ```batch
  win-clip -o -
  ```

### get-clip.exe

`get-clip.exe` reads the clipboard and writes the content to stdout or a specified file.

#### Examples:

- To read from the clipboard and print to stdout:

  ```batch
  get-clip
  ```

- To read from the clipboard and save to a file:
  ```batch
  get-clip output.txt
  ```

### set-clip.exe

`set-clip.exe` writes content from a file or stdin to the clipboard.

#### Examples:

- To copy the contents of a file to the clipboard:

  ```batch
  set-clip myfile.txt
  ```

- To copy content from stdin to the clipboard:

  ```batch
  echo "Hello, Clipboard!" | set-clip
  ```
