Простая библиотека для отладки скетчей на esp32

Поддерживает работу в LittleFS, SPIFFS, FATFS (тестировано только LittleFS)

Обеспечивает вывод в Serial информации:
  - Busybox::sysinfo()
  - Busybox::df()
  - Busybox::stat(FILE)
  - Busybox::stat(DIR)
  - Busybox::ls(PATH="/")
   -Busybox::tree(PATH="/", DEPTH=0, INDENT=0)

Вывод содерижмого файла:
  - Busybox::cat(FILE)
  - Busybox::dump(FILE)
  - Busybox::view(FILE) - аналог view NC (dump + text)

Операции с файлами:
  - Busybox::cp(SRC, DEST)
  - Busybox::mv(SRC,DEST)
  - Busybox::rm(FILE, .....)
  - Busybox::write(FILE, TEXT)
  - Busybox::append(FILE, TEXT)

Операции с директориями:
  - Busybox::mkdir(DIR)
  - Busybox::rmdir(DIR, FORCE=false)
  - Busybox::rmrf(DIR)
  
