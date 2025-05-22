#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

class Function {
private:
    vector<double> x_values;
    vector<double> y_values;

public:
    // Конструктор за замовчуванням
    Function() {}
    
    // Деструктор
    ~Function() {
        x_values.clear();
        y_values.clear();
    }

    // Функція вилучення (читання з файлу)
    void insert(const string& filename, int y_column) {
        ifstream infile(filename.c_str());
        if (!infile) {
            cerr << "Помилка відкриття файлу " << filename << endl;
            return;
        }

        double x, y1, y2;
        while (infile >> x >> y1 >> y2) {
            x_values.push_back(x);
            if (y_column == 1)
                y_values.push_back(y1);
            else
                y_values.push_back(y2);
        }
        infile.close();
        
        cout << "Успішно завантажено " << x_values.size() 
             << " точок з файлу " << filename << endl;
    }

    // Функція вставки (відображення на екрані)
    void display() const {
        cout << setw(12) << "X" << setw(12) << "Y" << endl;
        cout << setfill('-') << setw(24) << "" << setfill(' ') << endl;
        
        for (size_t i = 0; i < x_values.size(); ++i) {
            cout << setw(12) << fixed << setprecision(3) 
                 << x_values[i] << setw(12) << fixed 
                 << setprecision(3) << y_values[i] << endl;
        }
    }

    // Функція збереження з форматуванням згідно завдання
    void save(const string& filename) const {
        ofstream outfile(filename.c_str());
        if (!outfile) {
            cerr << "Помилка відкриття файлу для запису " << filename << endl;
            return;
        }

        for (size_t i = 0; i < x_values.size(); ++i) {
            // Встановлення формату: фіксована точка, 3 знаки після коми, ширина поля 10
            outfile << setw(10) << fixed << setprecision(3) 
                    << showpos << x_values[i] << "\t"
                    << setw(10) << fixed << setprecision(3) 
                    << showpos << y_values[i] << endl;
        }
        outfile.close();
        
        cout << "Дані збережено у файл " << filename 
             << " з відповідним форматуванням" << endl;
    }

    // Функція для відображення простого графіку
    void displayGraph() const {
        if (x_values.empty()) {
            cout << "Немає даних для відображення графіку" << endl;
            return;
        }

        // Знаходження мінімальних та максимальних значень для масштабування
        vector<double>::const_iterator min_it = min_element(y_values.begin(), y_values.end());
        vector<double>::const_iterator max_it = max_element(y_values.begin(), y_values.end());
        double min_y = *min_it;
        double max_y = *max_it;
        
        cout << "\nГрафік функції:" << endl;
        cout << setfill('-') << setw(50) << "" << setfill(' ') << endl;
        
        // Простий ASCII графік
        const int graph_height = 15;
        for (int row = graph_height; row >= 0; --row) {
            double y_level = min_y + (max_y - min_y) * row / graph_height;
            cout << setw(8) << fixed << setprecision(2) << y_level << " |";
            
            size_t max_points = (y_values.size() < 40) ? y_values.size() : 40;
            for (size_t i = 0; i < max_points; ++i) {
                double normalized_y = (y_values[i] - min_y) / (max_y - min_y) * graph_height;
                if (abs(normalized_y - row) < 0.5) {
                    cout << "*";
                } else {
                    cout << " ";
                }
            }
            cout << endl;
        }
        
        cout << setw(9) << " " << setfill('-') << setw(41) << "" << setfill(' ') << endl;
    }

    // Дружня функція для знаходження мін/макс значень
    friend void findMinMax(const Function& func1, const Function& func2);
    
    // Дружні функції для перевантаження операторів введення-виведення
    friend ostream& operator<<(ostream& stream, const Function& func);
    friend istream& operator>>(istream& stream, Function& func);
};

// Дружня функція для знаходження мінімальних та максимальних значень
void findMinMax(const Function& func1, const Function& func2) {
    if (func1.y_values.empty() || func2.y_values.empty()) {
        cout << "Одна з функцій не містить даних" << endl;
        return;
    }

    vector<double>::const_iterator minmax1_min = min_element(func1.y_values.begin(), func1.y_values.end());
    vector<double>::const_iterator minmax1_max = max_element(func1.y_values.begin(), func1.y_values.end());
    vector<double>::const_iterator minmax2_min = min_element(func2.y_values.begin(), func2.y_values.end());
    vector<double>::const_iterator minmax2_max = max_element(func2.y_values.begin(), func2.y_values.end());

    cout << "\n=== АНАЛІЗ ФУНКЦІЙ ===" << endl;
    cout << "Функція 1: мінімальне значення = " << fixed << setprecision(3)
         << *minmax1_min << ", максимальне значення = " << *minmax1_max << endl;
    cout << "Функція 2: мінімальне значення = " << fixed << setprecision(3)
         << *minmax2_min << ", максимальне значення = " << *minmax2_max << endl;
              
    // Порівняння функцій
    double global_min = min(*minmax1_min, *minmax2_min);
    double global_max = max(*minmax1_max, *minmax2_max);
    
    cout << "Глобальний мінімум серед обох функцій: " << global_min << endl;
    cout << "Глобальний максимум серед обох функцій: " << global_max << endl;
}

// Перевантаження оператора виведення
ostream& operator<<(ostream& stream, const Function& func) {
    stream << "Функція містить " << func.x_values.size() << " точок:" << endl;
    stream << setw(12) << "X" << setw(12) << "Y" << endl;
    stream << setfill('-') << setw(24) << "" << setfill(' ') << endl;
    
    for (size_t i = 0; i < func.x_values.size(); ++i) {
        stream << setw(12) << fixed << setprecision(3) 
               << func.x_values[i] << setw(12) << fixed 
               << setprecision(3) << func.y_values[i] << endl;
    }
    return stream;
}

// Перевантаження оператора введення
istream& operator>>(istream& stream, Function& func) {
    cout << "Введіть кількість точок: ";
    int n;
    stream >> n;
    
    func.x_values.clear();
    func.y_values.clear();
    
    cout << "Введіть " << n << " пар значень (x y):" << endl;
    for (int i = 0; i < n; ++i) {
        double x, y;
        cout << "Точка " << (i+1) << ": ";
        stream >> x >> y;
        func.x_values.push_back(x);
        func.y_values.push_back(y);
    }
    
    return stream;
}

// Власні маніпулятори
ostream& separator(ostream& stream) {
    stream << setfill('=') << setw(50) << "" << setfill(' ') << endl;
    return stream;
}

ostream& header(ostream& stream) {
    stream << "\n*** АНАЛІЗ МАТЕМАТИЧНИХ ФУНКЦІЙ ***" << endl;
    return stream;
}

int main() {
    cout << header << separator;
    
    Function func1, func2;
    string filename = "data.txt";

    // Завантаження даних з файлу
    cout << "\nЗавантаження даних з файлу..." << endl;
    func1.insert(filename, 1);  // Перша функція (другий стовпчик)
    func2.insert(filename, 2);  // Друга функція (третій стовпчик)

    cout << separator;

    // Відображення даних за допомогою перевантаженого оператора
    cout << "\nДані для функції 1:" << endl;
    cout << func1;
    
    cout << "\nДані для функції 2:" << endl;
    cout << func2;

    cout << separator;

    // Відображення графіків
    cout << "\nГрафік функції 1:" << endl;
    func1.displayGraph();
    
    cout << "\nГрафік функції 2:" << endl;
    func2.displayGraph();

    cout << separator;

    // Знаходження мінімальних та максимальних значень
    findMinMax(func1, func2);

    cout << separator;

    // Збереження даних у відформатованому вигляді
    cout << "\nЗбереження даних у файли..." << endl;
    func1.save("output1.txt");
    func2.save("output2.txt");

    cout << separator;

    // Демонстрація введення користувачем (опціонально)
    char choice;
    cout << "\nБажаєте ввести власну функцію? (y/n): ";
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        Function user_func;
        cin >> user_func;
        cout << "\nВаша функція:" << endl;
        cout << user_func;
        user_func.displayGraph();
    }

    cout << separator;
    cout << "Програма завершена успішно!" << endl;

    return 0;
}