#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

namespace {
std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec) {
  os << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    os << vec[i];
    if (i + 1 < vec.size()) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

constexpr bool isPrime(int n) {
  if (n <= 1) return false;
  if (n <= 3) return true;
  if (n % 2 == 0 || n % 3 == 0) return false;

  // We check divisors of the form 6k ± 1
  for (long i = 5; i * i <= n; i += 6) {
    if (n % i == 0 || n % (i + 2) == 0) return false;
  }
  return true;
}
}  // namespace

int main() {
  // 1. Генерирует вектор из 100 случайных чисел от 1 до 1000
  std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<int> dist(1, 1000);
  std::vector<int> v(100);
  std::generate(v.begin(), v.end(), [&gen, &dist]() { return dist(gen); });
  std::cout << "Сгенерированный вектор из 100 случайных чисел от 1 до 1000:"
            << std::endl
            << v << std::endl;
  std::cout << std::endl;

  // 2. Сортирует его по убыванию
  std::sort(v.begin(), v.end(), std::greater<int>());
  std::cout << "Отсортированный вектор:" << std::endl << v << std::endl;
  std::cout << std::endl;

  // 3. Находит все простые числа в векторе
  std::vector<int> primes;
  std::copy_if(v.begin(), v.end(), std::back_inserter(primes),
               [](int x) { return isPrime(x); });
  std::cout << "Простые числа из ветора:" << std::endl << primes << std::endl;
  std::cout << std::endl;

  // 4. Удаляет дубликаты
  // Note: для использования std::unique контейнер должен быть отсортирован
  auto it = std::unique(v.begin(), v.end());  // перемещаем дубликаты в конец
  v.erase(it, v.end());                       // удаляем дубликаты из вектора
  std::cout << "Вектор без дубликатов:" << std::endl << v << std::endl;
  std::cout << std::endl;

  // 5. Вычисляет сумму квадратов элементов
  long squared_norm = std::accumulate(
      v.begin(), v.end(), 0l, [](long acc, int x) { return acc + x * x; });
  std::cout << "Сумма квадратов элементов вектора: " << squared_norm
            << std::endl;
  std::cout << std::endl;

  return 0;
}