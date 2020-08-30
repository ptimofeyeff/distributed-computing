make: clean
	clang -o pa5 -std=c99 -Wall -pedantic *.c -L. -lruntime
run: make
	./pa5 --mutex -p 3
clean:
	-rm *.log pa5 *.gz
tar: clean
	cd .. && tar -czvf pa5.tar.gz pa5
