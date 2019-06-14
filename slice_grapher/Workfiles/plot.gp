# turtle.gp
#   - A gnuplot script
#   - Automatically generated by make_plotscript.sh

set xrange [-1.8:1.8]
set yrange [-0.8:0.8]

# set size 0.75, 1.0
set size 1.0, 1.0

set label "nloop = 0000001000" at -1.5, 0.75

# set terminal aqua

set terminal postscript eps color solid

set output "Workfiles/fig.zxplane.0000001000.eps"

plot	"Workfiles/lines.boundary_box.tt"	w l lt 1 title "",	     	"Workfiles/lines.zxplane_contour_en.tt"	w l lt 3 title "Enstrophy",  	"Workfiles/lines.zxplane_vector_vel.tt"	w l lt 4 title "(Vx,Vy)"