#ifndef INPUT_H
#define INPUT_H

#include <iosfwd>
#include <string>

class Input {
public:
    Input(std::istream& input, std::ostream& output);

    int readIntInRange(const std::string& prompt, int minimum, int maximum);
    std::string readNonEmptyLine(const std::string& prompt);
    bool readYesNo(const std::string& prompt);
    void waitForEnter();

private:
    std::istream& input_;
    std::ostream& output_;
};

#endif
