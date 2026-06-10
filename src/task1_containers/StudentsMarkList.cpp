#include "StudentsMarkList.hpp"

#include <algorithm>
#include <istream>
#include <iterator>
#include <numeric>

std::unique_ptr<StudentsMarkList> StudentsMarkList::createFromStream(
    std::istream& input) {
  auto markList = std::unique_ptr<StudentsMarkList>(new StudentsMarkList());
  std::string line;

  std::string name;
  int mark;

  while (input >> name >> mark) {
    // Here can be added a check via "find" to ensure the student's name is not
    // duplicated. By default, the mark is overwritten.
    // Also a mark validation can be added.
    markList->studentMarks[name] = mark;
  }

  return markList;
}

std::optional<StudentsMarkList::Student> StudentsMarkList::findBestStudent()
    const {
  return studentMarks.empty() ? std::nullopt
                              : std::optional(*std::max_element(
                                    studentMarks.begin(), studentMarks.end(),
                                    [](const auto& a, const auto& b) {
                                      return a.second < b.second;
                                    }));
}

double StudentsMarkList::getAvgMark() {
  if (studentMarks.empty()) {
    return 0;
  }

  // TODO an overflow check
  long sum = std::accumulate(
      studentMarks.begin(), studentMarks.end(), 0,
      [](long acc, const auto& student) { return acc + student.second; });
  return static_cast<double>(sum) / studentMarks.size();
}

void StudentsMarkList::printStudents(std::ostream& out) const {
  for (const auto& student : studentMarks) {
    out << student.first << " " << student.second << "; ";
  }
}
