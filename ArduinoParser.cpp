#include "CSVParser.h"
#ifdef ARDUINO_PARSER
csv_parser::arduino_parser::arduino_parser(const char * filename, int precision = 10){
    this->m_fileName = new char[strlen(filename) + 1];
    this->precision = precision;
    memset(this->m_fileName, 0, strlen(filename) + 1);
    strcpy(this->m_fileName, filename);
    File csv = SD.open(this->m_fileName, FILE_READ);
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

    rewind(&csv);
    setColumns(csv, &longest_col);

    rewind(&csv);
    this->size = getNumEntries(csv);
    rewind(&csv);
    this->data = (char ***)extmem_malloc(sizeof(char**) * this->size);
    int i = 0;
    i = 0;
    for(int i = 0; i < this->size; ++i){
        this->data[i] = (char **)extmem_malloc(sizeof(char*) * col_count);
        for(int j = 0; j < col_count; ++j){
            Serial.printf("i: %d, j: %d\n", i, j);
            this->data[i][j] = (char *)extmem_malloc(sizeof(char) * longest_col);
            memset(this->data[i][j], 0, NUM_LENGTH);
        }
    }
    setData(csv);
    rewind(&csv);
    csv.close();
}

csv_parser::arduino_parser& csv_parser::arduino_parser::operator=(const arduino_parser& p){
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

    return *this;
}

csv_parser::arduino_parser::arduino_parser(const arduino_parser& p){
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

char ** csv_parser::arduino_parser::operator[](int i)const{
    return this->data[i];
}

void csv_parser::arduino_parser::rewind(File* input_file){
    input_file->close();
    *input_file = SD.open(this->m_fileName, FILE_READ);
}

int csv_parser::arduino_parser::getNumEntries(File& input_file){
    if(!input_file ){
        return 0;
    }
    rewind(&input_file);
    char c;
    int i = 0;
    while(input_file.available()){
        c = input_file.read();
        if(c == '\n'){
            ++i;
        }
    }
    return i;
}

u_int8_t csv_parser::arduino_parser::setData(File& input_file){
    if(!input_file ){
        return 0;
    }
    rewind(&input_file);
    int i = 0, j = 0; 
    char data_val[NUM_LENGTH];
    memset(data_val, 0, NUM_LENGTH);
    char c;
    while(input_file.available()){
        c = input_file.read();
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

u_int8_t csv_parser::arduino_parser::setColumns(File& input_file, int* longest_col){
    if(!input_file){
        return 0;
    }
    rewind(&input_file);
    char col_name[*longest_col];
    memset(col_name, 0, *longest_col);
    char c;
    int i = 0;
    while((c = input_file.read()) != '\n'){
        if(c == ','){
            strcpy(this->columns[i],col_name);
            memset(col_name, 0, *longest_col);
            ++i;
        }else{
            strncat(col_name, &c, 1);
        }
    }
    strcpy(this->columns[i],col_name);
    return 1;
}

u_int8_t csv_parser::arduino_parser::getColumnData(File& input_file, int *col_count, int *longest_col){
    if(!input_file || *col_count != 0 || *longest_col != 0){
        return 0;
    }
    char c;
    int i = 0;
    *longest_col = 0;
    while((c = input_file.read()) != '\n'){
        if(c == ','){
            ++(*col_count);
            if(i > *longest_col){
                *longest_col = i;
            }
        }else{
            ++i;
        }
    }
    if(i > *longest_col){
        *longest_col = i;
    }
    ++(*col_count);
    return 1;
}

csv_parser::arduino_parser::~arduino_parser(){
    for(int i = 0; i < this->columns_length; ++i){
        delete [] this->columns[i];
    }
    delete [] this->columns;
    this->columns = nullptr;
    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < this->columns_length; ++j){
            extmem_free(this->data[i][j]);
        }
        extmem_free(this->data[i]);
    }
    extmem_free(this->data);
    this->data = nullptr;
    delete [] this->m_fileName;
    this->m_fileName = nullptr;
}

void csv_parser::arduino_parser::head(int numRows = 5){
    if(numRows > this->size){
        return;
    }
    for(int i = 0; i < this->columns_length; ++i){
        Serial.printf("%s\t", this->columns[i]);
    }
    Serial.printf("\n");
    for(int i = 0; i < this->columns_length; ++i){
        for(int j = 0; j < strlen(this->columns[i]); ++j){
            Serial.printf("--");
        }
    }
    Serial.printf("\n");
    for(int i = 0; i < numRows; ++i){
        for(int j = 0; j < this->columns_length; ++j){
            Serial.printf("%8.8s\t", this->data[i][j]);
        }
        Serial.printf("\n");
    }
    Serial.printf("\n");
}

void csv_parser::arduino_parser::tail(int numRows = 5){
    if(numRows > this->size){
        return;
    }
    for(int i = 0; i < this->columns_length; ++i){
        Serial.printf("%s\t", this->columns[i]);
    }
    Serial.printf("\n");
    for(int i = 0; i < this->columns_length; ++i){
        for(int j = 0; j < strlen(this->columns[i]); ++j){
            Serial.printf("--");
        }
    }
    Serial.printf("\n");
    for(int i = this->size - (numRows); i < this->size; ++i){
        for(int j = 0; j < this->columns_length; ++j){
            Serial.printf("%8.8s\t", this->data[i][j]);
        }
        Serial.printf("\n");
    }
    Serial.printf("\n");
}

// File * csv_parser::arduino_parser::toCSV(const char * filename)const{
//     File *csv = new File;
//     *csv = SD.open(filename, FILE_WRITE);
//     for(int i = 0; i < this->columns_length - 1; ++i){
//         fprintf(csv, "%s,", this->columns[i]);
//     }
//     fprintf(csv, "%s", this->columns[this->columns_length - 1]);
//     fprintf(csv,"\n");
    
//     for(int i = 0; i < this->size; ++i){
//         for(int j = 0; j < this->columns_length - 1; ++j){
//             fprintf(csv,"%s,", this->data[i][j]);
//         }
//         fprintf(csv, "%s", this->data[i][this->columns_length - 1]);
//         if(i != this->size - 1){
//             fprintf(csv, "\n");
//         }
//     }
//     return csv;
// }

u_int8_t csv_parser::arduino_parser::concat(const csv_parser::arduino_parser& p){
    if(p.columns_length != this->columns_length){
        return 0;
    }
    char *** new_data = new char**[this->size + p.getSize()];
    for(int i = 0; i < this->size + p.getSize(); ++i){
        new_data[i] = new char*[this->columns_length];
    }
    for(int i = 0; i < this->size + p.getSize(); ++i){
        for(int j = 0; j < this->columns_length; ++j){
            new_data[i][j] = new char[NUM_LENGTH];
        }
    }
    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < this->columns_length; ++j){
            strcpy(new_data[i][j], this->data[i][j]);
        }
    }
    
    for(int i = this->size; i < this->size + p.getSize(); ++i){
        for(int j = 0; j < this->columns_length; ++j){
            strcpy(new_data[i][j], p[i - this->size][j]);
        }
    }
    for(int i = 0; i < this->size; ++i){
        for(int j = 0; j < this->columns_length; ++j){
            delete [] this->data[i][j];
        }
        delete [] this->data[i];
    }
    delete [] this->data;
    this->data = new_data;
    this->size += p.getSize();
    return 1;

}

csv_parser::arduino_parser& csv_parser::arduino_parser::operator+=(const arduino_parser& p){
    concat(p);
    return *this;
}

int csv_parser::arduino_parser::getSize() const {
    return this->size;
}

char ** csv_parser::arduino_parser::getColumns() const {
    return this->columns;
}

int csv_parser::arduino_parser::numColumns() const {
    return this->columns_length;
}

char* csv_parser::arduino_parser::getFilename() const {
    return this->m_fileName;
}
#endif