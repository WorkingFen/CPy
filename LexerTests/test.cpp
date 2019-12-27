#include "pch.h"
#include "Lexer.h"
#include <tuple>
#include <mutex>
#include <sstream>

class LexerFixture: public ::testing::Test, public Translator::Lexer<std::istringstream> {
public:
    void SetUp() {
    }
    void Teardown() {
        source = nullptr;
        delete source;
        lexer = nullptr;
        delete lexer;
    }
protected:
    Translator::Source<std::istringstream>* source{};
    Lexer* lexer{};
};

class LexerFixtureParametrizedStr:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchStr, LexerFixtureParametrizedStr,
    ::testing::Values(
        std::make_tuple("L\"Test\"", "L\"Test\"", true),
        std::make_tuple("\"Test\"", "\"Test\"", true),
        std::make_tuple("L\"Test string\"", "L\"Test string\"", true),
        std::make_tuple("\"Test string\"", "\"Test string\"", true),
        std::make_tuple("L\"Test\")", "L\"Test\"", true),
        std::make_tuple("\"Test\")", "\"Test\"", true),
        std::make_tuple("L\"Test string\")", "L\"Test string\"", true),
        std::make_tuple("\"Test string\")", "\"Test string\"", true),
        std::make_tuple("\"Test string\\\"\"", "\"Test string\\\"\"", true),
        std::make_tuple("\"Test string", "", false),
        std::make_tuple("\"Test string\\\\\"\"", "\"Test string\\\\\"", true)
    ),
);

TEST_P(LexerFixtureParametrizedStr, MatchStr) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_str();

    if(correct)
        EXPECT_EQ(lexer->get_match(), std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class LexerFixtureParametrizedCon:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchCon, LexerFixtureParametrizedCon,
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

TEST_P(LexerFixtureParametrizedCon, MatchCon) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_con();

    if(correct)
        EXPECT_EQ(lexer->get_match(), std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class LexerFixtureParametrizedHex:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchHex, LexerFixtureParametrizedHex,
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

TEST_P(LexerFixtureParametrizedHex, MatchHex) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_hex();

    if(correct)
        EXPECT_EQ(lexer->get_match(), std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class LexerFixtureParametrizedDbl:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchDbl, LexerFixtureParametrizedDbl,
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

TEST_P(LexerFixtureParametrizedDbl, MatchDbl) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_dbl();

    if(correct)
        EXPECT_EQ(lexer->get_match(), std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class LexerFixtureParametrizedOct:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchOct, LexerFixtureParametrizedOct,
    ::testing::Values(
        std::make_tuple("00", "00", true),
        std::make_tuple("01", "01", true),
        std::make_tuple("00)", "00", true),
        std::make_tuple("04);", "04", true),
        std::make_tuple("04==01", "04", true)
    ),
);

TEST_P(LexerFixtureParametrizedOct, MatchOct) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_oct();

    if(correct)
        EXPECT_EQ(lexer->get_match(), std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class LexerFixtureParametrizedDec:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchDec, LexerFixtureParametrizedDec,
    ::testing::Values(
        std::make_tuple("0", "0", true),
        std::make_tuple("1", "1", true),
        std::make_tuple("42", "42", true),
        std::make_tuple("0)", "0", true),
        std::make_tuple("0;", "0", true),
        std::make_tuple("9999);", "9999", true)
    ),
);

TEST_P(LexerFixtureParametrizedDec, MatchDec) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_dec();

    if(correct)
        EXPECT_EQ(lexer->get_match(), std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class LexerFixtureParametrizedLiterals:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchLiterals, LexerFixtureParametrizedLiterals,
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

TEST_P(LexerFixtureParametrizedLiterals, MatchLiterals) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_literals();

    if(correct)
        EXPECT_EQ(lexer->get_match(), std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}

class LexerFixtureParametrizedLineComm:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchLineComm, LexerFixtureParametrizedLineComm,
    ::testing::Values(
        std::make_tuple("// Comment", true),
        std::make_tuple("//Comment", true),
        std::make_tuple("// More comments", true),
        std::make_tuple("//More comments", true),
        std::make_tuple("//More comments ", true)
    ),
);

TEST_P(LexerFixtureParametrizedLineComm, MatchLineComm) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_line_comm();

    EXPECT_EQ(correct, std::get<1>(GetParam()));
}

class LexerFixtureParametrizedBlockComm:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchBlockComm, LexerFixtureParametrizedBlockComm,
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

TEST_P(LexerFixtureParametrizedBlockComm, MatchBlockComm) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_block_comm();

    EXPECT_EQ(correct, std::get<1>(GetParam()));
}

class LexerFixtureParametrizedGraphic:
    public LexerFixture,
    public ::testing::WithParamInterface<std::tuple<std::string, std::string, bool>>
{};

INSTANTIATE_TEST_CASE_P(MatchGraphic, LexerFixtureParametrizedGraphic,
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

TEST_P(LexerFixtureParametrizedGraphic, MatchGraphic) {
    source = new Translator::Source<std::istringstream>(std::get<0>(GetParam()));
    lexer = new Lexer(source);
    bool correct = lexer->match_graphic();

    if(correct)
        EXPECT_EQ(lexer->get_match(), std::get<1>(GetParam()));
    EXPECT_EQ(correct, std::get<2>(GetParam()));
}
