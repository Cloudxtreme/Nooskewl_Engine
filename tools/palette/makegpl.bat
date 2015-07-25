grep "^rgb: " %1 | sort | uniq > %1.tmp1
sort_rgblist_by_hue < %1.tmp1 > %1.tmp2
rgblist_to_gpl %1.tmp2 %1.gpl
del %1.tmp1
del %1.tmp2
cat rgb.txt.gpl | sed -e 's/#/#\n255   0 255\tUntitled/' > palette.gpl
del %1.gpl