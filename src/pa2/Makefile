make: clean
	clang -o pa2 -std=c99 -Wall -pedantic *.c -L. -lruntime
run: make
	./pa2 -p 2 10 20
clean:
	-rm *.log pa2 *.gz
tar: clean
	cd .. && tar -czvf pa2.tar.gz pa2
