reset
set title "Time"
set terminal png font "Times_New_Roman"
set xlabel "target char position in string"
set ylabel "time (ns)"
set output "time.png"
set xrange [0:998]
set key left

plot\
"data.txt" using 1:7 with linespoints linewidth 2 title "default-memchr", \
"data.txt" using 1:11 with linespoints linewidth 2 title "default-strchr", \





