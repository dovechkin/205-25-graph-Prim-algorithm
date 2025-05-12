#pragma once
#include <queue>
#include <vector>

using namespace std;

namespace app {

/**
 * @brief Ребро графа с весом
 * @tparam T Тип веса ребра (int, double и т.д.)
 */
template <typename T>
struct Edge {
  int u;     ///< Начальная вершина ребра
  int v;     ///< Конечная вершина ребра
  T weight;  ///< Вес ребра

  /**
   * @brief Конструктор ребра
   * @param u Начальная вершина
   * @param v Конечная вершина
   * @param w Вес ребра
   */
  Edge(int u, int v, T w) : u(u), v(v), weight(w) {}
};

/**
 * @brief Алгоритм Прима для поиска минимального остовного дерева
 * @tparam T Тип весов рёбер (int, double и т.д.)
 * @param n_vertex Количество вершин в графе
 * @param edges Список рёбер графа
 * @param start Стартовая вершина (по умолчанию 1)
 * @return Вектор рёбер минимального остовного дерева
 * @note Вершины нумеруются с 1
 */
template <typename T>
vector<Edge<T>> algorithm_prima(int n_vertex, const vector<Edge<T>>& edges,
                                int start = 1)
{
  const T INF = numeric_limits<T>::max();

  vector<vector<pair<int, T>>> adj(n_vertex, vector<pair<int, T>>());

  for (auto [u, v, w] : edges) {
    --u;
    --v;

    adj[u].emplace_back(v, w);
    adj[v].emplace_back(u, w);
  }
  --start;

  vector<bool> visited(n_vertex, false);
  vector<pair<T, int>> keys(n_vertex, {INF, -1});

  priority_queue<pair<T, int>, vector<pair<T, int>>, greater<pair<T, int>>> q;
  for (int i = 0; i < n_vertex; ++i) {
    q.push({INF, i});
  }

  keys[start] = {0, -1};
  q.push({0, start});

  while (!q.empty()) {
    auto [key, u] = q.top();
    if (visited[u] || keys[u].first < key) {
      q.pop();
      continue;
    }

    visited[u] = true;
    for (auto [v, w] : adj[u]) {
      if (!visited[v] && w < keys[v].first) {
        keys[v] = {w, u};
        q.push({w, v});
      }
    }
  }

  vector<Edge<T>> result;
  result.reserve(n_vertex - 1);
  for (int u = 0; u < n_vertex; ++u) {
    auto [w, v] = keys[u];
    if (v == -1) {
      continue;
    }

    result.emplace_back(u + 1, v + 1, w);
  }

  return result;
}

}  // namespace app