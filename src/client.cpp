#include <httplib.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <json.hpp>S
#include <queue>
#include <vector>

using namespace std;
using json = nlohmann::json;
using namespace httplib;
namespace fs = std::filesystem;

const string HOST = "localhost";  ///< Хост сервера по умолчанию
const int PORT = 8080;  ///< Порт сервера по умолчанию
const string TEST_INT_DIR =
    "data/integer";  ///< Директория с тестами для целых весов
const string TEST_FLOAT_DIR =
    "data/float";  ///< Директория с тестами для дробных весов

/**
 * @brief Тестирование одного графа
 * @tparam T Тип весов рёбер (int/double)
 * @param client HTTP-клиент
 * @param headers Заголовки запроса
 * @param file Входной файл с тестовыми данными
 * @note Формат файла:
 * 1 строка: n m s - кол-во вершин, рёбер, стартовая вершина
 * m строк: u v w - рёбра графа
 * Последняя строка: total_weight - ожидаемый суммарный вес MST
 */
template <typename T>
void test(Client& client, Headers& headers, ifstream& file)
{
  int n, m, s;
  file >> n >> m >> s;

  cout << "Тестируем граф с " << n << " вершинами и " << m
       << " ребрами (стартуем с вершины - " << s << "): \n";

  json request;
  request["n_vertex"] = n;
  request["start"] = s;

  json edges_array = json::array();
  for (int i = 0; i < m; ++i) {
    int u, v;
    T w;
    file >> u >> v >> w;

    cout << u << "-" << v << "(" << w << ") ";
    json edge;
    edge["u"] = u;
    edge["v"] = v;
    edge["w"] = w;
    edges_array.push_back(edge);
  }
  request["edges"] = edges_array;
  cout << '\n';

  T ttl_weight;
  file >> ttl_weight;

  auto response = client.Post("/api/build_mst", headers, request.dump(),
                              "application/json");

  if (response && response->status == 200) {
    json response_data = json::parse(response->body);

    cout << "Получили ребра дерева:\n";
    T test_ttl_weight = 0;
    for (const auto& edge : response_data["edges"]) {
      cout << edge["u"] << "-" << edge["v"] << "(" << edge["w"] << ") ";
      T w = edge["w"];

      test_ttl_weight += w;
    }
    cout << '\n';

    if (ttl_weight == test_ttl_weight) {
      cout << "Веса совпадают!\n";
    }
    else {
      cout << "Веса отличаются! Нужно " << ttl_weight << ", а получили "
           << test_ttl_weight << '\n';
    }
  }
  else {
    cerr << "Ошибка выполения запроса!\n";
    if (response) {
      cerr << "Статус: " << response->status << '\n';
    }
    else {
      cerr << "Ошибка: " << response.error() << '\n';
    }
  }
}

/**
 * @brief Тестирование набора графов из директории
 * @tparam T Тип весов рёбер (int/double)
 * @param client HTTP-клиент
 * @param headers Заголовки запроса
 * @param dir Путь к директории с тестами
 * @throws filesystem_error Ошибка доступа к директории
 * @throws exception Ошибка открытия файла
 */
template <typename T>
void test_set(Client& client, Headers& headers, const string& dir)
{
  for (const auto& entry : fs::directory_iterator(dir)) {
    if (entry.is_regular_file() && entry.path().extension() == ".txt") {
      std::ifstream file(entry.path());
      if (!file.is_open()) {
        cout << "Не удалось открыть файл: " << entry.path() << '\n';
        throw new std::exception();
      }

      test<T>(client, headers, file);

      file.close();
    }
  }
}

/**
 * @brief Главная функция клиента
 * @return 0 в случае успеха, 1 при ошибке
 * @details Выполняет:
 * 1. Тесты с целыми весами из TEST_INT_DIR
 * 2. Тесты с дробными весами из TEST_FLOAT_DIR
 */
int main()
{
  try {
    Client client(HOST, PORT);
    Headers headers = {{"Content-Type", "application/json"},
                       {"Accept", "application/json"}};

    // тесты с целыми весами
    test_set<int>(client, headers, TEST_INT_DIR);

    // тесты с дробными весами
    test_set<double>(client, headers, TEST_FLOAT_DIR);

    client.stop();
  }
  catch (const fs::filesystem_error& e) {
    std::cerr << "Ошибка доступа к директории: " << e.what() << std::endl;
    return 1;
  }
  catch (const json::parse_error& e) {
    cerr << "JSON parse error: " << e.what() << endl;
    return 1;
  }
  catch (const exception& ex) {
    cerr << ex.what() << endl;
    return 1;
  }
}