#include <Arduino.h>

// default BUSYBOX_USE_LittleFS 
//#define BUSYBOX_USE_FATFS
//#define BUSYBOX_USE_SPIFFS

#include <LittleFS.h>
#include "Busybox.h"

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\nИнициализация файловой системы");
    if (!Busybox::begin( true)) {
        Serial.println("LittleFS mount failed");
        return;
    }
    Serial.println("LittleFS mounted successfully");

    Serial.println("\nСоздание и запись тестового файла");
    Busybox::write("/test.txt", "Hello, LittleFS!\nThis is a test file.");
    
    Serial.println("\nВывод списка файлов");
    Busybox::ls("/");

    Serial.println("\nВывод дерева директории с вложенностью 3");
    Busybox::tree("/", 3);
    
    Serial.println("\nВывод содержимого файла");
    Busybox::cat("/test.txt");
    
    Serial.println("\nHex dump файла");
    Busybox::dump("/test.txt");
    
    Serial.println("\nКопирование файла");
    Busybox::cp("/test.txt", "/test_copy.txt");

    Serial.println("\nСоздание директории");
    Busybox::mkdir("/dir");    

    Serial.println("\nПереименование файла");
    Busybox::mv("/test_copy.txt", "/dir/backup.txt");
    
    Serial.println("\nИнформация о файловой системе");
    Busybox::df();
    
    Serial.println("\nИнформация о файле");
    Busybox::stat("/test.txt");

    Serial.println("\nО директории");
    Busybox::stat("/dir");

    Serial.println("\nПопытка удаление не пустой директории");
    Busybox::rmdir("/dir");

    Serial.println("\nУдаление файлов");
    Busybox::rm("/test.txt");


    Serial.println("\nПринудительное удаление непустой директории");
    Busybox::rmdir("/dir", true);
}

void loop() {
    // Ваш код здесь
}