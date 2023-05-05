all: csv

csv: CSVParser.cpp main.cpp
	@g++ $^ -o $@ -Wall

.PHONY: clean

clean:
	@rm -rf csv