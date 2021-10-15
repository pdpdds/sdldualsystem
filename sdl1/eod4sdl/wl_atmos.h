#ifndef _WL_ATMOS_H_
#define _WL_ATMOS_H_

#define MAXPOINTS 500

#ifdef VARIABLEWEATHER
#define WEATHERCHANGEDELAY 20 // 20 seconds
#define WEATHERDELAY 1
#endif

#if defined(USE_STARSKY) || defined(USE_RAIN) || defined(USE_SNOW)
    void Init3DPoints();
#endif

#ifdef USE_STARSKY
    void DrawStarSky(byte *vbuf, uint32_t vbufPitch);
#endif

#ifdef USE_RAIN
    void DrawRain(byte *vbuf, uint32_t vbufPitch);
#endif

#ifdef USE_SNOW
    void DrawSnow(byte *vbuf, uint32_t vbufPitch);
#endif

#ifdef VARIABLEWEATHER
    void InitializeWeatherState(void);
    void PollWeatherChange(void);
#endif

#endif
