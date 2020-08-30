make: clean
	clang -o pa4 -std=c99 -Wall -pedantic *.c -L. -lruntime
run: make
	./pa4 --mutex -p 3
clean:
	-rm *.log pa4 *.gz
tar: clean
	cd .. && tar -czvf pa4.tar.gz pa4
