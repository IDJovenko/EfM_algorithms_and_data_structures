#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>

#include "StudentsMarkList.hpp"

#ifndef DATA_DIR
#define DATA_DIR "."
#endif

int main() {
  // 1. Читает список студентов из файла
  std::string studentsMarksFile = std::string(DATA_DIR) + "/students_marks.txt";
  std::ifstream file(studentsMarksFile);
  if (!file) {
    std::cerr << "Ошибка открытия файла " << studentsMarksFile << std::endl
              << "errno: " << errno << "("
              << std::system_category().message(errno) << ")" << std::endl;
  }

  auto marks = StudentsMarkList::createFromStream(file);

  // 2. Выводит студентов, отсортированных по имени
  std::cout << "Students sorted by name:" << std::endl;
  marks->printStudents(std::cout);
  std::cout << std::endl << std::endl;

  // 3. Находит студента с максимальной оценкой
  auto bestStudent = marks->findBestStudent();
  if (bestStudent.has_value()) {
    std::cout << "Best student: " << bestStudent->first << " (mark "
              << bestStudent->second << ")" << std::endl;
  } else {
    std::cout << "There is no students!" << std::endl;
  }
  std::cout << std::endl;

  // 4. Подсчитывает среднюю оценку
  std::cout << "Average mark: " << marks->getAvgMark() << std::endl;
  std::cout << std::endl;
  return 0;
}
