CC 	 =	g++
LIBS	 = 	-lm 

translator: converter_test.o Point.o Converter.o
	g++ -o translator converter_test.o Point.o Converter.o

translator.db: converter_test.db.o Point.db.o Converter.db.o
	g++ -o translator.db converter_test.o Point.o Converter.o

clean:
	rm *.o *~ mazeRoute mazeRoute.db mazeRoute.prof

# ------------------ Debug compile ------------------------------

converter_test.db.o: converter_test.cc Point.db.o Converter.db.o
	g++ -c -g converter_test.cc

Point.db.o: Point.h Point.cc 
	g++ -c -g Point.cc

Converter.db.o: Converter.h Converter.cc Pin.h
	g++ -c -g Converter.cc

# ----------------------- Optimized compile ----------------------------

converter_test.o: converter_test.cc Point.o Converter.o
	g++ -c -O2 converter_test.cc

Point.o: Point.h Point.cc 
	g++ -c -O2 Point.cc

Converter.o: Converter.h Converter.cc Pin.h
	g++ -c -O2 Converter.cc
