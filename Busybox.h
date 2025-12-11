#ifndef BUSYBOX_H
#define BUSYBOX_H

// Автоматический выбор реализации
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)

    #if defined(BUSYBOX_USE_FATFS) || defined(_FFAT_H_)
        #include "Busybox_FATFS.h"
    #elif defined(BUSYBOX_USE_SPIFFS) || defined(_SPIFFS_H_)
        #include "Busybox_SPIFFS.h"
    #elif defined(_LITTLEFS_H_)
        #include "Busybox_LFS.h"  // По умолчанию LittleFS
    #else
        #error "Unsupported FS
    #endif
    
#else
    #error "Unsupported platform"
#endif

namespace Busybox {
    // Вывод информации о памяти
    void sysinfo() {
        Serial.println("=== Memory Information ===");
        
#if defined(ARDUINO_ARCH_ESP32)
        // Для ESP32
        Serial.printf("Flash Size:   %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
        Serial.printf("Flash Speed:  %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
        
        Serial.printf("Free Heap:    %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Min Free:     %d bytes\n", ESP.getMinFreeHeap());
        Serial.printf("Max Alloc:    %d bytes\n", ESP.getMaxAllocHeap());
        
        Serial.printf("PSRAM Size:   %d bytes\n", ESP.getPsramSize());
        Serial.printf("Free PSRAM:   %d bytes\n", ESP.getFreePsram());
        Serial.printf("Max PSRAM:    %d bytes\n", ESP.getMaxAllocPsram());
        
        Serial.printf("Sketch Size:  %d bytes\n", ESP.getSketchSize());
        Serial.printf("Free Sketch:  %d bytes\n", ESP.getFreeSketchSpace());

#elif defined(ARDUINO_ARCH_ESP8266)
        // Для ESP8266
        Serial.printf("Flash Size:   %d MB\n", ESP.getFlashChipRealSize() / (1024 * 1024));
        Serial.printf("Flash Speed:  %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
        Serial.printf("Flash Mode:   %s\n", ESP.getFlashChipMode() == FM_QIO ? "QIO" : 
                                          ESP.getFlashChipMode() == FM_QOUT ? "QOUT" :
                                          ESP.getFlashChipMode() == FM_DIO ? "DIO" :
                                          ESP.getFlashChipMode() == FM_DOUT ? "DOUT" : "UNKNOWN");
        
        Serial.printf("Free Heap:    %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Max Alloc:    %d bytes\n", ESP.getMaxFreeBlockSize());
        Serial.printf("Heap Frag:    %d%%\n", ESP.getHeapFragmentation());
        Serial.printf("Free Stack:   %d bytes\n", ESP.getFreeContStack());

        Serial.printf("Sketch Size:  %d bytes\n", ESP.getSketchSize());
        Serial.printf("Free Sketch:  %d bytes\n", ESP.getFreeSketchSpace());
        Serial.printf("Sketch MD5:   %s\n", ESP.getSketchMD5().c_str());

#else
        // Для других платформ
        Serial.printf("Free Heap:    %d bytes\n", ::freeMemory());
#endif

        // Дополнительная системная информация
        Serial.println("=== System Information ===");
#if defined(ARDUINO_ARCH_ESP32)
        Serial.printf("Chip Model:   %s\n", ESP.getChipModel());
        Serial.printf("Chip Cores:   %u\n", ESP.getChipCores());
        Serial.printf("CPU Freq:     %u MHz\n", ESP.getCpuFreqMHz());
        Serial.printf("Cycle Count:  %u\n", ESP.getCycleCount());
        
#elif defined(ARDUINO_ARCH_ESP8266)
        Serial.printf("Chip ID:      0x%08X\n", ESP.getChipId());
        Serial.printf("CPU Freq:     %d MHz\n", ESP.getCpuFreqMHz());
        Serial.printf("SDK Version:  %s\n", ESP.getSdkVersion());
        Serial.printf("Core Version: %s\n", ESP.getCoreVersion().c_str());
        Serial.printf("Boot Version: %d\n", ESP.getBootVersion());
        Serial.printf("Boot Mode:    %d\n", ESP.getBootMode());
        Serial.printf("VCC:          %.2f V\n", ESP.getVcc() / 1024.0);
#endif

        // Uptime
        Serial.println("=== Runtime Information ===");
        unsigned long ms = millis();
        unsigned long sec = ms / 1000;
        unsigned long min = sec / 60;
        unsigned long hr = min / 60;
        Serial.printf("Uptime:       %02lu:%02lu:%02lu\n", hr % 24, min % 60, sec % 60);
        
#if defined(ARDUINO_ARCH_ESP32)
        Serial.printf("Reset Reason: %s\n", esp_reset_reason() == ESP_RST_POWERON ? "Power On" :
                                          esp_reset_reason() == ESP_RST_EXT ? "External" :
                                          esp_reset_reason() == ESP_RST_SW ? "Software" :
                                          esp_reset_reason() == ESP_RST_PANIC ? "Panic" :
                                          esp_reset_reason() == ESP_RST_INT_WDT ? "Interrupt WDT" :
                                          esp_reset_reason() == ESP_RST_TASK_WDT ? "Task WDT" :
                                          esp_reset_reason() == ESP_RST_WDT ? "Other WDT" :
                                          esp_reset_reason() == ESP_RST_DEEPSLEEP ? "Deep Sleep" :
                                          esp_reset_reason() == ESP_RST_BROWNOUT ? "Brownout" :
                                          esp_reset_reason() == ESP_RST_SDIO ? "SDIO" : "Unknown");
#elif defined(ARDUINO_ARCH_ESP8266)
        Serial.printf("Reset Reason: %s\n", ESP.getResetReason().c_str());
#endif
    }
}

#endif