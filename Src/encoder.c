/*
 * encoder.c
 *
 *  Created on: May 22, 2023
 *      Author: Charles
 */

#include "encoder.h"

int8_t PollEncoder()
{
	uint8_t value = ENCODER_VALUE;
	/*Have to account for last value of zero*/
	if (lastEncoderValue == 0)
	{
		if (value == 0)
		{
			return ENCODER_NO_CHANGE;
		}
		if (value > 18)
		{
			lastEncoderValue = value;
			return ENCODER_DOWN;
		} else
		{
			lastEncoderValue = value;
			return ENCODER_UP;
		}
	} else
	/*last value not zero*/
	{
		if ((ENCODER_VALUE > lastEncoderValue) && !(ENCODER_VALUE % 2)
				&& !(ENCODER_VALUE == 0))
		{
			lastEncoderValue = ENCODER_VALUE;
			return ENCODER_UP;
		}
		if ((ENCODER_VALUE < lastEncoderValue) && !(ENCODER_VALUE % 2)
				&& !(ENCODER_VALUE == 0))
		{
			lastEncoderValue = ENCODER_VALUE;
			return ENCODER_DOWN;
		}
	}
	if (ENCODER_VALUE == 0)
	{
		lastEncoderValue = 0;
	}

	return ENCODER_NO_CHANGE;

}
