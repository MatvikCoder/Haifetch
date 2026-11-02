#!/bin/sh
echo "Компилируем... / Building..."
g++ -Os -s -flto -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections -Wl,--gc-sections source.cpp -o haifetch
echo "Сжимаем... / Compressing..."
strip haifetch
./upx --best haifetch >> /dev/zero
echo "All done! / Всё готово!"
ls -l haifetch
