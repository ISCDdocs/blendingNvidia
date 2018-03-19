#Left projector
xrandr --output GPU-1.DP-0 --pos 0x0
xrandr --output GPU-0.DP-1 --pos 0x0

#Right projector
xrandr --output GPU-1.DP-1 --pos 1128x0
xrandr --output GPU-0.DP-0 --pos 1128x0

#Dell control screen
xrandr --output GPU-0.DVI-I-1 --pos 3048x0

#Blend the two projectors
/home/norgeot/Téléchargements/nvidia-settings-390.25/samples/_out/Linux_x86_64/nv-control-warpblend-own
