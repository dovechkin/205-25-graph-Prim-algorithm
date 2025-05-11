#include <httplib.h>

#include "handler.hpp"

/**
 * @brief Главная функция сервера
 * @details Запускает HTTP-сервер на порту 8080 и регистрирует маршруты API
 */
int main()
{
  httplib::Server server;

  // Регистрируем маршруты api-шки
  app::Handler::register_routes(server);

  std::cout << "Сервер запущен на http://localhost:8080 \n";
  server.listen("0.0.0.0", 8080);
}