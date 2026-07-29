#ifndef FUNCS_H
#define FUNCS_H

int mode_a_series_rlc(void);
int mode_b_parallel_rlc(void);/* renamed menu item 2 to match what mode B actually does */
int mode_c_low_pass(void);
int mode_d_component_finder(void);
/* swapped void to int so the functions can return a status when leaving a mode */
#endif