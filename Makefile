gridgen: algebraic.o boundary.o compspace.o cursor.o cut.o data.o distribute.o geometry.o gridgen.o interpolate.o laplace.o loc.o memory.o metrics.o middlecoff.o position.o quadrangle.o quality.o smooth.o spline.o structured.o sy.o triangle.o unstructured.o
	gcc -Wall -lm -o gridgen algebraic.o boundary.o compspace.o cursor.o cut.o data.o distribute.o geometry.o gridgen.o interpolate.o laplace.o loc.o memory.o metrics.o middlecoff.o position.o quadrangle.o quality.o smooth.o spline.o structured.o sy.o triangle.o unstructured.o

algebraic.o: algebraic.c gridgen.h algebraic.h compspace.h interpolate.h metrics.h
	gcc -Wall -c algebraic.c

boundary.o: boundary.c gridgen.h boundary.h distribute.h loc.h
	gcc -Wall -c boundary.c

compspace.o: compspace.c gridgen.h compspace.h loc.h
	gcc -Wall -c compspace.c

cursor.o: cursor.c cursor.h
	gcc -Wall -c cursor.c

cut.o: cut.c gridgen.h cut.h distribute.h
	gcc -Wall -c cut.c

data.o: data.c gridgen.h data.h loc.h
	gcc -Wall -c data.c

distribute.o: distribute.c gridgen.h distribute.h
	gcc -Wall -c distribute.c

geometry.o: geometry.c gridgen.h geometry.h boundary.h cut.h position.h spline.h
	gcc -Wall -c geometry.c

gridgen.o: gridgen.c gridgen.h data.h geometry.h memory.h structured.h unstructured.h quality.h
	gcc -Wall -c gridgen.c

interpolate.o: interpolate.c gridgen.h interpolate.h loc.h
	gcc -Wall -c interpolate.c

laplace.o: laplace.c gridgen.h laplace.h cursor.h loc.h metrics.h
	gcc -Wall -c laplace.c

loc.o: loc.c gridgen.h loc.h
	gcc -Wall -c loc.c

memory.o: memory.c gridgen.h memory.h
	gcc -Wall -c memory.c

metrics.o: metrics.c gridgen.h metrics.h loc.h
	gcc -Wall -c metrics.c

middlecoff.o: middlecoff.c gridgen.h middlecoff.h cursor.h metrics.h loc.h
	gcc -Wall -c middlecoff.c

position.o: position.c gridgen.h position.h
	gcc -Wall -c position.c

quadrangle.o: quadrangle.c gridgen.h loc.h quadrangle.h
	gcc -Wall -c quadrangle.c

quality.o: quality.c gridgen.h quality.h
	gcc -Wall -c quality.c

smooth.o: smooth.c gridgen.h cursor.h smooth.h loc.h
	gcc -Wall -c smooth.c

spline.o: spline.c gridgen.h spline.h sy.h distribute.h
	gcc -Wall -c spline.c

structured.o: structured.c gridgen.h algebraic.h laplace.h middlecoff.h structured.h quadrangle.h
	gcc -Wall -c structured.c

sy.o: sy.c gridgen.h sy.h
	gcc -Wall -c sy.c

triangle.o: triangle.c gridgen.h loc.h triangle.h
	gcc -Wall -c triangle.c

unstructured.o: unstructured.c metrics.h unstructured.h smooth.h triangle.h
	gcc -Wall -c unstructured.c

