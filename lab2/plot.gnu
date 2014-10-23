set xlabel "x"
set ylabel "y"

#set palette model RGB defined (0 "blue", 1 "red", 2 "green")
#unset colorbox

#plot "a.dat" using 1:2 with points title 'podpunkt a' 
#plot "b.dat" using 1:2 with points title 'podpunkt b' 
#plot "c.dat" using 1:2 with points title 'podpunkt c' 
plot "d.dat" using 1:2 with points title 'podpunkt d' 
pause -1

