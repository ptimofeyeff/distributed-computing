make: clean
	clang -o pa3 -std=c99 -Wall -pedantic *.c -L. -lruntime
run: make
	./pa3 -p 2 10 20
clean:
	-rm *.log pa3 *.gz
tar: clean
	cd .. && tar -czvf pa3.tar.gz pa3
