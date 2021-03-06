#include <SmallestCircle/smallest_circle.h>
#include <string>
#include <iostream>
#include <SmallestCircle/common.h>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <cassert>

using namespace std;
using namespace smallest_circle;

// Ad-hoc test framework, TODO: Use GTest
#define AssertEqual(val, target)                                        \
  {                                                                     \
    int line = __LINE__;                                                \
    if (val != target) {                                                \
      throw "Assertion '" + to_string(val) + "==" + to_string(target) + \
          "' at line " + to_string(line) + " failed.\n";                \
    }                                                                   \
  }

#define AssertDoubleEqual(val, target)                                  \
  {                                                                     \
    int line = __LINE__;                                                \
    if (fabs(val - target) > Epsilon) {                                 \
      throw "Assertion '" + to_string(val) + "==" + to_string(target) + \
          "' at line " + to_string(line) + " failed.\n";                \
    }                                                                   \
  }

void TestCircle3Points() {
  Circle test1(Point{-3, 4}, Point{4, 5}, Point{1, -4});
  AssertDoubleEqual(test1.center.x, 1);
  AssertDoubleEqual(test1.center.y, 1);
  AssertDoubleEqual(test1.radius, 5);
}

// Test 3 points that form a triangle
void TestFindSmallestCircle1() {
  vector<Point> test_points = {{-3, 4}, {4, 5}, {1, -4}};
  auto result = SmallestCircle::FindSmallestCircle(test_points);
  AssertDoubleEqual(result.center.x, 1);
  AssertDoubleEqual(result.center.y, 1);
  AssertDoubleEqual(result.radius, 5);
}

// Test 3 points that form a line
void TestFindSmallestCircle2() {
  // Horizontal
  vector<Point> test_points = {{-1, 0}, {0, 0}, {1, 0}};
  auto result = SmallestCircle::FindSmallestCircle(test_points);
  AssertDoubleEqual(result.center.x, 0);
  AssertDoubleEqual(result.center.y, 0);
  AssertDoubleEqual(result.radius, 1);

  // Vertical
  test_points = {{0, -1}, {0, 0}, {0, 1}};
  result = SmallestCircle::FindSmallestCircle(test_points);
  AssertDoubleEqual(result.center.x, 0);
  AssertDoubleEqual(result.center.y, 0);
  AssertDoubleEqual(result.radius, 1);

  // 45 degree
  test_points = {{-1, -1}, {0, 0}, {1, 1}};
  result = SmallestCircle::FindSmallestCircle(test_points);
  AssertDoubleEqual(result.center.x, 0);
  AssertDoubleEqual(result.center.y, 0);
  AssertDoubleEqual(result.radius, 1.414213562);
}

// Test 2 points
void TestFindSmallestCircle3() {
  // Horizontal
  vector<Point> test_points = {{-1, 0}, {1, 0}};
  auto result = SmallestCircle::FindSmallestCircle(test_points);
  AssertDoubleEqual(result.center.x, 0);
  AssertDoubleEqual(result.center.y, 0);
  AssertDoubleEqual(result.radius, 1);

  // 45 degree
  test_points = {{1, 1}, {-1, -1}};
  result = SmallestCircle::FindSmallestCircle(test_points);
  AssertDoubleEqual(result.center.x, 0);
  AssertDoubleEqual(result.center.y, 0);
  AssertDoubleEqual(result.radius, 1.414213562);
}

// Test 1 point
void TestFindSmallestCircle4() {
  vector<Point> test_points = {{1, 1}};
  auto result = SmallestCircle::FindSmallestCircle(test_points);
  AssertDoubleEqual(result.center.x, 1);
  AssertDoubleEqual(result.center.y, 1);
  AssertDoubleEqual(result.radius, 0);

  test_points = {{1000, -1000}};
  result = SmallestCircle::FindSmallestCircle(test_points);
  AssertDoubleEqual(result.center.x, 1000);
  AssertDoubleEqual(result.center.y, -1000);
  AssertDoubleEqual(result.radius, 0);
}

void TestFindSmallestCircle5(const int range, const int test_nums) {
  assert(test_nums >= 3);
  auto RangeRand = [&]() { return (rand() % (range * 2) - range) * 1.0; };

  srand(0);  // Fixed value for debugging
  vector<Point> ground_truth_points = {{RangeRand(), RangeRand()},
                                       {RangeRand(), RangeRand()},
                                       {RangeRand(), RangeRand()}};
  Circle ground_truth_circle = Circle(
      ground_truth_points[0], ground_truth_points[1], ground_truth_points[2]);
  // Brute force method to generate ground truth smallest circle
  for (int i = 0; i < ground_truth_points.size(); i++) {
    for (int j = i + 1; j < ground_truth_points.size(); j++) {
      Circle temp = Circle(ground_truth_points[i], ground_truth_points[j]);
      bool enclose_all = true;
      for (auto& p : ground_truth_points) enclose_all &= temp.Encloses(p);
      if (enclose_all && temp.radius < ground_truth_circle.radius) {
        ground_truth_circle = temp;
      }
    }
  }

  auto GeneratePointWithinCircle = [](const Circle& circle) {
    double theta = rand() % 360 / 180.0 * M_PI;
    double radius = rand() % (static_cast<int>(circle.radius) * 100) / 100.0;
    return Point{circle.center.x + radius * cos(theta),
                 circle.center.y + radius * sin(theta)};
  };

  vector<Point> test_points(test_nums - 3);
  for (auto& p : test_points)
    p = GeneratePointWithinCircle(ground_truth_circle);
  test_points.insert(test_points.end(), ground_truth_points.begin(),
                     ground_truth_points.end());

  auto t1 = Now();
  auto result = SmallestCircle::FindSmallestCircle(test_points);
  cout << "Testing " << test_nums << " points took " << Now() - t1 << "s\n";

  cout << "Writing test data into the files...\n";
  ofstream out("ground_truth_points.data");
  for (auto& p : ground_truth_points) {
    out << p.x << "," << p.y << "\n";
  }
  out.close();

  out.open("ground_truth_circle.data");
  out << ground_truth_circle.center.x << "," << ground_truth_circle.center.y
      << "," << ground_truth_circle.radius << "\n";
  out.close();

  out.open("test_points.data");
  for (auto& p : test_points) {
    out << p.x << "," << p.y << "\n";
  }
  out.close();

  out.open("test_result.data");
  out << result.center.x << "," << result.center.y << "," << result.radius
      << "\n";
  out.close();

  AssertDoubleEqual(result.center.x, ground_truth_circle.center.x);
  AssertDoubleEqual(result.center.y, ground_truth_circle.center.y);
  AssertDoubleEqual(result.radius, ground_truth_circle.radius);
}

int main(int argc, char** argv) {
  try {
    TestCircle3Points();
    TestFindSmallestCircle1();
    TestFindSmallestCircle2();
    TestFindSmallestCircle3();
    TestFindSmallestCircle4();

    int test_range = 1000;
    int test_num = 500;
    if (argc == 3) {
      test_range = atoi(argv[1]);
      test_num = atoi(argv[2]);
    }
    TestFindSmallestCircle5(test_range, test_num);
    cout << "Passed all test cases!\n";
  }
  catch (std::string& e) {
    cout << e;
  }
  catch (...) {
    cout << "Unexpected exception\n";
  }
}
