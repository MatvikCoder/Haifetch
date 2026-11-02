echo "Компилируем... / Building..."
g++ -Os source.cpp -o haifetch
echo "Сжимаем... / Compressing..."
strip haifetch
echo "All done! / Всё готово!"
