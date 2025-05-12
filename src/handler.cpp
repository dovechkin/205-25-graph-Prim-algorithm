#include "handler.hpp"

#include <json.hpp>

#include "graph.hpp"

using namespace httplib;
using namespace std;
using json = nlohmann::json;

namespace app {

void Handler::register_routes(Server& server)
{
  server.Post("/api/build_mst", build_mst_handler);
}

/**
 * @brief Обработчик POST-запроса для построения минимального остовного дерева
 * @param req Входящий HTTP-запрос с параметрами:
 *   - n_vertex: количество вершин (целое число > 1)
 *   - edges: массив рёбер [{"u": int, "v": int, "w": number}]
 *   - start: (опционально) стартовая вершина (по умолчанию 1)
 * @param res HTTP-ответ, содержащий:
 *   - edges: массив рёбер MST в формате [{"u": int, "v": int, "w": number}]
 * @note Возможные коды ошибок:
 *   - 410: неверный размер графа
 *   - 411: не переданы рёбра
 *   - 412: некорректные рёбра
 *   - 413: неверная стартовая вершина
 *   - 414: ошибка формата JSON
 */
void Handler::build_mst_handler(const Request& req, Response& res)
{
  try {
    json request = json::parse(req.body);

    // Количество вершин графа
    if (!request.contains("n_vertex") || !request["n_vertex"].is_number() ||
        request["n_vertex"] <= 1) {
      res.status = 410;
      res.set_content(R"({"Error": "Не верно передан размер графа!"})",
                      "application/json");
      return;
    }
    int n_vertex = request["n_vertex"];

    // Проверка ребер
    if (!request.contains("edges") || !request["edges"].is_array() ||
        request["edges"].empty()) {
      res.status = 411;
      res.set_content(R"({"Error": "Не переданы ребра графа!"})",
                      "application/json");
      return;
    }
    bool is_float_weights = false;
    bool is_correct_edges = true;
    for (const auto& e : request["edges"]) {
      is_correct_edges &= e.contains("u") && e["u"].is_number_integer() &&
                          e["u"] >= 1 && e["u"] <= n_vertex;
      is_correct_edges &= e.contains("v") && e["v"].is_number_integer() &&
                          e["v"] >= 1 && e["v"] <= n_vertex;
      is_correct_edges &= e["v"] != e["u"];
      is_correct_edges &= e.contains("w") && e["w"].is_number() && e["w"] > 0;

      if (!is_correct_edges) {
        break;
      }

      is_float_weights |= e["w"].is_number_float();
    }
    if (!is_correct_edges) {
      res.status = 412;
      res.set_content(R"({"Error": "Не верно переданы ребра графа!"})",
                      "application/json");
      return;
    }

    // Проверка стартовой вершины
    if (request.contains("start") &&
        (request["start"] < 1 || request["start"] > n_vertex)) {
      res.status = 413;
      res.set_content(
          R"({"Error": "Не верно передана стартовая вершина графа!"})",
          "application/json");
      return;
    }
    int start = request.value("start", 1);

    json body;
    json mst_array = json::array();

    if (is_float_weights) {
      vector<Edge<double>> edges;
      for (const auto& edge : request["edges"]) {
        edges.emplace_back(edge["u"].get<int>(), edge["v"].get<int>(),
                           edge["w"].get<double>());
      }

      auto result = algorithm_prima<double>(n_vertex, edges, start);

      for (const auto& edge : result) {
        json edge_obj;
        edge_obj["u"] = edge.u;
        edge_obj["v"] = edge.v;
        edge_obj["w"] = edge.weight;
        mst_array.push_back(edge_obj);
      }
    }
    else {
      vector<Edge<int>> edges;
      for (const auto& edge : request["edges"]) {
        edges.emplace_back(edge["u"].get<int>(), edge["v"].get<int>(),
                           edge["w"].get<int>());
      }

      auto result = algorithm_prima<int>(n_vertex, edges, start);

      for (const auto& edge : result) {
        json edge_obj;
        edge_obj["u"] = edge.u;
        edge_obj["v"] = edge.v;
        edge_obj["w"] = edge.weight;
        mst_array.push_back(edge_obj);
      }
    }

    body["edges"] = mst_array;
    res.status = 200;
    res.set_content(body.dump(), "application/json");
  }
  catch (const json::exception& e) {
    res.status = 414;
    res.set_content(R"({"Message: ", "Ошибка в формате данных!"})",
                    "application/json");
  }
}

}  // namespace app