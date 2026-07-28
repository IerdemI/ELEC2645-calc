# ELEC2645 Calculator

This project is a command-line calculator written in C for the ELEC2645 Embedded Systems Project.

The calculator currently incudes:

- Mode A - Series RLC Calculator
- Mode B - parallel RLC calculator
- Mode C - N/A
- Mode D - n\/\A

The program is split between `main.c` for menu handling and `funcs.c` for the calculator functions.

Pressing b or B through the program will send th euser back to the menus

## Compiling

The calculator was developed on Windows 11 using GCC.

Windows:
`gcc -Wall -Wextra -pedantic main.c funcs.c -o main.exe -lm`
Run: `.\main.exe`

Linux / Codespaces:
`gcc -Wall -wextra -pedantic main.c funcs.c -o main.out -lm`
Run: `./main.out`

macOS:
`clang -Wall -Wextra -pedantic main.c funcs.c -o main.out -lm`
Run: `./main.out`
