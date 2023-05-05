all: csv

csv: CSVParser.cpp main.cpp
	@g++ $^ -o $@

.PHONY: clean

clean:
	@rm -rf csv