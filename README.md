Простая библиотека для отладки скетчей на esp32
Поддерживает работу в LittleFS, SPIFFS, FATFS (тестировано только LittleFS)

Обеспечивает вывод в Serial информации:
  Bysibox::sysinfo()
  Bysibox::df()
  Bysibox::stat(FILE)
  Bysibox::stat(DIR)
  Bysibox::ls(PATH="/")
  Bysibox::tree(PATH="/", DEPTH=0, INDENT=0)

Вывод содерижмого файла:
  Bysibox::cat(FILE)
  Bysibox::dump(FILE)
  Bysibox::view(FILE) - аналог view NC (dump + text)

Операции с файлами:
  Bysibox::cp(SRC, DEST)
  Bysibox::mv(SRC,DEST)
  Bysibox::rm(FILE, .....)
  Bysibox::write(FILE, TEXT)
  Bysibox::append(FILE, TEXT)

Операции с директориями:
  Bysibox::mkdir(DIR)
  Bysibox::rmdir(DIR, FORCE=false)
  Bysibox::rmrf(DIR)
  
