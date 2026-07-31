#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "funcs.h"
#include <time.h>
#include <conio.h>
#include <windows.h>

double resistor_values[] = {
    1000, 1200, 1500, 1800,
    2200, 2700, 3300, 3900,
    4700, 5600, 6800, 8200
};

double capacitor_values[] = {
    10e-9, 12e-9, 15e-9, 18e-9,
    22e-9, 27e-9, 33e-9, 39e-9,
    47e-9, 56e-9, 68e-9, 82e-9
};

/* realistic values for resistors and capacitors */



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


int mode_b_parallel_rlc(void)
{
    double resistance;
    double inductance;
    double capacitance;
    double frequency;

    double angular_frequency;
    double conductance;
    double inductive_susceptance;
    double capacitive_susceptance;
    double total_susceptance;
    double admittance_magnitude;

    double denominator;
    double impedance_real;
    double impedance_imaginary;
    double impedance_magnitude;
    double phase_radians;
    double phase_degrees;

    printf("\n---------- Mode B: Parallel RLC Calculator ----------\n");
    printf("Enter 'b' at any input to return to the main menu.\n\n");

    /* reuse the same validated input function from mode A */
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

    /* convert frequency to angular frequency */
    angular_frequency = 2.0 * M_PI * frequency;

    /*
     * parallel branches are easier to combine using admittance
     * G is conductance and B is susceptance*/
    conductance = 1.0 / resistance;
    inductive_susceptance = -1.0 / (angular_frequency * inductance);
    capacitive_susceptance = angular_frequency * capacitance;

    total_susceptance =
        inductive_susceptance + capacitive_susceptance;

    /* magnitude of the total admittance */
    admittance_magnitude =
        sqrt((conductance * conductance) +
             (total_susceptance * total_susceptance));

    /*
     * take the repcrocal of the complex admittance to get impedance
     * 1 / (G + jB) = (G - jB) / (G^2 + B^2)
     */
    denominator =
        (conductance * conductance) +
        (total_susceptance * total_susceptance);

    impedance_real = conductance / denominator;
    impedance_imaginary = -total_susceptance / denominator;

    /* calculate impedance magnitude and phase */
    impedance_magnitude =
        sqrt((impedance_real * impedance_real) +
             (impedance_imaginary * impedance_imaginary));

    phase_radians = atan2(impedance_imaginary, impedance_real);
    phase_degrees = phase_radians * (180.0 / M_PI);

    printf("\n--------------- Results ---------------\n");
    printf("Angular frequency:          %.3f rad/s\n", angular_frequency);

    printf("\nAdmittance:\n");
    printf("Conductance G:              %.6f S\n", conductance);
    printf("Inductive susceptance BL:   %+.6f S\n", inductive_susceptance);
    printf("Capacitive susceptance BC:  %+.6f S\n", capacitive_susceptance);
    printf("Total susceptance B:        %+.6f S\n", total_susceptance);
    printf("Admittance magnitude:       %.6f S\n", admittance_magnitude);

    printf("\nEquivalent impedance:\n");
    printf("Real component:             %.3f ohms\n", impedance_real);
    printf("Imaginary component:        %+.3f ohms\n", impedance_imaginary);
    printf("Impedance magnitude:        %.3f ohms\n", impedance_magnitude);
    printf("Phase angle:                %.3f degrees\n", phase_degrees);

    /*
     * total susceptance near zero means the inductive and
     * capacitive branches are nearly cancelling each other
     */
    if (fabs(total_susceptance) < 0.000001) {
        printf("Circuit behaviour:          Approximately resonant\n");
    }
    else if (total_susceptance < 0.0) {
        printf("Circuit behaviour:          Inductive\n");
    }
    else {
        printf("Circuit behaviour:          Capacitive\n");
    }

    printf("---------------------------------------\n");

    return 1;
}

/*
 * gets the amount of frequency points for mode C
 * returns 0 if b is entered and 1 when the value is valid
 */
static int get_frequency_points(const char *prompt, int *points)
{
    char buffer[128];
    char *end;
    long value;

    for (;;) {
        printf("%s", prompt);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\nInput error. Exiting.\n");
            exit(1);
        }

        /* b or B goes back to the main menu */
        if ((buffer[0] == 'b' || buffer[0] == 'B') &&
            (buffer[1] == '\n' || buffer[1] == '\r' || buffer[1] == '\0')) {
            return 0;
        }

        value = strtol(buffer, &end, 10);

        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (end == buffer || *end != '\0' || value < 2 || value > 250) {
            printf("Enter a whole number between 2 and 250, or b to go back.\n");
        }
        else {
            *points = (int)value;
            return 1;
        }
    }
}

static int get_choice_with_timeout(const char *prompt,
                                   int min_choice,
                                   int max_choice,
                                   time_t deadline,
                                   int *choice)
{
    char buffer[32];
    int length = 0;
    time_t last_shown_second = -1;

    printf("%s", prompt);
    fflush(stdout);

    for (;;) {
        time_t now = time(NULL);

        if (now >= deadline) {
            printf("\nTime ran out.\n");
            return 0;
        }

        if (now != last_shown_second) {
            long remaining = (long)(deadline - now);

            printf("\rTime left: %2lds   ", remaining);
            fflush(stdout);
            last_shown_second = now;
        }

        if (_kbhit()) {
            int ch = _getch();

            if (ch == '\r' || ch == '\n') {
                char *end;
                long value;

                if (length == 0) {
                    continue;
                }

                buffer[length] = '\0';
                value = strtol(buffer, &end, 10);

                if (end != buffer && *end == '\0' &&
                    value >= min_choice && value <= max_choice) {
                    *choice = (int)value;
                    putchar('\n');
                    printf("\r");
                    return 1;
                }

                printf("\nInvalid input. Enter a number between %d and %d: ",
                       min_choice,
                       max_choice);
                fflush(stdout);
                length = 0;
                continue;
            }

            if (ch == '\b') {
                if (length > 0) {
                    length--;
                    printf("\b \b");
                    fflush(stdout);
                }
                continue;
            }

            if (isdigit((unsigned char)ch) && length < (int)sizeof(buffer) - 1) {
                buffer[length++] = (char)ch;
                putchar(ch);
                fflush(stdout);
            }
        }
        else {
            Sleep(10);
        }
    }
}

int mode_c_low_pass(void)
{
    double input_voltage;
    double resistance;
    double capacitance;
    double start_frequency;
    double stop_frequency;

    double cutoff_frequency;
    double frequency_step;
    double frequency;
    double output_voltage;

    int frequency_points;
    int i;

    FILE *csv_file;

    printf("\n----------- Mode C: RC Low-Pass Filter ----------\n");
    printf("Enter 'b' at any input to return to the main menu.\n\n");

    /* reuse the validated input function from the previous modes */
    if (!get_positive_double("Enter input voltage Vin (V): ", &input_voltage)) {
        return 0;
    }

    if (!get_positive_double("Enter Resistance R (ohms): ", &resistance)) {
        return 0;
    }

    if (!get_positive_double("Enter Capacitance C (farads): ", &capacitance)) {
        return 0;
    }

    if (!get_positive_double("Enter start frequency (Hz): ", &start_frequency)) {
        return 0;
    }

    /*
     * keep asking until the stop frequency is greater than the start
     * frequency so the frequency sweep always moves forwards
     */
    for (;;) {
        if (!get_positive_double("Enter stop frequency (Hz): ", &stop_frequency)) {
            return 0;
        }

        if (stop_frequency > start_frequency) {
            break;
        }

        printf("Stop frequency must be greater than start frequency.\n");
    }

    if (!get_frequency_points(
            "Enter number of frequency points (2-250): ",
            &frequency_points)) {
        return 0;
    }

    /*
     * cutoff frequency is useful for checking where the filter
     * output should start dropping
     */
    cutoff_frequency =
        1.0 / (2.0 * M_PI * resistance * capacitance);

    /*
     * subtract one BEC eause both the start and stop frequencies
     * are included in the selected amount of points
     */
    frequency_step =
        (stop_frequency - start_frequency) /
        (frequency_points - 1);

    csv_file = fopen("mode_c_results.csv", "w");

    if (csv_file == NULL) {
        printf("Could not create mode_c_results.csv\n");
        return 0;
    }

    /* column headings make the CSV easier to understand in Excel */
    fprintf(csv_file, "Frequency_Hz,Output_Voltage_V\n");

    printf("\nCutoff frequency: %.3f Hz\n", cutoff_frequency);
    printf("\nFrequency (Hz)\tOutput Voltage (V)\n");
    printf("-----------------------------------\n");

    for (i = 0; i < frequency_points; i++) {

        frequency = start_frequency + (i * frequency_step);

        /*
         * voltage magnitude of a first order RC low-pass filter:
         * Vout = Vin / sqrt(1 + (2pifRC)^2)
         */
        output_voltage =
            input_voltage /
            sqrt(1.0 +
                 ((2.0 * M_PI * frequency *
                   resistance * capacitance) *
                  (2.0 * M_PI * frequency *
                   resistance * capacitance)));

        /* same result is shown in the terminal and written to the CSV */
        printf("%.3f\t\t%.6f\n", frequency, output_voltage);

        fprintf(csv_file,
                "%.6f,%.9f\n",
                frequency,
                output_voltage);
    }

    fclose(csv_file);

    printf("-----------------------------------\n");
    printf("%d frequency points calculated.\n", frequency_points);
    printf("All results saved to mode_c_results.csv\n");
    printf("The CSV file can now be opened and plotted in Excel.\n");

    return 1;
}


int mode_d_component_finder(void)
{
    double resistor_values[] =
    {
        1000,1200,1500,1800,
        2200,2700,3300,3900,
        4700,5600,6800,8200
    };

    double capacitor_values[] =
    {
        10e-9,12e-9,15e-9,18e-9,
        22e-9,27e-9,33e-9,39e-9,
        47e-9,56e-9,68e-9,82e-9
    };

    double decade_multipliers[] =
    {
        100.0, 10.0, 1.0, 0.1, 0.01
    };

    double capacitor_multipliers[] =
    {
        100.0, 10.0, 1.0
    };

    double available_resistors[4];
    double available_capacitors[4];
    double best_resistance = 0.0;
    double best_capacitance = 0.0;
    double best_cutoff = 0.0;
    double best_error = 1000000.0;

    int used[12] = {0};

    printf("\n---------- Mode D: RC Component Finder ----------\n");

    /* picking 4 unique resistors */

    for(int i=0;i<12;i++)
        used[i]=0;

    for(int i=0;i<4;i++)
    {
        int index;

        do
        {
            index = rand()%12;
        }
        while(used[index]);

        used[index]=1;

        available_resistors[i] =
            resistor_values[index] * decade_multipliers[rand() % 5];
    }

    /* picking 4 unique capacitors */

    for(int i=0;i<12;i++)
        used[i]=0;

    for(int i=0;i<4;i++)
    {
        int index;

        do
        {
            index = rand()%12;
        }
        while(used[index]);

        used[index]=1;

        available_capacitors[i] =
            capacitor_values[index] * capacitor_multipliers[rand() % 3];
    }

    double min_cutoff = 0.0;
    double max_cutoff = 0.0;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            double current_cutoff =
                1.0 / (2.0 * M_PI *
                       available_resistors[i] *
                       available_capacitors[j]);

            if (i == 0 && j == 0)
            {
                min_cutoff = current_cutoff;
                max_cutoff = current_cutoff;
            }
            else
            {
                if (current_cutoff < min_cutoff)
                {
                    min_cutoff = current_cutoff;
                }

                if (current_cutoff > max_cutoff)
                {
                    max_cutoff = current_cutoff;
                }
            }
        }
    }

    double target_frequency =
        min_cutoff + ((double)rand() / (double)RAND_MAX) *
        (max_cutoff - min_cutoff);

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            double current_cutoff =
                1.0/(2*M_PI*
                available_resistors[i]*
                available_capacitors[j]);

            double current_error =
                fabs(target_frequency-current_cutoff)
                /target_frequency*100.0;

            if(current_error<best_error)
            {
                best_error=current_error;
                best_resistance=available_resistors[i];
                best_capacitance=available_capacitors[j];
                best_cutoff=current_cutoff;
            }
        }
    }

    printf("\nTarget cutoff frequency: %.0f Hz\n", target_frequency);

    printf("\nAvailable Resistors\n");

    for(int i=0;i<4;i++)
    {
        printf("%d. %.0f Ohm\n",i+1,available_resistors[i]);
    }

    printf("\nAvailable Capacitors\n");

    for(int i=0;i<4;i++)
    {
        printf("%d. %.0f nF\n",i+1,available_capacitors[i]*1e9);
    }

    int resistor_choice;
    int capacitor_choice;
    int timed_out = 0;

    /* user has 20 seconds to decide which capacitor and resistor to choose, this can be changed to increase the time ( or reduce)*/
    time_t deadline = time(NULL) + 20;

    if (!get_choice_with_timeout("\nChoose a resistor (1-4): ",
                                 1,
                                 4,
                                 deadline,
                                 &resistor_choice)) {
        timed_out = 1;
    }

    if (!timed_out && !get_choice_with_timeout("Choose a capacitor (1-4): ",
                                               1,
                                               4,
                                               deadline,
                                               &capacitor_choice)) {
        timed_out = 1;
    }
    /* comparison with the chosen value and the best possible combination */
    if (!timed_out) {
        double R = available_resistors[resistor_choice-1];
        double C = available_capacitors[capacitor_choice-1];
        double cutoff =
            1.0/(2*M_PI*R*C);
        double error =
            fabs(target_frequency-cutoff)/target_frequency*100.0;

        printf("\nYour cutoff frequency = %.2f Hz\n", cutoff);
        printf("Percentage error = %.2f%%\n", error);
    }

    printf("\nBest possible combination:\n");

    printf("Resistance = %.0f Ohm\n",best_resistance);

    printf("Capacitance = %.0f nF\n",best_capacitance*1e9);

    printf("Cutoff = %.2f Hz\n",best_cutoff);

    printf("Error = %.2f%%\n",best_error);

    return 1;
}