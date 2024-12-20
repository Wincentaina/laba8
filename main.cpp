#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

// Интерфейс для выполнения тестов
class ITestRunner {
public:
    virtual ~ITestRunner() = default;
    virtual bool executeTest(const string& input, const string& expected) const = 0;
};

// Простая реализация ITestRunner
class SimpleTestRunner : public ITestRunner {
public:
    bool executeTest(const string& input, const string& expected) const override {
        return input == expected;
    }
};

// Расширенная реализация ITestRunner
class AdvancedTestRunner : public ITestRunner {
private:
    int complexityLevel;

public:
    AdvancedTestRunner(int level) : complexityLevel(level) {}

    bool executeTest(const string& input, const string& expected) const override {
        cout << "Executing with complexity level: " << complexityLevel << endl;
        return input == expected && complexityLevel > 2;
    }
};

// Абстрактный базовый класс TestCaseBase
class TestCaseBase {
protected:
    string input;
    string expected;
    unique_ptr<ITestRunner> testRunner;

public:
    TestCaseBase(const string& input_str, const string& expected_str, unique_ptr<ITestRunner> runner)
        : input(input_str), expected(expected_str), testRunner(move(runner)) {}

    virtual ~TestCaseBase() = default;  // Виртуальный деструктор

    virtual bool runTest() const {
        return testRunner->executeTest(input, expected);
    }

    virtual TestCaseBase* clone() const = 0;

    const string& getInput() const {
        return input;
    }

    const string& getExpected() const {
        return expected;
    }
};

// Класс TestCase - производный от TestCaseBase
class TestCase : public TestCaseBase {
public:
    TestCase(const string& input_str, const string& expected_str, unique_ptr<ITestRunner> runner)
        : TestCaseBase(input_str, expected_str, move(runner)) {}

    TestCase* clone() const override {
        return new TestCase(input, expected, make_unique<SimpleTestRunner>());
    }
};

// Класс AdvancedTestCase - производный от TestCase
class AdvancedTestCase : public TestCase {
private:
    int complexityLevel;

public:
    AdvancedTestCase(const string& input_str, const string& expected_str, int level)
        : TestCase(input_str, expected_str, make_unique<AdvancedTestRunner>(level)), complexityLevel(level) {}

    bool runTest() const override {
        cout << "Running advanced test with complexity level: " << complexityLevel << endl;
        return TestCase::runTest();
    }

    AdvancedTestCase* clone() const override {
        return new AdvancedTestCase(input, expected, complexityLevel);
    }

    int getComplexityLevel() const {
        return complexityLevel;
    }
};

// Класс TestSuite
class TestSuite {
private:
    vector<shared_ptr<TestCaseBase>> tests;
    static int totalTestSuitesCreated;

public:
    TestSuite() {
        totalTestSuitesCreated++;
    }

    void addTest(shared_ptr<TestCaseBase> test) {
        tests.push_back(test);
    }

    const vector<shared_ptr<TestCaseBase>>& getTests() const {
        return tests;
    }

    int getTestCount() const {
        return tests.size();
    }

    static int getTotalTestSuitesCreated() {
        return totalTestSuitesCreated;
    }

    void sortTestsByInput() {
        sort(tests.begin(), tests.end(), [](const shared_ptr<TestCaseBase>& a, const shared_ptr<TestCaseBase>& b) {
            return a->getInput() < b->getInput();
        });
    }

    shared_ptr<TestCaseBase> findTestByExpected(const string& expected) const {
        auto it = find_if(tests.begin(), tests.end(), [&expected](const shared_ptr<TestCaseBase>& test) {
            return test->getExpected() == expected;
        });
        return (it != tests.end()) ? *it : nullptr;
    }
};

int TestSuite::totalTestSuitesCreated = 0;

// Класс Task
class Task {
private:
    string description;
    TestSuite testSuite;

public:
    Task(const string& desc, const TestSuite& suite)
        : description(desc), testSuite(suite) {}

    const string& getDescription() const {
        return description;
    }

    TestSuite& getTestSuite() {
        return testSuite;
    }
};

// Главная функция
int main() {
    auto test1 = make_shared<TestCase>("input3", "expected3", make_unique<SimpleTestRunner>());
    auto test2 = make_shared<AdvancedTestCase>("input1", "expected1", 5);
    auto test3 = make_shared<AdvancedTestCase>("input2", "expected2", 4);

    TestSuite suite;
    suite.addTest(test1);
    suite.addTest(test2);
    suite.addTest(test3);

    cout << "Before sorting tests:" << endl;
    for (const auto& test : suite.getTests()) {
        cout << "Test Input: " << test->getInput() << endl;
    }

    suite.sortTestsByInput();

    cout << "\nAfter sorting tests:" << endl;
    for (const auto& test : suite.getTests()) {
        cout << "Test Input: " << test->getInput() << endl;
    }

    cout << "\nSearching for test with expected output 'expected2':" << endl;
    auto foundTest = suite.findTestByExpected("expected2");
    if (foundTest) {
        cout << "Found test with input: " << foundTest->getInput() << endl;
    } else {
        cout << "Test not found." << endl;
    }

    cout << "\nTotal Test Suites Created: " << TestSuite::getTotalTestSuitesCreated() << endl;

    return 0;
}
