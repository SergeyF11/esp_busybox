#ifndef BUSYBOX_LFS_H
#define BUSYBOX_LFS_H

#include <LittleFS.h>

#pragma message("++++++++++++++++++++++ Using LitleFS file system ++++++++++++++++++")

namespace Busybox {

    // Инициализация файловой системы
    bool begin(bool formatOnFail = false) {
        return LittleFS.begin(formatOnFail);
    }

    // Форматирование файловой системы
    bool format() {
        return LittleFS.format();
    }

      // Классический ls с полными путями
    void ls(const char* path = "/") {
        File root = LittleFS.open(path);
        if (!root) {
            Serial.printf("ls: cannot access '%s'\n", path);
            return;
        }
        if (!root.isDirectory()) {
            Serial.println("Not a directory");
            root.close();
            return;
        }

        File file = root.openNextFile();
        while (file) {

            #if defined(ARDUINO_ARCH_ESP32)
            String fullPath('/');
            fullPath += file.name();
            #else
            String fullPath = file.name(); // имя файла
            #endif

            if (file.isDirectory()) {
                fullPath += '/';
                Serial.printf("%-32s [Dir]\n", fullPath.c_str());
            } else {
                Serial.printf("%-25s %6d bytes\n", fullPath.c_str(), file.size());
            }
            file.close();
            file = root.openNextFile();
        }
        root.close();
    }


// Древовидный вывод
    void tree(const char* path = "/", uint8_t levels = 0, uint8_t indent = 0) {
        String indentStr = "";
        for (int i = 0; i < indent; i++) {
            indentStr += "  ";
        }
        
        Serial.printf("%sListing directory: %s\n", indentStr.c_str(), path);

        File root = LittleFS.open(path);
        if (!root) {
            Serial.printf("%sFailed to open directory\n", indentStr.c_str());
            return;
        }
        if (!root.isDirectory()) {
            Serial.printf("%sNot a directory\n", indentStr.c_str());
            root.close();
            return;
        }

        File file = root.openNextFile();
        bool foundAny = false;
        
        while (file) {
            String name = file.name();
            int lastSlash = name.lastIndexOf('/');
            if (lastSlash != -1) {
                name = name.substring(lastSlash + 1);
            }
            
            if (file.isDirectory()) {
                Serial.printf("%s├── DIR : %s/\n", indentStr.c_str(), name.c_str());
                foundAny = true;
                if (levels > 0) {
                    String fullPath = String(path);
                    if (!fullPath.endsWith("/")) fullPath += "/";
                    fullPath += name;
                    tree(fullPath.c_str(), levels - 1, indent + 1);
                }
            } else {
                Serial.printf("%s├── FILE: %s", indentStr.c_str(), name.c_str());
                int nameLen = name.length();
                if (nameLen < 20) {
                    for (int i = nameLen; i < 20; i++) Serial.print(" ");
                }
                Serial.printf("  SIZE: %d\n", file.size());
                foundAny = true;
            }
            
            file = root.openNextFile();
        }
        
        Serial.printf("%s%s\n", indentStr.c_str(), foundAny ? "└── End" : "└── (empty)");
        root.close();
    }

    // // Список файлов и директорий с улучшенным форматированием
    // void tree(const char* path = "/", uint8_t levels = 0, uint8_t indent = 0) {
    //     String indentStr = "";
    //     for (int i = 0; i < indent; i++) {
    //         indentStr += "  ";
    //     }
        
    //     Serial.printf("%sListing directory: %s\n", indentStr.c_str(), path);

    //     File root = LittleFS.open(path);
    //     if (!root) {
    //         Serial.printf("%sFailed to open directory\n", indentStr.c_str());
    //         return;
    //     }
    //     if (!root.isDirectory()) {
    //         Serial.printf("%sNot a directory\n", indentStr.c_str());
    //         root.close();
    //         return;
    //     }

    //     File file = root.openNextFile();
    //     bool foundAny = false;
        
    //     while (file) {
    //         String name = file.name();
            
    //         // Извлекаем только последнюю часть пути
    //         int lastSlash = name.lastIndexOf('/');
    //         if (lastSlash != -1) {
    //             name = name.substring(lastSlash + 1);
    //         }
            
    //         if (file.isDirectory()) {
    //             Serial.printf("%s├── DIR : %s/\n", indentStr.c_str(), name.c_str());
    //             foundAny = true;
    //             if (levels > 0) {
    //                 // Формируем полный путь для рекурсии
    //                 String fullPath = String(path);
    //                 if (!fullPath.endsWith("/")) fullPath += "/";
    //                 fullPath += name;
    //                 tree(fullPath.c_str(), levels - 1, indent + 1);
    //             }
    //         } else {
    //             Serial.printf("%s├── FILE: %s", indentStr.c_str(), name.c_str());
    //             // Добавляем выравнивание для размера
    //             int nameLen = name.length();
    //             if (nameLen < 20) {
    //                 for (int i = nameLen; i < 20; i++) Serial.print(" ");
    //             }
    //             Serial.printf("  SIZE: %d\n", file.size());
    //             foundAny = true;
    //         }
            
    //         file = root.openNextFile();
    //     }
        
    //     if (!foundAny) {
    //         Serial.printf("%s└── (empty)\n", indentStr.c_str());
    //     } else {
    //         Serial.printf("%s└── End of %s\n\n", indentStr.c_str(), path);
    //     }
        
    //     root.close();
    // }

    //Удаление файла
    bool rm(const char* path) {
        if (LittleFS.remove(path)) {
            Serial.printf("File '%s' removed successfully\n", path);
            return true;
        } else {
            Serial.printf("Failed to remove file '%s'\n", path);
            return false;
        }
    }

    uint8_t rm(std::initializer_list<const char*> listPath ){
        uint8_t count = 0;
        for ( auto path : listPath){
            if ( rm(path)) count++;
        }
        return count;
    }
    
    /// @brief Удаление файлов (поддерживает неограниченное число аргументов). список нужно закончить nullptr !!
    /// @param firstPath 
    /// @param secondPath 
    /// @param  ...
    /// @return deleted files
    uint8_t rm(const char* firstPath, const char* secondPath, ...) {
        va_list args;
        const char* path = firstPath;
        uint8_t deleted = 0;
        uint8_t total = 1;

        bool allSuccess = rm(firstPath);
        deleted += allSuccess;

        path = secondPath;
        va_start(args, secondPath);
        
        while (path != nullptr) {
            total++;
            if ( ! rm(path) ) {
                allSuccess = false;
            } else {
                deleted++;
            }
            path = va_arg(args, const char*);
        }
        
        va_end(args);
        if ( deleted > 1 )
            Serial.printf("Files deleted: %d from %d\n", deleted, total);
        return deleted;
    }


    bool rmrf(const char* path);
    // Удаление директории (рекурсивное с флагом force)
    bool rmdir(const char* path, bool force = false) {
        if (!force) {
            // Простое удаление пустой директории
            if (LittleFS.rmdir(path)) {
                Serial.printf("Directory '%s' removed successfully\n", path);
                return true;
            } else {
                Serial.printf("Failed to remove directory '%s' (may be not empty)\n", path);
                return false;
            }
        } else {
            // Рекурсивное удаление с содержимым
            return rmrf(path);
        }
    }


    // Рекурсивное удаление директории с содержимым (аналог rm -rf)
    bool rmrf(const char* path) {
        File root = LittleFS.open(path);
        if (!root) {
            Serial.printf("Cannot open '%s'\n", path);
            return false;
        }
        
        if (!root.isDirectory()) {
            root.close();
            return rm(path);
        }

        bool success = true;
        File file = root.openNextFile();
        
        while (file) {
            const char* itemName = file.name();
            char fullPath[256];
            
#if defined(ARDUINO_ARCH_ESP32)
            // ESP32: file.name() возвращает только имя, формируем полный путь
            if (strcmp(path, "/") == 0) {
                snprintf(fullPath, sizeof(fullPath), "/%s", itemName);
            } else {
                snprintf(fullPath, sizeof(fullPath), "%s/%s", path, itemName);
            }
#else
            // ESP8266: file.name() возвращает полный путь
            strncpy(fullPath, itemName, sizeof(fullPath) - 1);
            fullPath[sizeof(fullPath) - 1] = '\0';
#endif

            // Закрываем файл перед удалением
            file.close();

            if (LittleFS.exists(fullPath)) {
                File checkFile = LittleFS.open(fullPath);
                if (checkFile) {
                    if (checkFile.isDirectory()) {
                        checkFile.close();
                        if (!rmrf(fullPath)) {
                            success = false;
                        }
                    } else {
                        checkFile.close();
                        if (!rm(fullPath)) {
                            success = false;
                        }
                    }
                }
            }
            file = root.openNextFile();
        }
        root.close();

        if (success && LittleFS.rmdir(path)) {
            Serial.printf("Directory '%s' removed recursively\n", path);
            return true;
        } else {
            Serial.printf("Failed to remove directory '%s' recursively\n", path);
            return false;
        }
    }

    // Вывод содержимого файла
    bool cat(const char* path) {
        File file = LittleFS.open(path, "r");
        if (!file) {
            Serial.printf("cat: cannot open '%s'\n", path);
            return false;
        }

        Serial.printf("--- %s ---\n", path);
        while (file.available()) {
            char buf[64];
            auto len = file.readBytes(buf, 64);

            Serial.write(buf, len); //file.read());
            delay(0);
        }
        Serial.println();
        file.close();
        return true;
    }

    // Вывод содержимого файла в hex-формате
    bool dump(const char* path, uint8_t bytesPerLine = 16) {
        File file = LittleFS.open(path, "r");
        if (!file) {
            Serial.printf("dump: cannot open '%s'\n", path);
            return false;
        }

        Serial.printf("Hex dump of '%s' (%d bytes):\n", path, file.size());
        
        size_t offset = 0;
        while (file.available()) {
            Serial.printf("%08X: ", offset);
            
            for (uint8_t i = 0; i < bytesPerLine; i++) {
                if (file.available()) {
                    uint8_t b = file.read();
                    Serial.printf("%02X ", b);
                    offset++;
                } else {
                    Serial.print("   ");
                }
                if (i == 7) Serial.print(" ");
            }
            Serial.println();
            delay(0);
        }
        
        file.close();
        return true;
    }

    // // Просмотр файла с правильной обработкой переноса кириллицы
    void view(const char* path, uint16_t bytesPerLine = 16) {
        File file = LittleFS.open(path, "r");
        if (!file) {
            Serial.printf("view: cannot open '%s'\n", path);
            return;
        }

        Serial.printf("=== View: %s (%d bytes) ===\n", path, file.size());
        Serial.println("Offset    Hex dump                                            ASCII/Cyrillic");
        Serial.println("--------  -------------------------------------------------  ----------------");

        size_t globalOffset = 0;
        uint8_t carryOver = 0;
        bool hasCarryOver = false;
        
        while (file.available() || hasCarryOver) {
            yield();
            // Читаем данные для текущей строки (только новые байты)
            uint8_t buffer[16] = {0};
            uint8_t bytesInBuffer = file.read(buffer, bytesPerLine);
            
            if (bytesInBuffer == 0 && !hasCarryOver) break;
            
            // Выводим offset текущей строки
            Serial.printf("%08X  ", globalOffset);
            
            // Вывод hex дампа (только новых байтов, без перенесенного)
            for (uint8_t i = 0; i < 16; i++) {
                if (i < bytesInBuffer) {
                    Serial.printf("%02X ", buffer[i]);
                } else {
                    Serial.print("   ");
                }
                if (i == 7) Serial.print(" ");
            }
            
            Serial.print(" |");
            
            // Обработка символов для ASCII колонки
            uint8_t asciiPos = 0;
            uint8_t bufferIndex = 0;

            int asciPosCorrector = hasCarryOver ? 1 :0;
            // Обрабатываем перенесенный байт, если есть
            if (hasCarryOver) {
                
                if (bytesInBuffer > 0) {
                    // Есть следующий байт - проверяем на кириллицу
                    uint8_t c2 = buffer[0];
                    if ((carryOver == 0xD0 && c2 >= 0x90 && c2 <= 0xBF) ||
                        (carryOver == 0xD1 && c2 >= 0x80 && c2 <= 0x8F) ||
                        (carryOver == 0xD1 && c2 >= 0x91 && c2 <= 0xBF) ||
                        (carryOver == 0xD0 && c2 == 0x81) ||
                        (carryOver == 0xD1 && c2 == 0x91)) {
                        // Кириллица - выводим БЕЗ пробела
                        Serial.write(carryOver);
                        Serial.write(c2);
                        asciiPos += 1;
                        bufferIndex = 1; // Пропускаем первый байт буфера
                    } else {
                        // Не кириллица - выводим как обычный символ
                        if (carryOver >= 32 && carryOver <= 126) {
                            Serial.write(carryOver);
                        } else {
                            Serial.print(".");
                        }
                        asciiPos++;
                    }
                } else {
                    // Нет следующих байтов - выводим как обычный символ
                    if (carryOver >= 32 && carryOver <= 126) {
                        Serial.write(carryOver);
                    } else {
                        Serial.print(".");
                    }
                    asciiPos++;
                }
                hasCarryOver = false;
                carryOver = 0;
            }
            
            // Обрабатываем оставшиеся байты буфера
            while (bufferIndex < bytesInBuffer && asciiPos < 16 /* + asciPosCorrector */) {
                uint8_t c = buffer[bufferIndex];
                bool isCyrillicStart = (c == 0xD0 || c == 0xD1);
                bool hasNextByte = (bufferIndex + 1) < bytesInBuffer;
                
                if (isCyrillicStart && hasNextByte) {
                    uint8_t c2 = buffer[bufferIndex + 1];
                    // Проверяем, является ли это валидной кириллицей
                    if ((c == 0xD0 && c2 >= 0x90 && c2 <= 0xBF) ||
                        (c == 0xD1 && c2 >= 0x80 && c2 <= 0x8F) ||
                        (c == 0xD1 && c2 >= 0x91 && c2 <= 0xBF) ||
                        (c == 0xD0 && c2 == 0x81) ||
                        (c == 0xD1 && c2 == 0x91)) {
                        
                        // Кириллица - занимает 2 позиции
                        if (asciiPos + 2 <= 16 +asciPosCorrector ) {
                            Serial.print(" "); // Пробел для новой кириллицы в строке
                            Serial.write(c);
                            Serial.write(c2);
                            asciiPos += 2;
                            bufferIndex += 2; // Пропускаем два байта
                        } else {
                            // Не помещается в текущую строку - сохраняем первый байт
                            carryOver = c;
                            hasCarryOver = true;
                            bufferIndex++;
                        }
                    } else {
                        // Не валидная кириллица - обрабатываем как обычный символ
                        if (c >= 32 && c <= 126) {
                            Serial.write(c);
                        } else {
                            Serial.print(".");
                        }
                        asciiPos++;
                        bufferIndex++;
                    }
                } else if (isCyrillicStart && !hasNextByte) {
                    // Начало кириллицы в конце строки - сохраняем
                    carryOver = c;
                    hasCarryOver = true;
                    bufferIndex++;

                } else {
                    // Обычный символ
                    if (c >= 32 && c <= 126) {
                        Serial.write(c);
                    } else {
                        Serial.print(".");
                    }
                    asciiPos++;
                    bufferIndex++;
                }
            }
            

            // Выравнивание ASCII колонки
            for (uint8_t j = asciiPos; j < 16; j++) {
                Serial.print(" ");
            }
            Serial.println("|");
            
            // Увеличиваем глобальную позицию
            globalOffset += 16;
            //Serial.flush();
        }
        
        file.close();
        Serial.println("--------  -------------------------------------------------  ----------------");
    }

    // Просмотр файла с правильной обработкой переноса кириллицы
    void view1(const char* path, uint16_t bytesPerLine = 16) {
        File file = LittleFS.open(path, "r");
        if (!file) {
            Serial.printf("view: cannot open '%s'\n", path);
            return;
        }

        Serial.printf("=== View: %s (%d bytes) ===\n", path, file.size());
        Serial.println("Offset    Hex dump                          ASCII/Cyrillic");
        Serial.println("--------  --------------------------------  ----------------");

        size_t globalOffset = 0;
        uint8_t carryOver = 0;
        bool hasCarryOver = false;
        
        while (file.available() || hasCarryOver) {
            // Читаем данные для текущей строки (только новые байты)
            uint8_t buffer[16] = {0};
            uint8_t bytesInBuffer = file.read(buffer, bytesPerLine);
            
            if (bytesInBuffer == 0 && !hasCarryOver) break;
            
            // Выводим offset текущей строки
            Serial.printf("%08X  ", globalOffset);
            
            // Вывод hex дампа (только новых байтов, без перенесенного)
            for (uint8_t i = 0; i < 16; i++) {
                if (i < bytesInBuffer) {
                    Serial.printf("%02X ", buffer[i]);
                } else {
                    Serial.print("   ");
                }
                if (i == 7) Serial.print(" ");
            }
            
            Serial.print(" |");
            
            // Обработка символов для ASCII колонки
            uint8_t asciiPos = 0;
            uint8_t bufferIndex = 0;
            
            // Обрабатываем перенесенный байт, если есть
            if (hasCarryOver) {
                if (bytesInBuffer > 0) {
                    // Есть следующий байт - проверяем на кириллицу
                    uint8_t c2 = buffer[0];
                    if ((carryOver == 0xD0 && c2 >= 0x90 && c2 <= 0xBF) ||
                        (carryOver == 0xD1 && c2 >= 0x80 && c2 <= 0x8F) ||
                        (carryOver == 0xD1 && c2 >= 0x91 && c2 <= 0xBF) ||
                        (carryOver == 0xD0 && c2 == 0x81) ||
                        (carryOver == 0xD1 && c2 == 0x91)) {
                        // Кириллица - выводим БЕЗ пробела
                        Serial.write(carryOver);
                        Serial.write(c2);
                        asciiPos += 2;
                        bufferIndex = 1; // Пропускаем первый байт буфера
                    } else {
                        // Не кириллица - выводим как обычный символ
                        if (carryOver >= 32 && carryOver <= 126) {
                            Serial.write(carryOver);
                        } else {
                            Serial.print(".");
                        }
                        asciiPos++;
                    }
                } else {
                    // Нет следующих байтов - выводим как обычный символ
                    if (carryOver >= 32 && carryOver <= 126) {
                        Serial.write(carryOver);
                    } else {
                        Serial.print(".");
                    }
                    asciiPos++;
                }
                hasCarryOver = false;
                carryOver = 0;
            }
            
            // Обрабатываем оставшиеся байты буфера
            while (bufferIndex < bytesInBuffer && asciiPos < 16) {
                uint8_t c = buffer[bufferIndex];
                bool isCyrillicStart = (c == 0xD0 || c == 0xD1);
                bool hasNextByte = (bufferIndex + 1) < bytesInBuffer;
                
                if (isCyrillicStart && hasNextByte) {
                    uint8_t c2 = buffer[bufferIndex + 1];
                    // Проверяем, является ли это валидной кириллицей
                    if ((c == 0xD0 && c2 >= 0x90 && c2 <= 0xBF) ||
                        (c == 0xD1 && c2 >= 0x80 && c2 <= 0x8F) ||
                        (c == 0xD1 && c2 >= 0x91 && c2 <= 0xBF) ||
                        (c == 0xD0 && c2 == 0x81) ||
                        (c == 0xD1 && c2 == 0x91)) {
                        
                        // Кириллица - занимает 2 позиции
                        if (asciiPos + 2 <= 16) {
                            Serial.print(" "); // Пробел для новой кириллицы в строке
                            Serial.write(c);
                            Serial.write(c2);
                            asciiPos += 2;
                            bufferIndex += 2; // Пропускаем два байта
                        } else {
                            // Не помещается в текущую строку - сохраняем первый байт
                            carryOver = c;
                            hasCarryOver = true;
                            bufferIndex++;
                        }
                    } else {
                        // Не валидная кириллица - обрабатываем как обычный символ
                        if (c >= 32 && c <= 126) {
                            Serial.write(c);
                        } else {
                            Serial.print(".");
                        }
                        asciiPos++;
                        bufferIndex++;
                    }
                } else if (isCyrillicStart && !hasNextByte) {
                    // Начало кириллицы в конце строки - сохраняем
                    carryOver = c;
                    hasCarryOver = true;
                    bufferIndex++;
                } else {
                    // Обычный символ
                    if (c >= 32 && c <= 126) {
                        Serial.write(c);
                    } else {
                        Serial.print(".");
                    }
                    asciiPos++;
                    bufferIndex++;
                }
            }
            
            // Выравнивание ASCII колонки
            for (uint8_t j = asciiPos; j < 16; j++) {
                Serial.print(" ");
            }
            Serial.println("|");
            
            // Увеличиваем глобальную позицию
            globalOffset += 16;
        }
        
        file.close();
        Serial.println("--------  --------------------------------  ----------------");
    }

    // Переименование/перемещение файла
    bool mv(const char* oldPath, const char* newPath) {
        if (LittleFS.rename(oldPath, newPath)) {
            Serial.printf("'%s' moved to '%s'\n", oldPath, newPath);
            return true;
        } else {
            Serial.printf("Failed to move file '%s' to '%s'\n", oldPath, newPath);
            return false;
        }
    }

    // Копирование файла
    bool cp(const char* sourcePath, const char* destPath) {
        File source = LittleFS.open(sourcePath, "r");
        if (!source) {
            Serial.printf("cp: cannot open source '%s'\n", sourcePath);
            return false;
        }

        File dest = LittleFS.open(destPath, "w");
        if (!dest) {
            Serial.printf("cp: cannot create '%s'\n", destPath);
            source.close();
            return false;
        }

        size_t bytesCopied = 0;
        uint8_t buffer[128];
        while (source.available()) {
            size_t bytesRead = source.read(buffer, sizeof(buffer));
            dest.write(buffer, bytesRead);
            bytesCopied += bytesRead;
        }

        source.close();
        dest.close();

        Serial.printf("cp: '%s' -> '%s' (%d bytes)\n", sourcePath, destPath, bytesCopied);
        return true;
    }

    // Создание директории
    bool mkdir(const char* path) {
        if (LittleFS.mkdir(path)) {
            Serial.printf("Directory '%s' created successfully\n", path);
            return true;
        } else {
            Serial.printf("Failed to create directory '%s'\n", path);
            return false;
        }
    }

    // Запись текста в файл
    bool write(const char* path, const char* content) {
        File file = LittleFS.open(path, "w");
        if (!file) {
            Serial.printf("write: cannot create '%s'\n", path);
            return false;
        }

        size_t bytesWritten = file.print(content);
        file.close();

        bool success = (bytesWritten == strlen(content));
        Serial.printf("write: %d bytes to '%s' %s\n", bytesWritten, path, success ? "OK" : "FAILED");
        return success;
    }

    // Добавление текста в конец файла
    bool append(const char* path, const char* content) {
        File file = LittleFS.open(path, "a");
        if (!file) {
            Serial.printf("append: cannot open '%s'\n", path);
            return false;
        }

        size_t bytesWritten = file.print(content);
        file.close();

        bool success = (bytesWritten == strlen(content));
        Serial.printf("append: %d bytes to '%s' %s\n", bytesWritten, path, success ? "OK" : "FAILED");
        return success;
    }

    // Получение информации о файле
    bool stat(const char* path) {
        if (LittleFS.exists(path)) {
            File file = LittleFS.open(path, "r");
            if (file) {
                Serial.printf( "%s: %s\n", file.isDirectory() ? "Dir" : "File", path );
                if (!file.isDirectory()) Serial.printf("Size: %d bytes\n", file.size());
                file.close();
                return true;
            }
        }
        Serial.printf("'%s' not found\n", path);
        return false;
    }

    // Получение свободного места
    void df() {
#if defined(ARDUINO_ARCH_ESP8266)
        FSInfo fs_info;
        if (!LittleFS.info(fs_info)) {
            Serial.println("df: failed to get filesystem info");
            return;
        }
        size_t totalBytes = fs_info.totalBytes;
        size_t usedBytes = fs_info.usedBytes;
#else
        size_t totalBytes = LittleFS.totalBytes();
        size_t usedBytes = LittleFS.usedBytes();
#endif

        Serial.println("Filesystem info:");
        Serial.printf("Total: %d bytes\n", totalBytes);
        Serial.printf("Used:  %d bytes\n", usedBytes);
        Serial.printf("Free:  %d bytes\n", totalBytes - usedBytes);
    }

} // namespace BusyboxLFS

#endif