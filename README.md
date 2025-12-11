Простая библиотека для отладки скетчей на esp32

Поддерживает работу в LittleFS, SPIFFS, FATFS (тестировано только LittleFS)

Выбор типа FS осуществляется в основном скетче ПЕРЕД подключением библиотеки с помощью объявления переменной окружения. Если переменная не объявлена используется LittleFS. 
  + #define BUSYBOX_USE_FATFS
   
    или
  + #define BUSYBOX_USE_SPIFFS

Обертки для FS:
  - Busybox::begin(FORMAT=false)
  - Busybox::format()

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
  
