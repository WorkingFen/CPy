#include "pch.h"
#include "ReadingTest.h"
#include <tuple>
#include <mutex>

class ReadingTestFixture: public ::testing::Test, ReadingTest {
public:
    void SetUp() {
        read = new ReadingTest();
    }
    void Teardown() {
        read = nullptr;
        delete read;
    }
protected:
    ReadingTest* read{};
};

class ReadingTestFixtureParametrizedStr:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchStr, ReadingTestFixtureParametrizedStr,
    ::testing::Values(
        std::make_tuple("L\"Test\"", "L\"Test\"", true),
        std::make_tuple("\"Test\"", "\"Test\"", true),
        std::make_tuple("L\"Test string\"", "L\"Test string\"", true),
        std::make_tuple("\"Test string\"", "\"Test string\"", true),
        std::make_tuple("L\"Test\")", "L\"Test\"", true),
        std::make_tuple("\"Test\")", "\"Test\"", true),
        std::make_tuple("L\"Test string\")", "L\"Test string\"", true),
        std::make_tuple("\"Test string\")", "\"Test string\"", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedStr, MatchStr) {
    std::string match{};
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_str(line, match);

    EXPECT_EQ(match, std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class ReadingTestFixtureParametrizedCon:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchCon, ReadingTestFixtureParametrizedCon,
    ::testing::Values(
        std::make_tuple("L''", "L''", true),
        std::make_tuple("''", "''", true),
        std::make_tuple("L'A'", "L'A'", true),
        std::make_tuple("L'a'", "L'a'", true),
        std::make_tuple("'A'", "'A'", true),
        std::make_tuple("'a'", "'a'", true),
        std::make_tuple("L'\t'", "L'\t'", true),
        std::make_tuple("'\t'", "'\t'", true),
        std::make_tuple("L' '", "L' '", true),
        std::make_tuple("' '", "' '", true),
        std::make_tuple("L'\n'", "L'\n'", true),
        std::make_tuple("'\n'", "'\n'", true),
        std::make_tuple("L'A')", "L'A'", true),
        std::make_tuple("'A')", "'A'", true),
        std::make_tuple("L'A')==", "L'A'", true),
        std::make_tuple("'A')==", "'A'", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedCon, MatchCon) {
    std::string match{};
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_con(line, match);

    EXPECT_EQ(match, std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class ReadingTestFixtureParametrizedHex:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchHex, ReadingTestFixtureParametrizedHex,
    ::testing::Values(
        std::make_tuple("0xDEAD", "0xDEAD", true),
        std::make_tuple("0xdead", "0xdead", true),
        std::make_tuple("0XDEAD", "0XDEAD", true),
        std::make_tuple("0Xdead", "0Xdead", true),
        std::make_tuple("0xDeAd", "0xDeAd", true),
        std::make_tuple("0xFF", "0xFF", true),
        std::make_tuple("0x42", "0x42", true),
        std::make_tuple("0x0", "0x0", true),
        std::make_tuple("0xDEAD=", "0xDEAD", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedHex, MatchHex) {
    std::string match{};
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_hex(line, match);

    EXPECT_EQ(match, std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class ReadingTestFixtureParametrizedDbl:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchDbl, ReadingTestFixtureParametrizedDbl,
    ::testing::Values(
        std::make_tuple(".0", ".0", true),
        std::make_tuple("0.", "0.", true),
        std::make_tuple(".1", ".1", true),
        std::make_tuple("1.", "1.", true),
        std::make_tuple("1.0", "1.0", true),
        std::make_tuple(".0)", ".0", true),
        std::make_tuple("0.)", "0.", true),
        std::make_tuple("1.0)", "1.0", true),
        std::make_tuple("0.);", "0.", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedDbl, MatchDbl) {
    std::string match{};
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_dbl(line, match);

    EXPECT_EQ(match, std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class ReadingTestFixtureParametrizedOct:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchOct, ReadingTestFixtureParametrizedOct,
    ::testing::Values(
        std::make_tuple("00", "00", true),
        std::make_tuple("01", "01", true),
        std::make_tuple("00)", "00", true),
        std::make_tuple("04);", "04", true),
        std::make_tuple("04==01", "04", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedOct, MatchOct) {
    std::string match{};
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_oct(line, match);

    EXPECT_EQ(match, std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class ReadingTestFixtureParametrizedDec:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchDec, ReadingTestFixtureParametrizedDec,
    ::testing::Values(
        std::make_tuple("0", "0", true),
        std::make_tuple("1", "1", true),
        std::make_tuple("42", "42", true),
        std::make_tuple("0)", "0", true),
        std::make_tuple("0;", "0", true),
        std::make_tuple("9999);", "9999", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedDec, MatchDec) {
    std::string match{};
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_dec(line, match);

    EXPECT_EQ(match, std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class ReadingTestFixtureParametrizedLiterals:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchLiterals, ReadingTestFixtureParametrizedLiterals,
    ::testing::Values(
        std::make_tuple("test", "test", true),
        std::make_tuple("_test", "_test", true),
        std::make_tuple("test_", "test_", true),
        std::make_tuple("_test_", "_test_", true),
        std::make_tuple("test_test", "test_test", true),
        std::make_tuple("test1", "test1", true),
        std::make_tuple("_test1", "_test1", true),
        std::make_tuple("test1_", "test1_", true),
        std::make_tuple("_1test", "_1test", true),
        std::make_tuple("_1_test", "_1_test", true),
        std::make_tuple("test(", "test", true),
        std::make_tuple("_test(", "_test", true),
        std::make_tuple("test_(", "test_", true),
        std::make_tuple("_test_(", "_test_", true),
        std::make_tuple("test_test(", "test_test", true),
        std::make_tuple("test1(", "test1", true),
        std::make_tuple("_test1(", "_test1", true),
        std::make_tuple("test1_(", "test1_", true),
        std::make_tuple("test_1_(", "test_1_", true),
        std::make_tuple("_1test(", "_1test", true),
        std::make_tuple("_1test(int", "_1test", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedLiterals, MatchLiterals) {
    std::string match{};
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_literals(line, match);

    EXPECT_EQ(match, std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class ReadingTestFixtureParametrizedLineComm:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchLineComm, ReadingTestFixtureParametrizedLineComm,
    ::testing::Values(
        std::make_tuple("// Comment", true),
        std::make_tuple("//Comment", true),
        std::make_tuple("// More comments", true),
        std::make_tuple("//More comments", true),
        std::make_tuple("//More comments ", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedLineComm, MatchLineComm) {
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_line_comm(line);

    EXPECT_EQ(correct, std::get<1>(GetParam()));
}

class ReadingTestFixtureParametrizedBlockComm:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchBlockComm, ReadingTestFixtureParametrizedBlockComm,
    ::testing::Values(
        std::make_tuple("/*Comment*/", true),
        std::make_tuple("/* Comment */", true),
        std::make_tuple("/* Comment*/", true),
        std::make_tuple("/*Comment */", true),
        std::make_tuple("/* More comments */", true),
        std::make_tuple("/*More comments*/", true),
        std::make_tuple("/*More comments*/test", true),
        std::make_tuple("/*More comments*/(", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedBlockComm, MatchBlockComm) {
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_block_comm(line);

    EXPECT_EQ(correct, std::get<1>(GetParam()));
}

class ReadingTestFixtureParametrizedGraphic:
    public ReadingTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchGraphic, ReadingTestFixtureParametrizedGraphic,
    ::testing::Values(
        std::make_tuple(")", ")", true),
        std::make_tuple("(", "(", true),
        std::make_tuple("}", "}", true),
        std::make_tuple("{", "{", true),
        std::make_tuple("]", "]", true),
        std::make_tuple("[", "[", true),
        std::make_tuple("++", "++", true),
        std::make_tuple("--", "--", true),
        std::make_tuple("<=", "<=", true),
        std::make_tuple(">=", ">=", true),
        std::make_tuple("==", "==", true),
        std::make_tuple("!=", "!=", true),
        std::make_tuple("&&", "&&", true),
        std::make_tuple("||", "||", true),
        std::make_tuple("*=", "*=", true),
        std::make_tuple("/=", "/=", true),
        std::make_tuple("%=", "%=", true),
        std::make_tuple("+=", "+=", true),
        std::make_tuple("-=", "-=", true),
        std::make_tuple(",", ",", true),
        std::make_tuple("+", "+", true),
        std::make_tuple("-", "-", true),
        std::make_tuple("=", "=", true),
        std::make_tuple("<", "<", true),
        std::make_tuple(">", ">", true),
        std::make_tuple("!", "!", true),
        std::make_tuple(";", ";", true),
        std::make_tuple("%", "%", true),
        std::make_tuple("*", "*", true),
        std::make_tuple("/", "/", true),
        std::make_tuple(".", ".", true),
        std::make_tuple(":", ":", true),
        std::make_tuple("?", "?", true),
        std::make_tuple("()", "(", true),
        std::make_tuple("{}", "{", true),
        std::make_tuple("[]", "[", true),
        std::make_tuple("(())", "(", true),
        std::make_tuple("();", "(", true),
        std::make_tuple(");", ")", true),
        std::make_tuple("[];", "[", true),
        std::make_tuple("];", "]", true),
        std::make_tuple("{};", "{", true),
        std::make_tuple("};", "}", true),
        std::make_tuple("(int)", "(", true),
        std::make_tuple("(\"int\")", "(", true)
    ),
    );

TEST_P(ReadingTestFixtureParametrizedGraphic, MatchGraphic) {
    std::string match{};
    std::string line = std::get<0>(GetParam());
    bool correct = read->match_graphic(line, match);

    EXPECT_EQ(match, std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}
