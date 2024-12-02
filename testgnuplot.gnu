set terminal png
set output "D:/Microsoft VS Code Projects/onlinealgs-GitWhizKid/testimage.png"

set title "Сигнал и его фильтрация"
set xlabel "Время (с)"
set ylabel "Амплитуда"

# Указываем, что данные разделены запятыми
set datafile separator ","

# Построение графиков
plot "D:/Microsoft VS Code Projects/onlinealgs-GitWhizKid/build/Debug/data.txt" using 1:2 with lines title "Noisy Signal", \
     "D:/Microsoft VS Code Projects/onlinealgs-GitWhizKid/build/Debug/data.txt" using 1:3 with lines title "Filtered Signal", \

set output