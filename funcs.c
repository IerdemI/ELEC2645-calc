#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "funcs.h"

/*
 * reads a positive floating-point value from the user
 *
 * fgets() is used instead of scanf() so the whole input line is read
 * strtod() converts the string into a double and checks for invalid characters
 * the value is stored using a pointer so the function can return a status
 *
 * returns 1 if the input is valid or 0 if the user enters b to go back
 */
static int get_positive_double(const char *prompt, double *value)
{
    char buffer[128];
    char *end;
    int valid_input;

    do {
        valid_input = 1;

        printf("%s", prompt);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\nInput error. Exiting.\n");
            exit(1);
        }

        /* b or B jumps back to the main menu */
        if ((buffer[0] == 'b' || buffer[0] == 'B') &&
            (buffer[1] == '\n' || buffer[1] == '\r' || buffer[1] == '\0')) {
            return 0;
        }

        *value = strtod(buffer, &end);

        /*
         * skip whitspace left after the number
         * This allow "100   " while still rejecting
         * "100abc "
         */
        while (isspace((unsigned char)*end)) {
            end++;
        }

        /*
         * reject the input if:
         * 1. no number was entered
         * 2. extra non-numeric characters remain
         * 3. the value is zero or negative
         */
        if (end == buffer || *end != '\0' || *value <= 0.0) {
            printf("Invalid input. Please enter a positive number.\n");
            valid_input = 0;
        }

    } while (!valid_input);

    return 1;
}
/*
 * this was void before because mode A didnt need to return anything
 * changed to int so it can return a status depending on if the user
 * finishes the calculation or presses b to leave early
 */

int mode_a_series_rlc(void)
{
    double resistance;
    double inductance;
    double capacitance;
    double frequency;

    double angular_frequency;
    double inductive_reactance;
    double capacitive_reactance;
    double net_reactance;
    double impedance_magnitude;
    double phase_radians;
    double phase_degrees;

    printf("\n=-------- Mode A: Series RLC Calculator --------\n");

    /*
     *component values are entered using SI units keeping one consistent unit system reduces conversion errors during the impedance calculations. */
    if (!get_positive_double("Enter resistance R (ohms): ", &resistance)) {
        return 0;
    }
    if (!get_positive_double("Enter inductance L (henries): ", &inductance)) {
        return 0;
    }
    if (!get_positive_double("Enter capacitance C (farads): ", &capacitance)) {
        return 0;
    }
    if (!get_positive_double("Enter frequency f (Hz): ", &frequency)) {
        return 0;
    }

    /*
     * convert frequency to angular frequency:
     * omega = 2 * pi * f
        */
    angular_frequency = 2.0 * M_PI * frequency;

    /*calculate the reactance of each reactive component:
     * XL = omega * L
     * XC = 1 / (omega * C)
     */
    inductive_reactance = angular_frequency * inductance;
    capacitive_reactance = 1.0 / (angular_frequency * capacitance);

    /*
     * for a series RLC circuit:
     * Z = R + j(XL - XC)
     * keeping the sign of the net reactance allows the program to decide whether the circuit behaves inductively or capacitively.
     */
    net_reactance = inductive_reactance - capacitive_reactance;

    /* Magnitude of the complex impedance:
     * z= sqrt(R^2 + X^2)
     */
    impedance_magnitude =
        sqrt((resistance * resistance) +
             (net_reactance * net_reactance));

    /*
     * atan2() is preferred to atan(X/R) because it handles the signs
     * of both components correctly and gives a reliable phase angle.
     */
    phase_radians = atan2(net_reactance, resistance);
    phase_degrees = phase_radians * (180.0 / M_PI);

    printf("\n--------------- Results ---------------\n");
    printf("Angular frequency:       %.3f rad/s\n", angular_frequency);
    printf("Inductive reactance XL:  %.3f ohms\n", inductive_reactance);
    printf("Capacitive reactance XC: %.3f ohms\n", capacitive_reactance);

    printf("\nComplex impedance:\n");
    printf("Real component:          %.3f ohms\n", resistance);
    printf("Imaginary component:     %+.3f ohms\n", net_reactance);

    printf("\nImpedance magnitude:     %.3f ohms\n", impedance_magnitude);
    printf("Phase angle:             %.3f degrees\n", phase_degrees);

    /*
     * THe sign XL - XC describes the overall reactive behaviour
     * a small tolerance is used rather than testing floating-point calculations for exact equality. */
    if (fabs(net_reactance) < 0.001) {
        printf("Circuit behaviour:       Approximately resonant\n");
    }
    else if (net_reactance > 0.0) {
        printf("Circuit behaviour:       Inductive\n");
    }
    else {
        printf("Circuit behaviour:       Capacitive\n");
    }

    printf("---------------------------------------\n");
    return 1;
}


int menu_item_2(void)
{
    printf("\n>> Mode B: Parallel RLC Calculator\n");
    printf("Mode B has not been implemented yet.\n");
    /* you can call a function from here that handles menu 2 */  
    return 1;
}


int menu_item_3(void)
{
    printf("\n>> Mode C: RC Low-Pass Filter\n");
    printf("Mode C has not been implemented yet.\n");
    /* you can call a function from here that handles menu 3*/  
    return 1;
}


int menu_item_4(void)
{
    printf("\n>> Additional Mode\n");
    printf("Additional mode has not been implemented yet.\n");
    /* you can call a function from here that handles menu 4 */  
    return 1;
}