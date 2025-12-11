#ifndef BUSYBOX_FATFS_H
#define BUSYBOX_FATFS_H

#include <FS.h>

#if defined(ARDUINO_ARCH_ESP32) 
#include <FFat.h>
#define FATFS FFat
#elif defined defined(ARDUINO_ARCH_ESP8266)
#include <FatFS.h>
#endif

#pragma message("++++++++++++++++++++++ Using FATFS file system ++++++++++++++++++")

namespace Busybox {
    
    bool begin(bool formatOnFail = false) {
        return FATFS.begin(formatOnFail);
    }

    bool format() {
        return FATFS.format();
    }

    void ls(const char* path = "/") {
        File root = FATFS.open(path);
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
            
            // FATFS может возвращать относительные пути
            if (!fullPath.startsWith("/") && strcmp(path, "/") != 0) {
                fullPath = String(path) + "/" + fullPath;
            } else if (!fullPath.startsWith("/")) {
                fullPath = "/" + fullPath;
            }

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
        if (FATFS.remove(path)) {
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
        File file = FATFS.open(path, "r");
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
        File file = FATFS.open(path, "r");
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
        if (FATFS.rename(oldPath, newPath)) {
            Serial.printf("mv: '%s' -> '%s'\n", oldPath, newPath);
            return true;
        } else {
            Serial.printf("mv: cannot move '%s' to '%s'\n", oldPath, newPath);
            return false;
        }
    }

    bool cp(const char* sourcePath, const char* destPath) {
        File source = FATFS.open(sourcePath, "r");
        if (!source) {
            Serial.printf("cp: cannot open source '%s'\n", sourcePath);
            return false;
        }

        File dest = FATFS.open(destPath, "w");
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
        if (FATFS.mkdir(path)) {
            Serial.printf("mkdir: '%s' created\n", path);
            return true;
        } else {
            Serial.printf("mkdir: cannot create '%s'\n", path);
            return false;
        }
    }

    bool rmdir(const char* path, bool force = false) {
        if (!force) {
            if (FATFS.rmdir(path)) {
                Serial.printf("rmdir: '%s' removed\n", path);
                return true;
            } else {
                Serial.printf("rmdir: cannot remove '%s' (may be not empty)\n", path);
                return false;
            }
        } else {
            // Для FATFS нужно реализовать рекурсивное удаление
            Serial.printf("rmdir: recursive remove not implemented for FATFS '%s'\n", path);
            return false;
        }
    }

    bool write(const char* path, const char* content) {
        File file = FATFS.open(path, "w");
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
        File file = FATFS.open(path, "a");
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
        if (FATFS.exists(path)) {
            File file = FATFS.open(path, "r");
            if (file) {
                Serial.printf("%s: %s\n", file.isDirectory() ? "Dir" : "File", path);
                if (!file.isDirectory()) {
                    Serial.printf("Size: %d bytes\n", file.size());
                }
                file.close();
                return true;
            }
        }
        Serial.printf("stat: '%s' not found\n", path);
        return false;
    }

    void df() {
        // FATFS обычно не предоставляет эту информацию через стандартный API
        Serial.println("FATFS: df not available");
    }

    void tree(const char* path = "/", uint8_t levels = 0, uint8_t indent = 0) {
        String indentStr = "";
        for (int i = 0; i < indent; i++) {
            indentStr += "  ";
        }
        
        Serial.printf("%sListing directory: %s\n", indentStr.c_str(), path);

        File root = FATFS.open(path);
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
        
        if (!foundAny) {
            Serial.printf("%s└── (empty)\n", indentStr.c_str());
        } else {
            Serial.printf("%s└── End of %s\n\n", indentStr.c_str(), path);
        }
        
        root.close();
    }

} // namespace Busybox

#endif