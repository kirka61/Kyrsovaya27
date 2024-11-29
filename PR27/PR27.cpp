#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Структура для зберігання інформації про автомобіль
struct Car {
    int id;
    string firm, model, type, plateNumber, fuelType;
};

// Структура для зберігання інформації про АЗС
struct GasStation {
    int id;
    string name, address;
    vector<string> fuelTypes;
    bool isOperational;
    int carsServed = 0; // Кількість обслужених транспортних засобів
};

// Структура для зберігання інформації про депо
struct AutoBase {
    int id;
    string address;
    int cars = 0; // Установка початкового значення
};

// Функція завантаження та відображення даних про транспортний засіб
vector<Car> loadCars(const string& filename) {
    vector<Car> cars;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return cars;
    }

    string line;
    cout << "Car Info:\n";
    while (getline(file, line)) {
        cout << line << endl;  // Відображення вмісту файлу
        stringstream ss(line);
        Car car;
        ss >> car.id >> car.firm >> car.model >> car.type >> car.plateNumber >> car.fuelType;
        cars.push_back(car);
    }
    return cars;
}

// Функція завантаження та відображення даних про АЗС
vector<GasStation> loadGasStations(const string& filename) {
    vector<GasStation> stations;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return stations;
    }

    string line;
    cout << "\nAZS Info:\n";
    while (getline(file, line)) {
        cout << line << endl;  // Відображення вмісту файлу
        stringstream ss(line);
        GasStation station;
        int fuelTypeCount;
        ss >> station.id >> station.name >> station.address >> fuelTypeCount;
        station.fuelTypes.resize(fuelTypeCount);
        for (int i = 0; i < fuelTypeCount; ++i) {
            ss >> station.fuelTypes[i];
        }
        ss >> station.isOperational >> station.carsServed;
        stations.push_back(station);
    }
    return stations;
}

// Функція завантаження та відображення даних про вагонні депо
vector<AutoBase> loadAutoBases(const string& filename) {
    vector<AutoBase> bases;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return bases;
    }

    string line;
    cout << "\nAuto_Base Info:\n";
    while (getline(file, line)) {
        cout << line << endl;  // Відображення вмісту файлу
        stringstream ss(line);
        AutoBase base;
        ss >> base.id >> base.address >> base.cars; // Загрузить количество машин
        bases.push_back(base);
    }
    return bases;
}

// Автозаправні станції, які підходять для конкретного транспортного засобу
vector<GasStation> findStationsForCar(int car_Id, const vector<Car>& cars, const vector<GasStation>& stations) {
    vector<GasStation> suitableStations;

    // Пошук авто за ID
    auto it = find_if(cars.begin(), cars.end(), [car_Id](const Car& car) { return car.id == car_Id; });
    if (it == cars.end()) {
        cout << "Автомобиль с ID " << car_Id << " не найден." << endl;
        return suitableStations;
    }

    // Отримати тип палива автомобіля
    string carFuelType = it->fuelType;

    // Перевіряємо, які АЗС підтримують це паливо і працюють
    for (const auto& station : stations) {
        if (station.isOperational) {
            bool hasFuelType = any_of(station.fuelTypes.begin(), station.fuelTypes.end(),
                [&carFuelType](const string& fuel) { return fuel == carFuelType; });
            if (hasFuelType) {
                suitableStations.push_back(station);
            }
        }
    }

    return suitableStations;
}

// Скільки автомобілів з кузовом седан заправляються дизельним паливом
int countDieselSedans(const vector<Car>& cars) {
    int count = 0;
    for (const auto& car : cars) {
        if (car.type == "седан" && car.fuelType == "DP") {
            count++;
        }
    }
    return count;
}

// Список АЗС, на яких відсутній заданий сорт бензину
vector<GasStation> findGasStationsWithoutFuel(const vector<GasStation>& stations, const string& fuelType) {
    vector<GasStation> result;
    for (const auto& station : stations) {
        if (find(station.fuelTypes.begin(), station.fuelTypes.end(), fuelType) == station.fuelTypes.end()) {
            result.push_back(station);
        }
    }
    return result;
}

// АЗС, що обслуговує максимальну кількість автомобілів
void printMaxServedGasStation(const vector<GasStation>& stations) {
    if (stations.empty()) {
        cout << "Список АЗС пуст." << endl;
        return;
    }

    // Находим АЗС с максимальным значением carsServed
    const GasStation* maxServedStation = &stations[0];
    for (const auto& station : stations) {
        if (station.carsServed > maxServedStation->carsServed) {
            maxServedStation = &station;
        }
    }

    // Выводим результат
    cout << "АЗС с максимальним обслужинням:\n";
    cout << "ID: " << maxServedStation->id << " Назва: " << maxServedStation->name << " Обслужено машин: " << maxServedStation->carsServed << endl;
}

// Автомобілі, що заправляються сортом бензину
vector<Car> findCarsByFuelType(const vector<Car>& cars, const string& fuelType) {
    vector<Car> result;
    for (const auto& car : cars) {
        if (car.fuelType == fuelType) {
            result.push_back(car);
        }
    }
    return result;
}

// Автобаза з максимальною кількістю автомобілів
AutoBase findAutoBaseWithMaxCars(const vector<AutoBase>& bases) {
    return *max_element(bases.begin(), bases.end(), [](const AutoBase& a, const AutoBase& b) {
        return a.cars < b.cars;
        });
}

// Функція збереження даних про моторні депо
void saveAutoBases(const string& filename, const vector<AutoBase>& bases) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл для записи: " << filename << endl;
        return;
    }

    for (const auto& base : bases) {
        file << base.id << " " << base.address << " " << base.cars << endl; // Сохраняем количество машин
    }

    cout << "Дані об автобазах успішно збережені у файл: " << filename << endl;
}

int main() {
    int choice_a;
    int carId;

    vector<Car> cars = loadCars("Car.txt");
    vector<GasStation> stations = loadGasStations("AZS.txt");
    vector<AutoBase> bases = loadAutoBases("Auto_Base.txt");

    while (true) {
        cout << "\n0 | Вихід\n1 | На які АЗС можна заправити конкретний автомобіль\n2 | Скільки автомобілів з кузовом седан заправляються дизельним паливом\n3 | Список АЗС, на яких відсутній заданий сорт бензину\n4 | АЗС, що обслуговує максимальну кількість автомобілів\n5 | Автомобілі, що заправляються сортом бензину\n6 | Автобаза з максимальною кількістю автомобілів\n7 | Додати автомобілі в автобазу\n";
        cin >> choice_a;

        switch (choice_a) {
        case 0:
            return 0;

        case 1:
            // На які АЗС можна заправити конкретний автомобіль
            cout << "Введіть ID автомобіля: ";
            cin >> carId;
            {
                auto suitableStations = findStationsForCar(carId, cars, stations);
                if (!suitableStations.empty()) {
                    cout << "\nАЗС, підходить для заправки авто з ID " << carId << ":\n";
                    for (const auto& station : suitableStations) {
                        cout << station.name << " - " << station.address << endl;
                    }
                }
                else {
                    cout << "Підходящих АЗС не знайдено." << endl;
                }
            }
            break;

        case 2:
            // Скільки автомобілів з кузовом седан заправляються дизельним паливом
            cout << "\nКількість автомобілів в кузові седан з використанням DP: " << countDieselSedans(cars) << endl;
            break;

        case 3:
        {
            // Список АЗС, на яких відсутній заданий сорт бензину
            string fuelType;
            cout << "Введіть марку бензину для пошуку (A92, A95, A98, DP): ";
            cin >> fuelType;

            auto stationsWithoutFuel = findGasStationsWithoutFuel(stations, fuelType);
            cout << "\nАЗС, де немає " << fuelType << ":\n";
            for (const auto& station : stationsWithoutFuel) {
                cout << station.name << " - " << station.address << endl;
            }
        }
        break;

        case 4:
        {
            // АЗС, що обслуговує максимальну кількість автомобілів
            printMaxServedGasStation(stations);
        }
        break;

        case 5:
        {
            // Автомобілі, що заправляються сортом бензину
            string fuelType;
            cout << "Введіть марку бензину для пошуку (A92, A95, A98, DP): ";
            cin >> fuelType;

            auto carsByFuel = findCarsByFuelType(cars, fuelType);
            if (!carsByFuel.empty()) {
                cout << "\nТранспортні засоби, які заправляються паливом " << fuelType << ":\n";
                for (const auto& car : carsByFuel) {
                    cout << car.firm << " " << car.model << " - " << car.plateNumber << endl;
                }
            }
            else {
                cout << "Автомобилей, використовувающих" << fuelType << ", не знайдено." << endl;
            }
        }
        break;

        case 6:
        {
            // Автобаза з максимальною кількістю автомобілів
            auto maxCarBase = findAutoBaseWithMaxCars(bases);
            cout << "\nАвтобаза з максимальною кількістю автомобілів: " << maxCarBase.id << " - " << maxCarBase.address << endl;
        }
        break;

        case 7: {
            int baseId, change;
            cout << "Введіть ID автобази: ";
            cin >> baseId;
            cout << "Введіть кількість машин відремонтованих у авто базі: ";
            cin >> change;

            // Знайти автобазу по ID
            auto it = find_if(bases.begin(), bases.end(), [baseId](const AutoBase& base) { return base.id == baseId; });
            if (it != bases.end()) {
                it->cars += change; // Змінити кількість автомобилей
                cout << "Кількість автомобілів у автобазі " << it->id << " змінено. Поточна кількість: " << it->cars << endl;
                saveAutoBases("Auto_Base.txt", bases);
            }
            else {
                cout << "Автобаза з ID " << baseId << " не знайдена." << endl;
            }
            break;
        }

        default:
            cout << "Неправильний вибір. Будь ласка, виберіть ще раз." << endl;
        }
    }
}
