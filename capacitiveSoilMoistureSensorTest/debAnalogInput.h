// This function will filter (clip) *valCur if it is outside the allowed range of valid values.
// Returns true if a change has occurred (get value of debVal).
bool debAnalogInput(
    int16_t *valCur,
    int16_t *valPrev,
    uint8_t *state,
    float *sum,
    int16_t *debVal,
    uint8_t *ctr,
    uint32_t *tRef,
    uint16_t valSignifCh,
    uint32_t periodSignifCh,
    int16_t valMax,
    int16_t valMin,
    uint16_t qtySamples)
{
  bool hasChanged{};
  if (*valCur > valMax)
    *valCur = valMax;
  else if (*valCur < valMin)
    *valCur = valMin;
  int16_t minVal = *valPrev - valSignifCh;
  int16_t maxVal = *valPrev + valSignifCh;
  switch (*state)
  {
  case 0: // Waiting for a significant change.
    if (*valCur < minVal || *valCur > maxVal)
    {
      *tRef = millis();
      *state = 1;
    }
    break;
  case 1: // Time significant change.
    if (*valCur >= minVal && *valCur <= maxVal)
    {
      *state = 0; // change resets (false trigger)
    }
    else if (millis() - *tRef > periodSignifCh)
    { // real change
      *sum = 0;
      *ctr = 0;
      *state = 2;
    }
    break;
  case 2: // Sample and get mean (it is assumed that values are stable in this phase).
    if (*ctr < qtySamples)
    {
      *sum += *valCur;
      (*ctr)++;
    }
    else
    {
      *debVal = (int16_t)(*sum / (float)qtySamples);
      hasChanged = true;
      *valPrev = *debVal;
      *state = 0;
    }
    break;
  default:
    break;
  }
  return hasChanged;
}

// Example of usage start =================================================
// #define DBG_IMU_PITCH_DEB
// const uint8_t IMU_DEB_VAL_SIGNIFCHANGE = 1;
// const uint8_t IMU_DEB_PERIOD_SIGNIFCHANGE = 50;
// const int16_t IMU_DEB_VAL_MAX = 200;
// const int16_t IMU_DEB_VAL_MIN = -200;
// const uint16_t QTY_SAMPLES = 10;

// static int16_t imu_pitch_deb_valPrev{};
// static uint8_t imu_pitch_deb_st{};
// static float imu_pitch_deb_sum{};
// static int16_t imu_pitch_deb_val{};
// static uint8_t imu_pitch_deb_ctr{};
// static uint32_t imu_pitch_deb_tRef{};
// if (debAnalogInput(
//         &imu_pitch,
//         &imu_pitch_deb_valPrev,
//         &imu_pitch_deb_st,
//         &imu_pitch_deb_sum,
//         &imu_pitch_deb_val,
//         &imu_pitch_deb_ctr,
//         &imu_pitch_deb_tRef,
//         IMU_DEB_VAL_SIGNIFCHANGE,
//         IMU_DEB_PERIOD_SIGNIFCHANGE,
//         IMU_DEB_VAL_MAX,
//         IMU_DEB_VAL_MIN,
//         QTY_SAMPLES))
// {
// #ifdef DBG_IMU_PITCH_DEB
//   DBG_PORT.print(F("imu_pitch_deb_val: "));
//   DBG_PORT.println(imu_pitch_deb_val);
// #endif
// }
// Example of usage end   =================================================