#pragma once
#include <httplib.h>

#include "graph.hpp"

using namespace httplib;
using namespace std;

namespace app {

/**
 * @brief Класс для обработки HTTP-запросов
 */
class Handler {
 public:
  /**
   * @brief Регистрация маршрутов сервера
   * @param server Ссылка на сервер HTTP
   */
  static void register_routes(Server& server);

  /**
   * @brief Обработчик запроса для построения MST
   * @param req Входящий HTTP-запрос
   * @param res Исходящий HTTP-ответ
   */
  static void build_mst_handler(const Request& req, Response& res);
};

}  // namespace app