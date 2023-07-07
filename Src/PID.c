#include "PID.h"
#include "main.h"
#include <stdbool.h>

static float pid_error;
static bool pid_reset;
static uint16_t pwm_amount;
static float Kp, Ki, Kd;
static float temp_iState =
		{ 0.0 }, temp_dState =
		{ 0.0 }, pTerm, iTerm, dTerm;

static float target_temperature;
static float current_temperature;
static uint32_t last_display_time = 0;
static uint32_t last_PWM_output_time = 0;

float get_pid_output()
{
	float pid_output;

	pid_error = (float) target_temperature - current_temperature;

	dTerm = K2 * Kd * (current_temperature - temp_dState) + K1 * dTerm;
	temp_dState = current_temperature;

	if (pid_error > PID_FUNCTIONAL_RANGE)
	{
		pid_output = BANG_MAX;
		pid_reset = true;
	} else if (pid_error < -(PID_FUNCTIONAL_RANGE))
	{
		pid_output = 0;
		pid_reset = true;
	} else
	{
		if (pid_reset)
		{
			temp_iState = 0.0;
			pid_reset = false;
		}
		pTerm = Kp * pid_error;
		temp_iState += pid_error;
		iTerm = Ki * temp_iState;

		pid_output = pTerm + iTerm - dTerm;

		if (pid_output > PID_MAX)
		{
			if (pid_error > 0)
				temp_iState -= pid_error; // conditional un-integration
			pid_output = PID_MAX;
		}
		else if (pid_output < 0)
		{
			if (pid_error < 0)
				temp_iState -= pid_error; // conditional un-integration
			pid_output = 0;
		}
	}
	printf("Kp: %.3f Ki: %.3f Kd: %.3f\n", Kp, Ki, Kd);
	printf("pid_error: %.3f pTerm: %.3f iTerm: %.3f dTerm: %.3f pid_output: %.3f\n",
				pid_error, pTerm, iTerm, dTerm, pid_output);
	return pid_output;
}

/**
 * PID Autotuning
 *
 * Alternately heat and cool the oven, observing its behavior to
 * determine the best PID values to achieve a stable temperature.
 * Needs sufficient heater power to make some overshoot at target
 * temperature to succeed.
 */

void PID_autotune(float temp, int ncycles)
{
	float input = 0.0;
	int cycles = 0;
	bool heating = true;

	millis_t temp_ms = millis(), t1 = temp_ms, t2 = temp_ms;
	long t_high = 0, t_low = 0;

	long bias, d;
	float Ku, Tu;
	float workKp = 0, workKi = 0, workKd = 0;
	float max = 0, min = 10000;

	/*Initialize PWM, bias and d to 127 (half of max)*/
	pwm_amount = bias = d = (PID_MAX) >> 1;

	wait_for_heatup = true;
	printf("\nStarting auto tune\n");
	/*PID tuning loop*/
	while (wait_for_heatup)
	{
		millis_t ms = millis();

		/*Get temperature and limit to last max*/
		input = current_temperature;
		NOLESS(max, input);
		NOMORE(min, input);

		/*End the heating cycle when above last last cycle
		 * temperature for 5 seconds.
		 * Set PWM for the cooling cycle = bias - d / 2
		 * Now in cooling cycle*/
		if (heating && input > temp)
		{
			if (ELAPSED(ms, t2 + 5000UL))
			{
				heating = false;
				pwm_amount = (bias - d) >> 1;
				t1 = ms;
				t_high = t1 - t2;
				max = temp;
			}
		}

		if (!heating && input < temp)
		{
			if (ELAPSED(ms, t1 + 5000UL))
			{
				heating = true;
				t2 = ms;
				t_low = t2 - t1;
				if (cycles > 0)
				{
					long max_pow = PID_MAX

					;
					bias += (d * (t_high - t_low)) / (t_low + t_high);
					bias = constrain(bias, 20, max_pow - 20);
					d = (bias > max_pow / 2) ? max_pow - 1 - bias : bias;

					if (cycles > 2)
					{

						Ku = (4.0 * d) / (M_PI * (max - min) * 0.5);
						Tu = ((float) (t_low + t_high) * 0.001);

						workKp = 0.6 * Ku;
						workKi = 2 * workKp / Tu;
						workKd = workKp * Tu * 0.125;
					}
				}

				pwm_amount = (bias + d) >> 1;
				cycles++;
				min = temp;
			}
		}
		if (input > temp + MAX_OVERSHOOT_PID_AUTOTUNE)
		{
			printf(" Temperature too high\n\r");
			return;
		}
		// Over 5 minutes?
//		if (((ms - t1) + (ms - t2)) > (10L * 60L * 1000L * 2L))
//		{
//			printf(" timeout\n\r");
//			return;
//		}
		if (cycles > ncycles)
		{
			printf(
					" finished! Put the last Kp, Ki and Kd constants from below into Configuration.h\n");
			printf("\n Classic PID Kp: %.3f Ki: %.3f Kd: %.3f\n\r",
					workKp, workKi, workKd);
			return;
		}
		/*Set PID every 2 seconds*/
		if ((ms - last_PWM_output_time) > 1000)
		{
			UpdatePWM(pwm_amount);
		}
		/*Update the display every 3 seconds*/
		if ((ms - last_display_time) > 3000)
		{
			printf("input: %.3f\n\r", input);
			printf("pwm_amount: %d\n\r", pwm_amount);
			printf("bias: %ld\n\r", bias);
			printf("d: %ld\n\r", d);
			printf("min: %.3f\n\r", min);
			printf("max: %.3f\n\r", max);
			printf("Ku: %.3f\n\r", Ku);
			printf("Tu: %.3f\n\r", Tu);
			printf("\n Classic PID Kp: %.3f Ki: %.3f Kd: %.3f\n\r",
					workKp, workKi, workKd);
			last_display_time = ms;
		}
	}
}
void set_PID_constants(float p, float i, float d)
{
	Kp = p;
	Ki = i;
	Kd = d;
}

void set_target_temperature(float target)
{
	target_temperature = target;
}

void set_current_temperature(float temperature)
{
	current_temperature = temperature;
}
