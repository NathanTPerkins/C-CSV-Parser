all: csv

csv: CSVParser.cpp CSVParser.h
	@g++ $< -o $@

.PHONY: clean

clean:
	@rm -rf csv