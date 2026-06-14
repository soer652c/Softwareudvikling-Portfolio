#include "Input.h"

#include <iostream>
#include <limits>

Input::Input(std::istream& input, std::ostream& output) : input_(input), output_(output) {}

int Input::readIntInRange(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        output_ << prompt;

        int value = 0;
        if (input_ >> value && value >= minimum && value <= maximum) {
            input_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        output_ << "Skriv et tal mellem " << minimum << " og " << maximum << ".\n";
        input_.clear();
        input_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string Input::readNonEmptyLine(const std::string& prompt) {
    while (true) {
        output_ << prompt;

        std::string line;
        std::getline(input_, line);

        if (!line.empty()) {
            return line;
        }

        output_ << "Teksten maa ikke vaere tom.\n";
    }
}

bool Input::readYesNo(const std::string& prompt) {
    while (true) {
        output_ << prompt << " (j/n): ";

        std::string answer;
        std::getline(input_, answer);

        if (answer == "j" || answer == "J" || answer == "ja" || answer == "Ja") {
            return true;
        }

        if (answer == "n" || answer == "N" || answer == "nej" || answer == "Nej") {
            return false;
        }

        output_ << "Svar j eller n.\n";
    }
}

void Input::waitForEnter() {
    output_ << "\nTryk enter for at fortsaette...";
    std::string ignored;
    std::getline(input_, ignored);
    output_ << "\n";
}
