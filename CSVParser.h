#ifndef CSV_PARSER
#define CSV_PARSER

#include <string.h>
#include <stdio.h>
#ifdef ARDUINO_PARSER
#include <SD.h>
#include <Arduino.h>
#endif

#define NUM_LENGTH 100

namespace csv_parser{

    enum ERROR_CODES{FILE_NOT_FOUND};

    class parser final{
    private:
        char *m_fileName;
        char **columns;
        int columns_length;
        int size;
        int longest_col;
        char ***data;
        int precision;

        u_int8_t getColumnData(FILE *, int *, int *);
        u_int8_t setColumns(FILE *, int*);
        int getNumEntries(FILE *);
        u_int8_t setData(FILE *);
    public:
        parser() = delete;
        parser(const parser&);
        parser& operator=(const parser&);
        char ** operator[](int)const;
        parser(const char *, int);
        ~parser();

        int getSize()const;
        FILE *toCSV(const char *)const;
        char **getColumns()const;
        int numColumns()const;
        char * getFilename()const;
        parser& operator+=(const parser&);
        u_int8_t concat(const parser&);
        void head(int);
        void tail(int);
    };

    #ifdef ARDUINO_PARSER
    class arduino_parser final{
    private:
        char *m_fileName;
        char **columns;
        int columns_length;
        int size;
        int longest_col;
        char ***data;
        int precision;

        u_int8_t getColumnData(File&, int *, int *);
        u_int8_t setColumns(File&, int*);
        int getNumEntries(File&);
        u_int8_t setData(File&);
    public:
        arduino_parser() = delete;
        arduino_parser(const arduino_parser&);
        arduino_parser& operator=(const arduino_parser&);
        arduino_parser& operator+=(const arduino_parser&);
        char ** operator[](int);
        arduino_parser(const char *, int);
        ~arduino_parser();

        File *toCSV()const;
        u_int8_t concat(const parser&);
        void rewind(File*);
        int getSize()const; 
        char **getColumns()const;
        int numColumns()const;
        char * getFilename()const;
        void head(int);
        void tail(int);
    };
    #endif

}

#endif