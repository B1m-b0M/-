#include <iostream>     // Для введення/виведення даних
#include <fstream>      // Для роботи з файлами
#include <string>       // Для роботи з рядками
#include <vector>       // Контейнер vector
#include <map>          // Асоціативний контейнер map
#include <set>          // Асоціативний контейнер set
#include <algorithm>    // Для використання алгоритмів STL
#include <sstream>      // Для обробки рядків як потоків
#include <iomanip>      // Для форматування виведення
#include <filesystem>   // Для роботи з файловою системою (C++17)

// Структура для представлення футболіста (бомбардира)
struct Player {
    int number;         // Номер гравця
    std::string surname; // Прізвище
    std::string name;    // Ім'я
    
    // Конструктор за замовчуванням
    Player() : number(0) {}
    
    // Конструктор з параметрами
    Player(int num, const std::string& sur, const std::string& n) 
        : number(num), surname(sur), name(n) {}
    
    // Оператор порівняння для використання в контейнері set
    bool operator<(const Player& other) const {
        if (surname != other.surname) return surname < other.surname;
        if (name != other.name) return name < other.name;
        return number < other.number;
    }
    
    // Оператор порівняння на рівність
    bool operator==(const Player& other) const {
        return number == other.number && 
               surname == other.surname && 
               name == other.name;
    }
};

// Структура для представлення голу
struct Goal {
    Player player;      // Гравець, який забив гол
    std::string team;   // Команда (країна)
    int minute;         // Хвилина, коли був забитий гол
    
    // Конструктор з параметрами
    Goal(const Player& p, const std::string& t, int min) 
        : player(p), team(t), minute(min) {}
};

// Структура для представлення матчу
struct Match {
    std::string team1;          // Перша команда (країна)
    std::string team2;          // Друга команда (країна)
    int score1;                 // Кількість голів першої команди
    int score2;                 // Кількість голів другої команди
    std::vector<Goal> goals;    // Список голів у матчі
    
    // Конструктор за замовчуванням
    Match() : score1(0), score2(0) {}
    
    // Конструктор з параметрами
    Match(const std::string& t1, const std::string& t2, int s1, int s2) 
        : team1(t1), team2(t2), score1(s1), score2(s2) {}
    
    // Метод для додавання голу
    void addGoal(const Goal& goal) {
        goals.push_back(goal);
    }
    
    // Отримати рахунок у вигляді рядка
    std::string getScoreString() const {
        return team1 + " " + std::to_string(score1) + ":" + 
               std::to_string(score2) + " " + team2;
    }
};

// Структура для представлення інформації про бомбардира
struct ScorerInfo {
    std::string team;           // Команда (країна)
    std::set<int> matchIndices; // Індекси матчів, у яких гравець забивав
    int totalGoals;             // Загальна кількість голів
    
    // Конструктор за замовчуванням
    ScorerInfo() : totalGoals(0) {}
};

// Функція для перевірки наявності файлу
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// Функція для очищення екрану (кросплатформна)
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Функція для розділення рядка за роздільником
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Функція для читання даних з файлу з обробкою помилок
std::vector<Match> readMatchesFromFile(const std::string& filename, int& totalMatches, int& totalGoals) {
    std::ifstream file(filename);
    std::vector<Match> matches;
    totalGoals = 0;

    if (!file.is_open()) {
        std::cerr << "Помилка відкриття файлу: " << filename << std::endl;
        return matches;
    }

    std::string line;
    
    try {
        // Читаємо кількість матчів
        if (std::getline(file, line)) {
            std::istringstream iss(line);
            if (!(iss >> totalMatches)) {
                throw std::runtime_error("Некоректний формат числа матчів");
            }
        } else {
            throw std::runtime_error("Файл порожній");
        }
        
        // Перевірка на коректність числа матчів
        if (totalMatches <= 0) {
            throw std::runtime_error("Некоректна кількість матчів: " + std::to_string(totalMatches));
        }
        
        // Читаємо дані про кожен матч
        for (int i = 0; i < totalMatches; ++i) {
            if (!std::getline(file, line)) {
                throw std::runtime_error("Недостатньо даних у файлі для матчу " + std::to_string(i+1));
            }
            
            // Виводимо рядок для діагностики
            std::cout << "Рядок матчу " << (i+1) << ": " << line << std::endl;
            
            std::vector<std::string> parts = splitString(line, ';');
            if (parts.size() < 5) {
                throw std::runtime_error("Некоректний формат даних матчу " + std::to_string(i+1) + 
                                        " (кількість частин: " + std::to_string(parts.size()) + ")");
            }
            
            std::string team1 = parts[0];
            std::string team2 = parts[1];
            int score1, score2, playerCount;
            
            try {
                score1 = std::stoi(parts[2]);
                score2 = std::stoi(parts[3]);
                playerCount = std::stoi(parts[4]);
            } catch (const std::exception& e) {
                throw std::runtime_error("Некоректний формат чисел в матчі " + std::to_string(i+1));
            }
            
            std::cout << "Матч " << (i+1) << ": " << team1 << " vs " << team2 << ", Рахунок: " 
                      << score1 << ":" << score2 << ", Гравців: " << playerCount << std::endl;
            
            Match match(team1, team2, score1, score2);
            
            // Перевіряємо, чи достатньо даних для всіх бомбардирів
            // Кожен бомбардир має 5 полів: номер, прізвище, ім'я, команда, хвилина
            if (parts.size() < 5 + playerCount * 5) {
                throw std::runtime_error("Недостатньо даних про бомбардирів у матчі " + std::to_string(i+1) + 
                                        ". Потрібно: " + std::to_string(5 + playerCount * 5) + 
                                        ", Є: " + std::to_string(parts.size()));
            }

            // Читаємо дані про кожного бомбардира
            for (int j = 0; j < playerCount; ++j) {
                int index = 5 + j * 5; // Кожен гравець займає 5 полів
                
                if (index + 4 >= parts.size()) {
                    throw std::runtime_error("Вихід за межі даних для бомбардира " + 
                                        std::to_string(j+1) + " в матчі " + std::to_string(i+1) + 
                                        " (індекс: " + std::to_string(index) + ", розмір: " + 
                                        std::to_string(parts.size()) + ")");
                }
                
                int number, minute;
                std::string surname, name, team;
                
                try {
                    number = std::stoi(parts[index]);     // номер гравця
                    surname = parts[index + 1];           // прізвище
                    name = parts[index + 2];              // ім'я
                    team = parts[index + 3];              // команда
                    minute = std::stoi(parts[index + 4]); // хвилина
                } catch (const std::exception& e) {
                    throw std::runtime_error("Некоректний формат даних бомбардира " + 
                                        std::to_string(j+1) + " в матчі " + std::to_string(i+1) +
                                        " (номер: " + parts[index] + ", хвилина: " + parts[index + 4] + ")");
                }
                
                std::cout << "  Бомбардир " << (j+1) << ": " << number << " " << surname << " " 
                          << name << " (" << team << "), хвилина: " << minute << std::endl;
                
                Player player(number, surname, name);
                Goal goal(player, team, minute);
                match.addGoal(goal);
                
                totalGoals++;
            }
            
            matches.push_back(match);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Помилка при читанні файлу: " << e.what() << std::endl;
        matches.clear();
        totalMatches = 0;
        totalGoals = 0;
    }
    
    return matches;
}

// Функція-предикат для порівняння бомбардирів за кількістю голів (для сортування)
bool compareByGoals(const std::pair<Player, ScorerInfo>& a, const std::pair<Player, ScorerInfo>& b) {
    // Спочатку за кількістю голів (спадання)
    if (a.second.totalGoals != b.second.totalGoals) {
        return a.second.totalGoals > b.second.totalGoals; 
    }
    // Потім за прізвищем (зростання)
    if (a.first.surname != b.first.surname) {
        return a.first.surname < b.first.surname;
    }
    // Потім за ім'ям (зростання)
    return a.first.name < b.first.name;
}

// Функція для виведення горизонтальної лінії потрібної довжини
void printHorizontalLine(int length, char symbol = '-') {
    std::cout << std::string(length, symbol) << std::endl;
}

// Функція для форматування рахунку матчу
std::string formatMatchScore(const Match& match) {
    std::ostringstream oss;
    oss << match.team1 << " " << match.score1 << ":" << match.score2 << " " << match.team2;
    return oss.str();
}

// Головна функція програми
int main() {
    // Налаштування підтримки кирилиці в консолі
    setlocale(LC_ALL, "Ukrainian");
    
    // Ім'я вхідного файлу
    std::string filename = "matches.txt";
    int totalMatches = 0;
    int totalGoals = 0;
    
    // Перевіряємо, чи існує файл
    if (!fileExists(filename)) {
        std::cout << "Файл '" << filename << "' не знайдено в поточній директорії." << std::endl;
        std::cout << "Введіть шлях до файлу з даними: ";
        std::getline(std::cin, filename);
        
        // Друга спроба
        if (!fileExists(filename)) {
            std::cout << "Помилка: файл '" << filename << "' не знайдено." << std::endl;
            std::cout << "Програма завершує роботу." << std::endl;
            return 1;
        }
    }
    
    std::cout << "Початок читання файлу: " << filename << std::endl;
    
    // Читаємо дані з файлу
    std::vector<Match> matches = readMatchesFromFile(filename, totalMatches, totalGoals);
    
    // Якщо не вдалося прочитати дані або файл порожній
    if (matches.empty()) {
        std::cout << "Не вдалося прочитати дані з файлу або файл порожній." << std::endl;
        std::cout << "Програма завершує роботу." << std::endl;
        
        // Додаємо паузу перед виходом для діагностики
        std::cout << "Натисніть Enter для виходу...";
        std::cin.get();
        
        return 1;
    }
    
    std::cout << "Файл успішно прочитано. Знайдено матчів: " << matches.size() << std::endl;
    
    // Створюємо асоціативний контейнер для зберігання інформації про бомбардирів
    std::map<Player, ScorerInfo> scorers;
    
    // Обробляємо кожен матч та збираємо статистику
    for (int i = 0; i < matches.size(); ++i) {
        const Match& match = matches[i];
        
        // Обробляємо кожен гол у матчі
        for (const Goal& goal : match.goals) {
            // Додаємо або оновлюємо інформацію про бомбардира
            auto& info = scorers[goal.player];
            info.team = goal.team;
            info.matchIndices.insert(i);
            info.totalGoals++;
        }
    }
    
    // Перетворюємо map в vector для сортування
    std::vector<std::pair<Player, ScorerInfo>> sortedScorers(scorers.begin(), scorers.end());
    
    // Сортуємо бомбардирів за кількістю забитих голів (використовуємо предикат)
    std::sort(sortedScorers.begin(), sortedScorers.end(), compareByGoals);
    
    // Визначаємо довжину таблиці для красивого відображення
    const int TABLE_WIDTH = 115;
    
    // Заголовок результатів
    std::cout << "\n" << std::string(TABLE_WIDTH / 2 - 15, '=') << " СТАТИСТИКА ФУТБОЛЬНИХ МАТЧІВ " << std::string(TABLE_WIDTH / 2 - 15, '=') << "\n" << std::endl;
    std::cout << "Кількість проведених зустрічей: " << totalMatches << std::endl;
    std::cout << "Загальна кількість забитих м'ячів: " << totalGoals << std::endl << std::endl;
    
    // Виводимо інформацію про всі матчі
    std::cout << std::string(TABLE_WIDTH / 2 - 8, '=') << " РЕЗУЛЬТАТИ МАТЧІВ " << std::string(TABLE_WIDTH / 2 - 8, '=') << std::endl;
    
    std::cout << std::left
              << std::setw(5) << "№"
              << std::setw(50) << "Рахунок"
              << std::setw(20) << "Кількість голів"
              << "Бомбардири" << std::endl;
    
    printHorizontalLine(TABLE_WIDTH, '-');
    
    for (int i = 0; i < matches.size(); ++i) {
        const Match& match = matches[i];
        
        std::cout << std::left
                  << std::setw(5) << (i + 1)
                  << std::setw(50) << formatMatchScore(match)
                  << std::setw(20) << match.goals.size();
        
        // Виводимо бомбардирів цього матчу
        bool first = true;
        for (const auto& goal : match.goals) {
            if (!first) {
                std::cout << ", ";
            }
            std::cout << goal.player.surname << " " << goal.player.name 
                      << " (" << goal.minute << "')";
            first = false;
        }
        
        std::cout << std::endl;
    }
    
    printHorizontalLine(TABLE_WIDTH, '=');
    
    // Виводимо інформацію про бомбардирів
    std::cout << "\n" << std::string(TABLE_WIDTH / 2 - 9, '=') << " НАЙКРАЩІ БОМБАРДИРИ " << std::string(TABLE_WIDTH / 2 - 9, '=') << std::endl;
    
    std::cout << std::left
              << std::setw(5) << "№"
              << std::setw(20) << "Країна"
              << std::setw(18) << "Прізвище"
              << std::setw(15) << "Ім'я"
              << std::setw(8) << "Номер"
              << std::setw(10) << "Голів"
              << "Матчі" << std::endl;
    
    printHorizontalLine(TABLE_WIDTH, '-');
    
    // Виводимо інформацію про кожного бомбардира
    int rank = 1;
    for (const auto& scorer : sortedScorers) {
        const Player& player = scorer.first;
        const ScorerInfo& info = scorer.second;
        
        std::cout << std::left
                  << std::setw(5) << rank++
                  << std::setw(20) << info.team
                  << std::setw(18) << player.surname
                  << std::setw(15) << player.name
                  << std::setw(8) << player.number
                  << std::setw(10) << info.totalGoals;
        
        // Вивід матчів
        bool first = true;
        for (int matchIndex : info.matchIndices) {
            if (!first) {
                std::cout << ", ";
            }
            std::cout << "[" << (matchIndex + 1) << "]";
            first = false;
        }
        
        std::cout << std::endl;
    }
    
    printHorizontalLine(TABLE_WIDTH, '=');
    
    // Виводимо статистику по командах
    std::cout << "\n" << std::string(TABLE_WIDTH / 2 - 10, '=') << " СТАТИСТИКА ПО КОМАНДАХ " << std::string(TABLE_WIDTH / 2 - 10, '=') << std::endl;
    
    // Збираємо інформацію про команди
    std::map<std::string, int> teamGoals;
    for (const Match& match : matches) {
        for (const Goal& goal : match.goals) {
            teamGoals[goal.team]++;
        }
    }
    
    // Перетворюємо map в vector для сортування
    std::vector<std::pair<std::string, int>> sortedTeams(teamGoals.begin(), teamGoals.end());
    
    // Сортуємо команди за кількістю забитих голів (спадання)
    std::sort(sortedTeams.begin(), sortedTeams.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::cout << std::left
              << std::setw(5) << "№"
              << std::setw(30) << "Країна"
              << "Кількість голів" << std::endl;
    
    printHorizontalLine(TABLE_WIDTH, '-');
    
    // Виводимо інформацію про кожну команду
    rank = 1;
    for (const auto& team : sortedTeams) {
        std::cout << std::left
                  << std::setw(5) << rank++
                  << std::setw(30) << team.first
                  << team.second << std::endl;
    }
    
    printHorizontalLine(TABLE_WIDTH, '=');
    
    std::cout << "\nДякуємо за використання програми! Натисніть Enter для виходу...";
    std::cin.get();
    
    return 0;
}