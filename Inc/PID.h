/*
 * PID.h
 *
 *  Created on: Mar 25, 2023
 *      Author: Charles
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#include "main.h"
#include <stdbool.h>
#include "MAX6675.h"

typedef uint32_t millis_t;
#define millis()	HAL_GetTick()
#define K1		0.95
#define K2		(1 - K1)
#define PID_FUNCTIONAL_RANGE	20
#define MAX_OVERSHOOT_PID_AUTOTUNE 50
#define PID_MAX		1000
#define BANG_MAX	PID_MAX

// Macros to constrain values
#define NOLESS(v,n) do{ if (v < n) v = n; }while(0)
#define NOMORE(v,n) do{ if (v > n) v = n; }while(0)

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define PENDING(NOW,SOON) ((long)(NOW-(SOON))<0)
#define ELAPSED(NOW,SOON) (!PENDING(NOW,SOON))

extern volatile bool wait_for_heatup;



float get_pid_output();
void set_PID_constants(float p, float i, float d);
void set_target_temperature(float target_temperature);
void set_current_temperature(float temperature);
void PID_autotune(float temp, int ncycles);

#endif /* INC_PID_H_ */
