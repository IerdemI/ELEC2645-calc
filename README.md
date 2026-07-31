# ELEC2645 Calculator

This project is a command-line calculator written in C for the ELEC2645 Embedded Systems Project.

The calculator currently incudes:

- Mode A - Series RLC Calculator
- Mode B - parallel RLC calculator
- Mode C - RC lowpass filter calculator that plots output voltage across a frequency range and saves the results to CSV whils also providing the values in terminal.
- Mode D - RC component finder game that generates random resistor and capacitor options, then compares them against a target cutoff frequency. If the users input match the closest one to the target cut-off requency programe corrects it. It also has a timer of 20 secnds 

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
