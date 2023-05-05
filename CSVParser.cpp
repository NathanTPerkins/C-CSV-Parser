#include "CSVParser.h"
csv_parser::parser::parser(const char * filename, int precision = 10){
    this->m_fileName = new char[strlen(filename)];
    this->precision = precision;
    strcpy(this->m_fileName, filename);

    FILE *csv = fopen(this->m_fileName, "r");
    int col_count = 0, longest_col = 0; 
    this->columns = nullptr;
    this->columns_length = 0;
    if(getColumnData(csv, &col_count, &longest_col)){
        this->columns_length = col_count;
        this->longest_col = longest_col;
        this->columns = new char*[col_count];
        for(int i = 0; i < col_count; ++i)
            this->columns[i] = new char[longest_col];
    }
    setColumns(csv, &longest_col);
    this->size = getNumEntries(csv);
    this->data = new char**[this->size];
    for(int i = 0; i < this->size; ++i){
        this->data[i] = new char*[col_count];
    }
    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < col_count; ++j){
            this->data[i][j] = new char[NUM_LENGTH];
        }
    }
    setData(csv);
    fclose(csv);
}

void csv_parser::parser::operator=(const parser& p){
    this->size = p.size;
    this->columns_length = p.columns_length;
    this->precision = p.precision;
    this->m_fileName = new char[strlen(p.getFilename())];
    strcpy(this->m_fileName, p.getFilename());

    this->columns = new char*[p.columns_length];
    for(int i = 0; i < this->columns_length; ++i)
        this->columns[i] = new char[longest_col];
    
    for(int i = 0; i < this->columns_length; ++i){
        strcpy(this->columns[i], p.columns[i]);
    }

    this->data = new char**[this->size];
    for(int i = 0; i < this->size; ++i){
        this->data[i] = new char*[p.columns_length];
    }
    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < p.columns_length; ++j){
            this->data[i][j] = new char[NUM_LENGTH];
        }
    }

    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < p.columns_length; ++j){
            strcpy(this->data[i][j], p.data[i][j]);
        }
    }
}

csv_parser::parser::parser(const parser& p){
    this->size = p.size;
    this->columns_length = p.columns_length;
    this->precision = p.precision;
    this->m_fileName = new char[strlen(p.getFilename())];
    strcpy(this->m_fileName, p.getFilename());

    this->columns = new char*[p.columns_length];
    for(int i = 0; i < this->columns_length; ++i)
        this->columns[i] = new char[longest_col];
    
    for(int i = 0; i < this->columns_length; ++i){
        strcpy(this->columns[i], p.columns[i]);
    }

    this->data = new char**[this->size];
    for(int i = 0; i < this->size; ++i){
        this->data[i] = new char*[p.columns_length];
    }
    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < p.columns_length; ++j){
            this->data[i][j] = new char[NUM_LENGTH];
        }
    }

    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < p.columns_length; ++j){
            strcpy(this->data[i][j], p.data[i][j]);
        }
    }
}

char ** csv_parser::parser::operator[](int i){
    return this->data[i];
}

int csv_parser::parser::getNumEntries(FILE *input_file){
    if(input_file == NULL){
        return 0;
    }
    rewind(input_file);
    char c;
    int i = 0;
    while((c = getc(input_file)) != EOF){
        if(c == '\n'){
            ++i;
        }
    }
    return i;
}

u_int8_t csv_parser::parser::setData(FILE *input_file){
    if(input_file == NULL){
        return 0;
    }
    rewind(input_file);
    int i = 0, j = 0; 
    char data_val[NUM_LENGTH];
    char c;
    while((c = fgetc(input_file)) != EOF){
        if(c == '\n'){
            if(i != 0){
                strcpy(this->data[i - 1][j], data_val);
                memset(data_val, 0, NUM_LENGTH);
                
            }
            j = 0;
            ++i;
        }else if (c == ','){
            if(i != 0){
                strcpy(this->data[i - 1][j], data_val);
                memset(data_val, 0, NUM_LENGTH);
            }
            ++j;
        }else{
            if(i != 0){
                strncat(data_val, &c, 1);
            }
        }
        
    }
    strcpy(this->data[i - 1][j], data_val);
    return 1;
}

u_int8_t csv_parser::parser::setColumns(FILE *input_file, int* longest_col){
    if(input_file == NULL){
        return 0;
    }
    rewind(input_file);
    char col_name[*longest_col];
    char c;
    int i = 0, j = 0;
    while((c = getc(input_file)) != '\n'){
        if(c == ','){
            strcpy(this->columns[i],col_name);
            memset(col_name, 0, *longest_col);
            j = 0;
            ++i;
        }else{
            strncat(col_name, &c, 1);
        }
    }
    strcpy(this->columns[i],col_name);
    return 1;
}

u_int8_t csv_parser::parser::getColumnData(FILE *input_file, int *col_count, int *longest_col){
    if(input_file == NULL || *col_count != 0 || *longest_col != 0){
        return 0;
    }
    char c;
    int i = 0;
    *longest_col = 0;
    while((c = getc(input_file)) != '\n'){
        if(c == ','){
            ++(*col_count);
            if(i > *longest_col){
                *longest_col = i;
            }
        }else{
            ++i;
        }
    }
    ++(*col_count);
    return 1;
}

csv_parser::parser::~parser(){
    for(int i = 0; i < this->columns_length; ++i){
        delete [] this->columns[i];
    }
    delete [] this->columns;
    this->columns = nullptr;
    this->columns_length = 0;
    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < this->columns_length; ++j){
            delete [] this->data[i][j];
        }
    }
    delete [] this->data;
    this->data = nullptr;
    delete [] this->m_fileName;
    this->m_fileName = nullptr;
}

int csv_parser::parser::getSize() const {
    return this->size;
}

char ** csv_parser::parser::getColumns() const {
    return this->columns;
}

int csv_parser::parser::numColumns() const {
    return this->columns_length;
}

char* csv_parser::parser::getFilename() const {
    return this->m_fileName;
}