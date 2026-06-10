#pragma once
#include <iosfwd>
#include <map>
#include <memory>
#include <optional>
#include <string>

// TODO Add a namespace!!!
class StudentsMarkList {
 public:
  using Student = std::pair<std::string, int>;

  static std::unique_ptr<StudentsMarkList> createFromStream(
      std::istream& input);

  // Displays students sorted by name.
  void printStudents(std::ostream& out) const;
  // Finds the student with the highest mark.
  std::optional<Student> findBestStudent() const;
  // Calculates the average rating.
  double getAvgMark();

 private:
  StudentsMarkList() = default;
  StudentsMarkList(const StudentsMarkList&) = default;
  StudentsMarkList(StudentsMarkList&&) = default;

  std::map<std::string, int> studentMarks;
};
