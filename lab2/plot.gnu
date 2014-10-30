set xlabel "x"
set ylabel "y"

#set palette model RGB defined (0 "blue", 1 "red", 2 "green")
#unset colorbox

plot filename using 1:2 with points title 'podpunkt' 
if(filename eq "hull.dat") plot "hull.dat" using 1:2 with linespoints title 'otoczka' 
pause -1


