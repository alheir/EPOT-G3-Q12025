
#ifndef PWM_GEN_H
#define PWM_GEN_H

void init_mcpwm();

void start_gen();
void stop_gen();
void set_ma(float ma);
float get_ma();
void set_mf(int mf);
int get_mf();

#endif