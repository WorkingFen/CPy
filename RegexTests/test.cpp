#include "pch.h"
#include "RegexTest.h"
#include <tuple>
#include <mutex>

class RegexTestFixture : public ::testing::Test, RegexTest {
public:
	void SetUp() {
		regex = new RegexTest();
	}
	void Teardown() {
		regex = nullptr;
		delete regex;
	}
protected:
	RegexTest *regex{};
};

class RegexTestFixtureParametrized :
	public RegexTestFixture,
	public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>> 
{};

INSTANTIATE_TEST_CASE_P(RegexWord, RegexTestFixtureParametrized,
    ::testing::Values(
        std::make_tuple("int silnia", "int", " "),
        std::make_tuple("int silnia()", "int", " "),
        std::make_tuple("test\n", "test", "\n"),
        std::make_tuple("test\t", "test", "\t"),
        std::make_tuple("test ", "test", " "),
        std::make_tuple("test", "test", ""),
        std::make_tuple("\ntest", "", "\n"),
        std::make_tuple("\ttest ", "", "\t"),
        std::make_tuple(" test ", "", " ")
    ),
);

TEST_P(RegexTestFixtureParametrized, RegexWord) {
	std::string line = std::get<0>(GetParam());
	std::smatch sm = regex->regex_word(line);
	
	EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
	EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedStr :
	public RegexTestFixture,
	public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexStr, RegexTestFixtureParametrizedStr,
	::testing::Values(
		std::make_tuple("L\"Test\"", "L\"Test\"", ""),
        std::make_tuple("\"Test\"", "\"Test\"", ""),
        std::make_tuple("L\"Test string\"", "L\"Test string\"", ""),
        std::make_tuple("\"Test string\"", "\"Test string\"", ""),
        std::make_tuple("L\"Test\")", "L\"Test\"", ")"),
        std::make_tuple("\"Test\")", "\"Test\"", ")"),
        std::make_tuple("L\"Test string\")", "L\"Test string\"", ")"),
        std::make_tuple("\"Test string\")", "\"Test string\"", ")")
    ),
);

TEST_P(RegexTestFixtureParametrizedStr, RegexStr) {
	std::string line = std::get<0>(GetParam());
	std::smatch sm = regex->regex_str(line);

	EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
	EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedCon:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexCon, RegexTestFixtureParametrizedCon,
    ::testing::Values(
        std::make_tuple("L''", "L''", ""),
        std::make_tuple("''", "''", ""),
        std::make_tuple("L'A'", "L'A'", ""),
        std::make_tuple("L'a'", "L'a'", ""),
        std::make_tuple("'A'", "'A'", ""),
        std::make_tuple("'a'", "'a'", ""),
        std::make_tuple("L'\t'", "L'\t'", ""),
        std::make_tuple("'\t'", "'\t'", ""),
        std::make_tuple("L' '", "L' '", ""),
        std::make_tuple("' '", "' '", ""),
        std::make_tuple("L'\n'", "L'\n'", ""),
        std::make_tuple("'\n'", "'\n'", ""),
        std::make_tuple("L'A')", "L'A'", ")"),
        std::make_tuple("'A')", "'A'", ")"),
        std::make_tuple("L'A')==", "L'A'", ")=="),
        std::make_tuple("'A')==", "'A'", ")==")
    ),
);

TEST_P(RegexTestFixtureParametrizedCon, RegexCon) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_con(line);

    EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
    EXPECT_EQ(sm[3].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedHex:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexHex, RegexTestFixtureParametrizedHex,
    ::testing::Values(
        std::make_tuple("0xDEAD", "0xDEAD", ""),
        std::make_tuple("0xdead", "0xdead", ""),
        std::make_tuple("0XDEAD", "0XDEAD", ""),
        std::make_tuple("0Xdead", "0Xdead", ""),
        std::make_tuple("0xDeAd", "0xDeAd", ""),
        std::make_tuple("0xFF", "0xFF", ""),
        std::make_tuple("0x42", "0x42", ""),
        std::make_tuple("0x0", "0x0", ""),
        std::make_tuple("0xDEAD=", "0xDEAD", "=")
    ),
);

TEST_P(RegexTestFixtureParametrizedHex, RegexHex) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_hex(line);

    EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
    EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedDbl:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexDbl, RegexTestFixtureParametrizedDbl,
    ::testing::Values(
        std::make_tuple(".0", ".0", ""),
        std::make_tuple("0.", "0.", ""),
        std::make_tuple(".1", ".1", ""),
        std::make_tuple("1.", "1.", ""),
        std::make_tuple("1.0", "1.0", ""),
        std::make_tuple(".0)", ".0", ")"),
        std::make_tuple("0.)", "0.", ")"),
        std::make_tuple("1.0)", "1.0", ")"),
        std::make_tuple("0.);", "0.", ");")
    ),
);

TEST_P(RegexTestFixtureParametrizedDbl, RegexDbl) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_dbl(line);

    EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
    EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedOct:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexOct, RegexTestFixtureParametrizedOct,
    ::testing::Values(
        std::make_tuple("00", "00", ""),
        std::make_tuple("01", "01", ""),
        std::make_tuple("00)", "00", ")"),
        std::make_tuple("04);", "04", ");"),
        std::make_tuple("04==01", "04", "==01")
    ),
);

TEST_P(RegexTestFixtureParametrizedOct, RegexOct) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_oct(line);

    EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
    EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedDec:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexDec, RegexTestFixtureParametrizedDec,
    ::testing::Values(
        std::make_tuple("0", "0", ""),
        std::make_tuple("1", "1", ""),
        std::make_tuple("42", "42", ""),
        std::make_tuple("0)", "0", ")"),
        std::make_tuple("0;", "0", ";"),
        std::make_tuple("9999);", "9999", ");")
    ),
);

TEST_P(RegexTestFixtureParametrizedDec, RegexDec) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_dec(line);

    EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
    EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedLiterals:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexLiterals, RegexTestFixtureParametrizedLiterals,
    ::testing::Values(
        std::make_tuple("test", "test", ""),
        std::make_tuple("_test", "_test", ""),
        std::make_tuple("test_", "test_", ""),
        std::make_tuple("_test_", "_test_", ""),
        std::make_tuple("test_test", "test_test", ""),
        std::make_tuple("test1", "test1", ""),
        std::make_tuple("_test1", "_test1", ""),
        std::make_tuple("test1_", "test1_", ""),
        std::make_tuple("_1test", "_1test", ""),
        std::make_tuple("_1_test", "_1_test", ""),
        std::make_tuple("test(", "test", "("),
        std::make_tuple("_test(", "_test", "("),
        std::make_tuple("test_(", "test_", "("),
        std::make_tuple("_test_(", "_test_", "("),
        std::make_tuple("test_test(", "test_test", "("),
        std::make_tuple("test1(", "test1", "("),
        std::make_tuple("_test1(", "_test1", "("),
        std::make_tuple("test1_(", "test1_", "("),
        std::make_tuple("test_1_(", "test_1_", "("),
        std::make_tuple("_1test(", "_1test", "("),
        std::make_tuple("_1test(int", "_1test", "(int")
    ),
);

TEST_P(RegexTestFixtureParametrizedLiterals, RegexLiterals) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_literals(line);

    EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
    EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedLineComm:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexLineComm, RegexTestFixtureParametrizedLineComm,
    ::testing::Values(
        std::make_tuple("// Comment", "// Comment"),
        std::make_tuple("//Comment", "//Comment"),
        std::make_tuple("// More comments", "// More comments"),
        std::make_tuple("//More comments", "//More comments"),
        std::make_tuple("//More comments ", "//More comments ")
    ),
);

TEST_P(RegexTestFixtureParametrizedLineComm, RegexLineComm) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_line_comm(line);

    EXPECT_EQ(sm[0].str(), std::get<1>(GetParam()));
}

class RegexTestFixtureParametrizedBlockComm:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexBlockComm, RegexTestFixtureParametrizedBlockComm,
    ::testing::Values(
        std::make_tuple("/*Comment*/", "/*Comment*/", ""),
        std::make_tuple("/* Comment */", "/* Comment */", ""),
        std::make_tuple("/* Comment*/", "/* Comment*/", ""),
        std::make_tuple("/*Comment */", "/*Comment */", ""),
        std::make_tuple("/* More comments */", "/* More comments */", ""),
        std::make_tuple("/*More comments*/", "/*More comments*/", ""),
        std::make_tuple("/*More comments*/test", "/*More comments*/", "test"),
        std::make_tuple("/*More comments*/(", "/*More comments*/", "(")
    ),
);

TEST_P(RegexTestFixtureParametrizedBlockComm, RegexBlockComm) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_block_comm(line);

    EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
    EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}

class RegexTestFixtureParametrizedGraphic:
    public RegexTestFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>>
{};

INSTANTIATE_TEST_CASE_P(RegexGraphic, RegexTestFixtureParametrizedGraphic,
    ::testing::Values(
        std::make_tuple(")", ")", ""),
        std::make_tuple("(", "(", ""),
        std::make_tuple("}", "}", ""),
        std::make_tuple("{", "{", ""),
        std::make_tuple("]", "]", ""),
        std::make_tuple("[", "[", ""),
        std::make_tuple("++", "++", ""),
        std::make_tuple("--", "--", ""),
        std::make_tuple("<=", "<=", ""),
        std::make_tuple(">=", ">=", ""),
        std::make_tuple("==", "==", ""),
        std::make_tuple("!=", "!=", ""),
        std::make_tuple("&&", "&&", ""),
        std::make_tuple("||", "||", ""),
        std::make_tuple("*=", "*=", ""),
        std::make_tuple("/=", "/=", ""),
        std::make_tuple("%=", "%=", ""),
        std::make_tuple("+=", "+=", ""),
        std::make_tuple("-=", "-=", ""),
        std::make_tuple(",", ",", ""),
        std::make_tuple("+", "+", ""),
        std::make_tuple("-", "-", ""),
        std::make_tuple("=", "=", ""),
        std::make_tuple("<", "<", ""),
        std::make_tuple(">", ">", ""),
        std::make_tuple("!", "!", ""),
        std::make_tuple(";", ";", ""),
        std::make_tuple("%", "%", ""),
        std::make_tuple("*", "*", ""),
        std::make_tuple("/", "/", ""),
        std::make_tuple(".", ".", ""),
        std::make_tuple(":", ":", ""),
        std::make_tuple("?", "?", ""),
        std::make_tuple("()", "(", ")"),
        std::make_tuple("{}", "{", "}"),
        std::make_tuple("[]", "[", "]"),
        std::make_tuple("(())", "(", "())"),
        std::make_tuple("();", "(", ");"),
        std::make_tuple(");", ")", ";"),
        std::make_tuple("[];", "[", "];"),
        std::make_tuple("];", "]", ";"),
        std::make_tuple("{};", "{", "};"),
        std::make_tuple("};", "}", ";"),
        std::make_tuple("(int)", "(", "int)"),
        std::make_tuple("(\"int\")", "(", "\"int\")")
    ),
);

TEST_P(RegexTestFixtureParametrizedGraphic, RegexGraphic) {
    std::string line = std::get<0>(GetParam());
    std::smatch sm = regex->regex_graphic(line);

    EXPECT_EQ(sm[1].str(), std::get<1>(GetParam()));
    EXPECT_EQ(sm[2].str(), std::get<2>(GetParam()));
}
