#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>

typedef struct {
  const std::string name;
  const std::regex pattern;
} Pattern;

const std::vector<Pattern> patterns = {
    {"REAL", std::regex("\\b\\d+\\.\\d+\\b")},
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
    {"RELACIONAL_OPERATOR", std::regex("<>")},

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

    {"ASSIGN", std::regex(":=")},

    {"COMMENT", std::regex("\\{[^\\}]*\\}")},
    {"OPEN_COMMENT", std::regex("\\{.*")},

    {"IDENTIFIER", std::regex("\\b[a-zA-Z]\\w*\\b")},
};

int main(int argc, char **argv) {
  std::string input, type, symbol, unknown;
  std::string::const_iterator start = input.begin(), end = input.end(),
                              last_start;
  std::smatch match_results;
  std::ifstream input_file;
  std::ofstream output_file;

  unsigned int match_size = 0, match_position = input.size(), line = 1;
  bool opened_comment = false;

  // Trata a entrada dos parametros argc, argv
  if (argc > 1) {
    if (argc == 2) {
      input_file.open(argv[1]);
      output_file.open("a.txt");
    } else if (argc == 3) {
      input_file.open(argv[1]);
      output_file.open(argv[2]);
    } else {
      std::cerr << "ERROR: Wrong amount of parameters" << std::endl;
      std::cout << "Quantidade de parâmetros errados\nTente: analyse "
                   "<input_file> <output_file>"
                << std::endl;
    }
  }

  // Verifica se os arquivos foram abertos normalmente

  if (!input_file) {
    std::cerr << "ERROR: Could not open file" << std::endl;
    std::cout << "Não foi possivel abrir o arquivo " << argv[1]
              << "\nTente novamente" << std::endl;
    return -1;
  }
  if (!output_file) {
    std::cerr << "ERROR: Could not create file" << std::endl;
    std::cout << "Não foi possivel criar o arquivo" << argv[2]
              << "\nTente novamente" << std::endl;
    return -1;
  }

  // Inicia a leitura do arquivo

  while (getline(input_file, input)) {
    start = input.begin();
    end = input.end();
    last_start = input.begin();
    while (true) {
      match_size = 0;
      match_position = input.size();

      for (auto pattern : patterns) {
        // procura por uma match por todos os padrões
        if (std::regex_search(start, end, match_results, pattern.pattern)) {
          // Se a procura resultou em um match, duas condições são verificadas:

          // 1: Caso a posição do match corrente seja menor do que a menor
          // posição previamente escolhida, o match corrente torna-se o
          // escolhido Em outras palavras, o match mais a esquerda é escolhido
          if (match_results.position() < match_position) {
            match_position = match_results.position();
            match_size = match_results.length();
            type = pattern.name;
            symbol = match_results[0];
          }
          // 2: Caso a posição do match corrente coincida com o match
          // previamente escolhido, torna-se o escolhido aquele que tem mais
          // caracteres Importante para tratar os casos de <= e <, em que ambos
          // começam no mesmo lugar, porém o <= contém mais caracteres
          else if (match_results.position() == match_position) {
            if (match_results.length() > match_size) {
              match_size = match_results.length();
              type = pattern.name;
              symbol = match_results[0];
            }
          }
        }
      }

      // Caso a busca não encontre um match, interrompe o laço e vai para a
      // proxima linha (se possivel)

      if (match_size == 0)
        break;

      // Se foi encontrado um comentário aberto ou um comentário ou se o
      // comentário estiver aberto
      if (type == "OPEN_COMMENT" || type == "COMMENT" || opened_comment) {
        opened_comment = true;

        // É performado uma busca ao caracter '}', local de fechamento do
        // comentário
        auto closing_brace_pos = input.find('}');

        // Caso seja achado o fechamento do comentário, a execução do algoritmo
        // volta a partir do '}'
        if (closing_brace_pos != std::string::npos) {
          start = input.begin() + closing_brace_pos;
          opened_comment = false;
        }
        // Caso contrário, pula-se a linha
        else {
          start = end;
          break;
        }
        continue;
      }

      // A escrita é performada no arquivo de saída
      output_file << type << ' ' << symbol << ' ' << line << std::endl;
      // std::cout << type << ' ' << symbol << ' ' << line << std::endl;

      // Atualiza-se o inicio para logo após ao match encontrado
      start += match_position + match_size;

      // Verifica se palavras há algum caractere entre os matches
      unknown = input.substr(last_start - input.begin(),
                             start - last_start - match_size);

      for (auto c : unknown) {
        if (c != ' ') {
          std::cout << "ERROR: caracter desconhecido pela linguagem: "
                    << unknown << " linha: " << line << std::endl;
          break;
        }
      }

      last_start = start;
    }

    if (start - input.begin() != input.size()) {
      unknown = input.substr(start - input.begin() + 1, start - input.end());
      for (auto c : unknown) {
        if (c != ' ') {
          std::cout << "ERROR: caracter desconhecido pela linguagem: "
                    << unknown << " linha: " << line << std::endl;
          break;
        }
      }
    }

    // Linha += 1
    line++;
  }

  // Fecha os arquivos
  input_file.close();
  output_file.close();

  return 0;
}
