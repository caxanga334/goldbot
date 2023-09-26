#ifndef PLUGIN_GLOBALS_H_
#define PLUGIN_GLOBALS_H_

extern float g_flTickInterval;

#define TICK_INTERVAL (g_flTickInterval)
#define TIME_TO_TICKS(dt) ( static_cast<int>(0.5f + static_cast<float>(dt)/TICK_INTERVAL) )

#endif // !PLUGIN_GLOBALS_H_

