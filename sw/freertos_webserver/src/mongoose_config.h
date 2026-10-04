#pragma once

#define MG_ARCH MG_ARCH_ARMGCC
#define MG_ENABLE_FREERTOS 1  // Speicher und mg_millis() über FreeRTOS
#define MG_ENABLE_LWIP 1      // Sockets aus dem lwIP des BSP
#define MG_ENABLE_TCPIP 0     // eingebauter Stack bleibt aus, lwIP übernimmt
