#ifndef BUSYBOX_SPIFFS_H
#define BUSYBOX_SPIFFS_H

#include <FS.h>
#include <SPIFFS.h>

#pragma message("++++++++++++++++++++++ Using SPIFFS file system ++++++++++++++++++")

namespace Busybox {
    
    bool _spiffNotSupported(const char * fn){
        Serial.print("SPIFFS does not support ");
        Serial.println(fn);
        return false;
    }

    bool begin(bool formatOnFail = false) {
        return SPIFFS.begin(formatOnFail);
    }

    bool format() {
        return SPIFFS.format();
    }

    void ls(const char* path = "/") {
        File root = SPIFFS.open(path);
        if (!root) {
            Serial.printf("ls: cannot access '%s'\n", path);
            return;
        }
        if (!root.isDirectory()) {
            root.close();
            Serial.println("Not a directory");
            return;
        }

        File file = root.openNextFile();
        while (file) {
            String fullPath = file.name();
            
            // SPIFFS всегда возвращает полные пути
            if (file.isDirectory()) {
                Serial.printf("%-32s [Dir]\n", fullPath.c_str());
            } else {
                Serial.printf("%-25s %6d bytes\n", fullPath.c_str(), file.size());
            }
            
            file.close();
            file = root.openNextFile();
        }
        root.close();
    }

    bool rm(const char* path) {
        if (SPIFFS.remove(path)) {
            Serial.printf("rm: '%s' removed\n", path);
            return true;
        } else {
            Serial.printf("rm: cannot remove '%s'\n", path);
            return false;
        }
    }

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
            if (!rm(path)) {
                allSuccess = false;
            } else {
                deleted++;
            }
            path = va_arg(args, const char*);
        }
        
        va_end(args);
        if (deleted > 1)
            Serial.printf("rm: %d files deleted from %d\n", deleted, total);
        return deleted;
    }

    bool cat(const char* path) {
        File file = SPIFFS.open(path, "r");
        if (!file) {
            Serial.printf("cat: cannot open '%s'\n", path);
            return false;
        }

        Serial.printf("--- %s ---\n", path);
        while (file.available()) {
            Serial.write(file.read());
        }
        Serial.println();
        file.close();
        return true;
    }

    bool dump(const char* path, uint8_t bytesPerLine = 16) {
        File file = SPIFFS.open(path, "r");
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
        }
        
        file.close();
        return true;
    }

    bool mv(const char* oldPath, const char* newPath) {
        if (SPIFFS.rename(oldPath, newPath)) {
            Serial.printf("mv: '%s' -> '%s'\n", oldPath, newPath);
            return true;
        } else {
            Serial.printf("mv: cannot move '%s' to '%s'\n", oldPath, newPath);
            return false;
        }
    }

    bool cp(const char* sourcePath, const char* destPath) {
        File source = SPIFFS.open(sourcePath, "r");
        if (!source) {
            Serial.printf("cp: cannot open source '%s'\n", sourcePath);
            return false;
        }

        File dest = SPIFFS.open(destPath, "w");
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

    bool mkdir(const char* path) {
        // SPIFFS не поддерживает директории, но оставляем для совместимости
        //Serial.println("mkdir: SPIFFS does not support directories ");
        return  _spiffNotSupported("directories"); //false;
    }

    bool rmdir(const char* path, bool force = false) {
        return _spiffNotSupported("directories");
        // SPIFFS не поддерживает директории
        // Serial.println("rmdir: SPIFFS does not support directories");
        // return false;
    }

    bool write(const char* path, const char* content) {
        File file = SPIFFS.open(path, "w");
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

    bool append(const char* path, const char* content) {
        File file = SPIFFS.open(path, "a");
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

    bool stat(const char* path) {
        if (SPIFFS.exists(path)) {
            File file = SPIFFS.open(path, "r");
            if (file) {
                Serial.printf("File: %s\n", path);
                Serial.printf("Size: %d bytes\n", file.size());
                file.close();
                return true;
            }
        }
        Serial.printf("stat: '%s' not found\n", path);
        return false;
    }

    void df() {
#if defined(ARDUINO_ARCH_ESP8266)
        FSInfo fs_info;
        if (SPIFFS.info(fs_info)) {
            Serial.println("SPIFFS info:");
            Serial.printf("Total: %d bytes\n", fs_info.totalBytes);
            Serial.printf("Used:  %d bytes\n", fs_info.usedBytes);
            Serial.printf("Free:  %d bytes\n", fs_info.totalBytes - fs_info.usedBytes);
        } else {
            Serial.println("df: failed to get SPIFFS info");
        }
#elif defined(ARDUINO_ARCH_ESP32)
        Serial.printf("SPIFFS total: %d, used: %d\n", 
                     SPIFFS.totalBytes(), SPIFFS.usedBytes());
#endif
    }

    void tree(const char* path = "/", uint8_t levels = 0, uint8_t indent = 0) {
        // SPIFFS не поддерживает директории, поэтому tree = ls
        _spiffNotSupported("directory tree");
        ls(path);
    }

} // namespace Busybox

#endif