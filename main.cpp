#include <iostream>
#include <memory>
#include <string>
#include <vector>

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

    const TestSuite& getTestSuite() const {
        return testSuite;
    }
};

// Класс UserSolution
class UserSolution {
private:
    string solutionCode;

public:
    UserSolution(const string& code)
        : solutionCode(code) {}

    const string& getSolutionCode() const {
        return solutionCode;
    }
};

// Класс ExecutionResult
class ExecutionResult {
private:
    string actualOutput;
    bool isPassed;

public:
    ExecutionResult() : isPassed(false) {}

    void setActualOutput(const string& output) {
        actualOutput = output;
    }

    const string& getActualOutput() const {
        return actualOutput;
    }

    void setIsPassed(bool passed) {
        isPassed = passed;
    }

    bool getIsPassed() const {
        return isPassed;
    }
};

// Класс Submission
class Submission {
private:
    UserSolution solution;
    vector<ExecutionResult> results;
    int totalPassed;

public:
    Submission(const UserSolution& userSolution, int testCount)
        : solution(userSolution), totalPassed(0) {
        results.resize(testCount);
    }

    void setTotalPassed(int passed) {
        totalPassed = passed;
    }

    int getTotalPassed() const {
        return totalPassed;
    }

    vector<ExecutionResult>& getResults() {
        return results;
    }

    const UserSolution& getSolution() const {
        return solution;
    }
};

// Функция для выполнения теста
ExecutionResult runTestCase(UserSolution& solution, const TestCaseBase& test) {
    ExecutionResult result;
    result.setActualOutput(test.runTest() ? "Passed" : "Failed");
    result.setIsPassed(test.runTest());
    return result;
}

// Функция для проверки решения
Submission checkSolution(UserSolution& solution, Task& task) {
    Submission submission(solution, task.getTestSuite().getTestCount());
    int totalPassed = 0;
    for (size_t i = 0; i < task.getTestSuite().getTestCount(); ++i) {
        const TestCaseBase& test = *task.getTestSuite().getTests()[i];
        ExecutionResult result = runTestCase(solution, test);
        submission.getResults()[i] = result;
        if (result.getIsPassed()) {
            totalPassed++;
        }
    }
    submission.setTotalPassed(totalPassed);
    return submission;
}

// Главная функция
int main() {
    auto test1 = new TestCase("input1", "input1", make_unique<SimpleTestRunner>());
    auto test2 = new AdvancedTestCase("input2", "expected2", 3);

    TestSuite suite;
    suite.addTest(test1);
    suite.addTest(test2);

    Task task("Example Task", suite);

    UserSolution solution("user_solution_code");

    Submission submission = checkSolution(solution, task);

    cout << "Total tests passed: " << submission.getTotalPassed() << " out of " << suite.getTestCount() << endl;

    for (size_t i = 0; i < suite.getTestCount(); i++) {
        cout << "Test " << (i + 1) << ": " << (submission.getResults()[i].getIsPassed() ? "Passed" : "Failed") << endl;
    }

    cout << "Total Test Suites Created: " << TestSuite::getTotalTestSuitesCreated() << endl;

    return 0;
}
