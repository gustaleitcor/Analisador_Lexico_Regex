#include <iostream>
#include <regex>
#include <iterator>
#include <fstream>

typedef struct
{
    const std::string name;
    const std::regex pattern;
} Pattern;

const std::vector<Pattern>
    patterns = {
        {"REAL", std::regex("\\d+\\.\\d+")},
        {"INTEGER", std::regex("\\d+")},

        {"OPERATOR", std::regex("-")},
        {"OPERATOR", std::regex("\\+")},
        {"OPERATOR", std::regex("\\*")},
        {"OPERATOR", std::regex("/")},

        {"RELACIONAL_OPERATOR", std::regex("=")},
        {"RELACIONAL_OPERATOR", std::regex(">")},
        {"RELACIONAL_OPERATOR", std::regex("<")},
        {"RELACIONAL_OPERATOR", std::regex(">=")},
        {"RELACIONAL_OPERATOR", std::regex("<=")},

        {"LOGICAL_OPERATOR", std::regex("\\band\\b")},
        {"LOGICAL_OPERATOR", std::regex("\\bor\\b")},

        {"KEYWORD", std::regex("\\bprogram\\b")},
        {"KEYWORD", std::regex("\\bvar\\b")},
        {"KEYWORD", std::regex("\\binteger\\b")},
        {"KEYWORD", std::regex("\\breal\\b")},
        {"KEYWORD", std::regex("\\bif\\b")},
        {"KEYWORD", std::regex("\\bthen\\b")},
        {"KEYWORD", std::regex("\\belse\\b")},
        {"KEYWORD", std::regex("\\bwhile\\b")},
        {"KEYWORD", std::regex("\\bboolean\\b")},
        {"KEYWORD", std::regex("\\bprocedure\\b")},
        {"KEYWORD", std::regex("\\bbegin\\b")},
        {"KEYWORD", std::regex("\\bend\\b")},
        {"KEYWORD", std::regex("\\bdo\\b")},
        {"KEYWORD", std::regex("\\bnot\\b")},

        {"DELIMITER", std::regex("\\.")},
        {"DELIMITER", std::regex(",")},
        {"DELIMITER", std::regex(";")},
        {"DELIMITER", std::regex(":")},
        {"DELIMITER", std::regex("\\(")},
        {"DELIMITER", std::regex("\\)")},

        {"ATRIBUTION", std::regex(":=")},

        {"COMMENT", std::regex("\\{[^\\}]*\\}")},
        {"IDENTIFIER", std::regex("\\b[a-zA-Z][a-zA-Z0-9_]*\\b")},
};

int main(int argc, char **argv)
{
    std::string input, type, symbol;
    std::string::const_iterator start = input.begin(), end = input.end();
    std::smatch match_results;
    std::ifstream input_file;
    std::ofstream output_file;

    unsigned int match_size = 0, match_position = input.size(), line = 1;

    if (argc > 1)
    {
        if (argc == 2)
        {
            input_file.open(argv[1]);
            output_file.open("a.txt");
        }
        else if (argc == 3)
        {
            input_file.open(argv[1]);
            output_file.open(argv[2]);
        }
        else
        {
            std::cerr << "ERROR: Wrong amount of parameters" << std::endl;
            std::cout << "Quantidade de parâmetros errados\nTente: analyse <input_file> <output_file>" << std::endl;
        }
    }

    if (!input_file)
    {
        std::cerr << "ERROR: Could not open file" << std::endl;
        std::cout << "Não foi possivel abrir o arquivo " << argv[1] << "\nTente novamente" << std::endl;
        return -1;
    }
    if (!output_file)
    {
        std::cerr << "ERROR: Could not create file" << std::endl;
        std::cout << "Não foi possivel criar o arquivo" << argv[2] << "\nTente novamente" << std::endl;
        return -1;
    }

    while (getline(input_file, input))
    {
        start = input.begin();
        end = input.end();
        while (true)
        {
            match_size = 0;
            match_position = input.size();

            for (auto pattern : patterns)
            {
                if (std::regex_search(start, end, match_results, pattern.pattern))
                {
                    if (match_results.position() == match_position)
                    {
                        if (match_results.length() > match_size)
                        {
                            match_size = match_results.length();
                            type = pattern.name;
                            symbol = match_results[0];
                        }
                    }
                    else if (match_results.position() < match_position)
                    {
                        match_position = match_results.position();
                        match_size = match_results.length();
                        type = pattern.name;
                        symbol = match_results[0];
                    }
                }
            }

            if (match_size == 0)
                break;

            output_file << type << ' ' << symbol << ' ' << line << std::endl;
            // std::cout << type << ' ' << symbol << ' ' << line << std::endl;

            start += match_position + match_size;
        }
        line++;
    }

    input_file.close();
    output_file.close();
}
