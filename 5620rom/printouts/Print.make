echo "\n\n\n\n\n"
banner makefiles
date
echo "\f\c"

(
cat <<!
src/term
src/xt/layersys
src/xt/layersys/rdpatch
src/xt/layersys/roms
src/lib/libc
src/lib/libj
src/lib/liblayer
src/lib/libsys
!
) | while read a
	do
	pr -f -h "$a/makefile" ../$a/makefile
	done

(
cat <<!
src/term/map
src/xt/layersys/comb-map
!
) | while read a
	do
	pr -f -h "$a" ../$a
	done
