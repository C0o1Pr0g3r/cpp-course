#include "Txt.h"

#include <cstring>


namespace l1 {
    Txt::Txt(const std::string& filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            this->_content = nullptr;
            this->_number_of_lines = 0;
            this->_number_of_chars = 0;

            return;
        }

        file.seekg(0, std::ios_base::end);

        this->_number_of_chars = file.tellg();
        this->_content = new char[this->_number_of_chars + 1];

        file.seekg(0, std::ios_base::beg);

        file.read(this->_content, this->_number_of_chars);
        this->_content[this->_number_of_chars] = '\0';

        this->_number_of_lines = 0;
        for (std::size_t i = 0; i < this->_number_of_chars; ++i) {
            if (this->_content[i] == '\n') {
                ++this->_number_of_lines;
            }
        }

        file.close();
    }


    Txt::Txt(const Txt& other)
        : _number_of_lines(other._number_of_lines),
          _number_of_chars(other._number_of_chars) {
        if (!other._number_of_chars) {
            this->_content = nullptr;

            return;
        }

        this->_content = new char[this->_number_of_chars + 1];
        strcpy(this->_content, other._content);
    }


    Txt::Txt(Txt&& other)
        : _content(other._content),
          _number_of_lines(other._number_of_lines),
          _number_of_chars(other._number_of_chars) {
        other._content = nullptr;
        other._number_of_lines = 0;
        other._number_of_chars = 0;
    }


    Txt& Txt::operator=(const Txt& other) {
        if (this != &other) {
            delete[] this->_content;
            this->_number_of_lines = other._number_of_lines;
            this->_number_of_chars = other._number_of_chars;
            if (other._number_of_chars) {
                this->_content = new char[this->_number_of_chars + 1];
                strcpy(this->_content, other._content);
            } else {
                this->_content = nullptr;
            }
        }

        return *this;
    }



    Txt& Txt::operator=(Txt&& other) {
        if (this != &other) {
            delete[] this->_content;
            this->_number_of_lines = other._number_of_lines;
            this->_number_of_chars = other._number_of_chars;
            this->_content = other._content;
            other._content = nullptr;
            other._number_of_lines = 0;
            other._number_of_chars = 0;
        }

        return *this;
    }


    Txt::~Txt() {
        delete[] this->_content;
    }


    std::size_t Txt::size() const {
        return this->_number_of_lines;
    }
}
