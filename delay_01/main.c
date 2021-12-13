#include "userdelfx.h"
#include "float_math.h"
#include "buffer_ops.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_LEN 48000 * 10
static __sdram float s_delay_ram[BUFFER_LEN];

static float rate, p, slope;
static uint32_t z, z2, prev, next;
static uint8_t isStop;
static float azar_f;

void DELFX_INIT(uint32_t platform, uint32_t api)
{
  buf_clr_f32(s_delay_ram, BUFFER_LEN);
  z = 0;
  z2 = 0;
  prev = 0;
  next = 0;
  p = 0.f;
  slope = 0.f;
  isStop = 0;
}

void DELFX_PROCESS(float *xn, uint32_t frames)
{

  for (uint32_t i = 0; i < frames * 2; i++)
  {
    s_delay_ram[z++] = xn[i];
    if (z > BUFFER_LEN - 1)
      z = 0;
  }

  if (rate < 0.1f)
  {
    isStop = 0;
    rate = 0.f;
  }
  else
  {

    if (!isStop)
    {
      isStop = 1;
      rate = 0.f;
      p = (z - frames * 2.f) / 2.f;
      if (p < 0.f)
        p = (BUFFER_LEN - 2.f * p) / 2.f;
    }

    for (uint32_t i = 0; i < frames; i++)
    {

      uint32_t length_mono = BUFFER_LEN / 2;

      azar_f = ((float)rand()/160) / (float)(RAND_MAX);

      prev = (uint32_t)p;
      slope = p - prev;
      next = prev + 1;
      if (next > length_mono - 1)
        next = 0;

      float s1L = s_delay_ram[prev * 2];
      float s2L = s_delay_ram[next * 2];
      float s1R = s_delay_ram[prev * 2 + 1];
      float s2R = s_delay_ram[next * 2 + 1];

      float currentL = 0.f, currentR = 0.f;
      currentL = s1L + (s2L - s1L) * slope + azar_f;
      currentR = s1R + (s2R - s1R) * slope;

      xn[i * 2] = currentL;
      xn[i * 2 + 1] = currentR;

      p += 1.f - rate;
      if (p > (float)length_mono - 1.f)
        p = 0.f;
    }
  }
}

void DELFX_PARAM(uint8_t index, int32_t value)
{
  const float valf = q31_to_f32(value);
  switch (index)
  {
  case k_user_delfx_param_time:
    rate = valf;
    break;
  case k_user_delfx_param_depth:
    break;
  default:
    break;
  }
}
