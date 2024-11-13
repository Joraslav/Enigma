#include <iostream>
#include <map>
#include <random>
#include <string>
#include <utility>

template <class Key, class Val>
using map = std::map<Key, Val>;

using string = std::string;

class Rotor
{
private:
    map<char, char> table_;
    map<char, char> reverse_table_;
    char main_later_ = 'a';
    int later_ = 0;

public:
    Rotor() = default;
    Rotor(const char later);

    Rotor(const Rotor &other);
    Rotor &operator=(const Rotor &other);

    char GetMainLater();
    int GetLaterInt();
    char GetWhenIn(const char c);
    char GetWhenOut(const char c);
    void SetMainLater(const char later);
    void MakeRotate(const int rot);
    char CalculateNextLater(const char input, const int prev_main_later = 0);
};

Rotor::Rotor(const char later)
{
    for (size_t i = 0; i != 26; ++i)
    {
        table_.insert({static_cast<char>(i + 97), static_cast<char>(122 - i)});
        reverse_table_.insert({static_cast<char>(122 - i), static_cast<char>(i + 97)});
    }
    SetMainLater(later);
    later_ = static_cast<int>(main_later_);
}

Rotor::Rotor(const Rotor &other)
{
    table_ = other.table_;
    main_later_ = other.main_later_;
}

Rotor &Rotor::operator=(const Rotor &other)
{
    if (this != &other)
    {
        Rotor temp(other);
        std::swap(table_, temp.table_);
        std::swap(main_later_, temp.main_later_);
    }
    return *this;
}

char Rotor::GetMainLater() { return main_later_; }

int Rotor::GetLaterInt() { return later_; }

char Rotor::GetWhenIn(const char c) { return table_.at(c); }

char Rotor::GetWhenOut(const char c) { return reverse_table_.at(c); }

void Rotor::SetMainLater(const char later) { main_later_ = later; }

void Rotor::MakeRotate(const int rot)
{
    later_ = (later_ - 97 + rot) % 26 + 97;
    main_later_ = static_cast<char>(later_);
}

char Rotor::CalculateNextLater(const char input, const int prev_main_later)
{
    int input_int = static_cast<int>(input);
    int in_correct = input_int - 97;
    int later_correct = later_ - 97;
    int prev_correct = 0;
    if (prev_main_later != 0)
    {
        prev_correct = prev_main_later - 97;
    }
    int rez = (in_correct + abs(later_correct - prev_correct)) % 26;
    return static_cast<char>(rez + 97);
}

/*Рефлектор*/

class Reflector
{
private:
    map<char, char> table_;

public:
    Reflector(/* args */);

    char CalculateReflect(const char input, const int prev_main_later);
};

Reflector::Reflector(/* args */)
{
    for (size_t i = 97; i < 111; ++i)
    {
        table_.insert({static_cast<char>(i), static_cast<char>(i + 12)});
        table_.insert({static_cast<char>(i + 12), static_cast<char>(i)});
    }
}

char Reflector::CalculateReflect(const char input, const int prev_main_later)
{
    int in_correct = input - 97;
    int rez = (prev_main_later - in_correct) % 26;
    return table_.at(static_cast<char>(rez) + 97);
}

/*Энигма*/

enum class CodeStatus
{
    IN,
    OUT,
};

class Enigma
{
private:
    string output_;
    Rotor &rot_1_, &rot_2_, &rot_3_;
    Reflector &reflector_;

public:
    Enigma(Rotor &rot_1, Rotor &rot_2, Rotor &rot_3, Reflector &ref) : rot_1_(rot_1), rot_2_(rot_2), rot_3_(rot_3), reflector_(ref) {};

    char Code(const char c, CodeStatus status = CodeStatus::IN);
    void Run(const string &text);
    string GetOutput();
};

char Enigma::Code(const char c, CodeStatus status)
{
    char rez;
    char first, second, thrid;
    char after_first, after_second, after_thrid;
    switch (status)
    {
    case CodeStatus::OUT:
        first = rot_3_.CalculateNextLater(c);
        after_first = rot_3_.GetWhenOut(first);
        second = rot_2_.CalculateNextLater(after_first, rot_3_.GetLaterInt());
        after_second = rot_2_.GetWhenOut(second);
        thrid = rot_1_.CalculateNextLater(after_second, rot_2_.GetLaterInt());
        after_thrid = rot_1_.GetWhenOut(thrid);
        rez = after_thrid;
        break;

    case CodeStatus::IN:
        first = rot_1_.CalculateNextLater(c);
        after_first = rot_1_.GetWhenIn(first);
        second = rot_2_.CalculateNextLater(after_first, rot_1_.GetLaterInt());
        after_second = rot_2_.GetWhenIn(second);
        thrid = rot_3_.CalculateNextLater(after_second, rot_2_.GetLaterInt());
        after_thrid = rot_3_.GetWhenIn(thrid);
        rez = after_thrid;
        break;
    }
    return rez;
}

void Enigma::Run(const string &text)
{
    for (const char c : text)
    {
        rot_1_.MakeRotate(1);
        char after_in = Code(c);
        char after_reflect = reflector_.CalculateReflect(after_in, rot_3_.GetLaterInt());
        char after_out = Code(after_reflect, CodeStatus::OUT);
        output_ += after_out;
    }
}

string Enigma::GetOutput() { return output_; }

/*Вспомогающие алгоритмы*/

string ParseLine(const string &input_line)
{
    string words;
    for (const char c : input_line)
    {
        if (std::isspace(c) || std::isdigit(c) || std::ispunct(c))
        {
            continue;
        }
        else
        {
            if (std::isupper(c))
            {
                char c_lower = std::tolower(c);
                words += c_lower;
            }
            else
            {
                words += c;
            }
        }
    }
    return words;
}

int main()
{
    Rotor rot_1('r'), rot_2('v'), rot_3('c');
    Reflector reflector;
    Enigma enigma(rot_1, rot_2, rot_3, reflector);
    string input = "xwv";
    string parse_input = ParseLine(input);
    enigma.Run(parse_input);
    std::cout << enigma.GetOutput() << std::endl;
    return 0;
}