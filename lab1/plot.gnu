set xlabel "x"
set ylabel "y"

set palette model RGB defined (0 "blue", 1 "red", 2 "green")
unset colorbox

#plot "a.dat" using 1:2:3 with points title 'podpunkt a' pt 2 palette
#plot "b.dat" using 1:2:3 with points title 'podpunkt b' pt 2 palette
#plot "c.dat" using 1:2:3 with points title 'podpunkt c' pt 2 palette
plot "d.dat" using 1:2:4 with points title 'podpunkt d' pt 2 palette
pause -1

