#ifndef TXT_H_INCLUDED
#define TXT_H_INCLUDED

#include <fstream>


namespace l1 {
    class Txt {
    private:
        std::ifstream::char_type* _content;
        std::size_t _number_of_lines;
        std::size_t _number_of_chars;

    public:
        Txt(const std::string& filename = "");
        Txt(const Txt& other);
        Txt(Txt&& other);
        Txt& operator=(const Txt& other);
        Txt& operator=(Txt&& other);

        ~Txt();

        std::size_t size() const;
    };
}

#endif // TXT_H_INCLUDED
