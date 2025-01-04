
#include "../Scanner/scanner.h"
#include "../FileScan/FileScan.h"
#include <map>
#include <list>
#include <sstream>
#include "gtest/gtest.h"

namespace {

	TEST(ScannerParse, StringWords) {

		Scanner scan("{item, construction{bracket} cat} something new ", true);
		std::map<LLTOKEN_CODE, int> token_count;
		token_count[L_ERR_TOKEN] = 0;
		std::list<std::string> word_list;
		std::list<std::string> punct_list;
		while (true) {
			scan.get_token();
			auto token = scan.code;
			if (token == L_END_OF_FILE)
				break;
			switch (token) {
			case L_NUMBER_TOKEN:
				token_count[L_NUMBER_TOKEN] += 1;
				break;
			case L_WORD_TOKEN: 
				token_count[L_WORD_TOKEN] += 1;
				word_list.push_back(scan.lexeme);
				break;
			case L_PUNCT_TOKEN:
				token_count[L_PUNCT_TOKEN] += 1;
				punct_list.push_back(scan.lexeme);
				break;
			case L_SPACE_TOKEN:
				token_count[L_SPACE_TOKEN] += 1;
				break;
			case L_ERR_TOKEN:
				token_count[L_ERR_TOKEN] += 1;
				break;
			default:
				token_count[L_ERR_TOKEN] += 1;
				break;
			}
		}
		std::stringstream ss;
		for (auto word : word_list) {
			ss << word << "|";
		}
		EXPECT_EQ(ss.str(), "item|construction|bracket|cat|something|new|");
		ss.str("");
		for (auto p : punct_list) {
			ss << p << "|";
		}
		EXPECT_EQ(ss.str(), "{|,|{|}|}|");
		EXPECT_EQ(token_count[L_WORD_TOKEN], 6);
		EXPECT_EQ(token_count[L_ERR_TOKEN], 0);
		EXPECT_EQ(token_count[L_SPACE_TOKEN], 5);
	}

	TEST(ScannerParse, StringNumber) {

		Scanner scan("{23456, 2{34} 55} something WE234 ", true);
		std::map<LLTOKEN_CODE, int> token_count;
		std::list<std::string> word_list;
		std::list<int> number_list;
		token_count[L_ERR_TOKEN] = 0;
		while (true) {
			scan.get_token();
			auto token = scan.code;
			if (token == L_END_OF_FILE)
				break;
			switch (token) {
			case L_NUMBER_TOKEN:
				token_count[L_NUMBER_TOKEN] += 1;
				number_list.push_back(std::atoi(scan.lexeme));
				break;
			case L_WORD_TOKEN:
				token_count[L_WORD_TOKEN] += 1;
				word_list.push_back(scan.lexeme);
				break;
			case L_PUNCT_TOKEN:
				token_count[L_PUNCT_TOKEN] += 1;
				break;
			case L_SPACE_TOKEN:
				token_count[L_SPACE_TOKEN] += 1;
				break;
			case L_ERR_TOKEN:
				token_count[L_ERR_TOKEN] += 1;
				break;
			default:
				token_count[L_ERR_TOKEN] += 1;
				break;
			}
		}
		std::stringstream ss;
		for (auto word : word_list) {
			ss << word << " ";
		}
		int total = 0;
		for (auto n : number_list) {
			total += n;
		}
		EXPECT_EQ(ss.str(), "something WE ");
		EXPECT_EQ(token_count[L_WORD_TOKEN], 2);
		EXPECT_EQ(token_count[L_NUMBER_TOKEN], 5);
		EXPECT_EQ(token_count[L_ERR_TOKEN], 0);
		EXPECT_EQ(token_count[L_SPACE_TOKEN], 5);
		EXPECT_EQ(total, 23781);
	}

	TEST(ScannerParse, PunctuationDash) {

		Scanner scan("{5 { {  5 something-WE234}}", true);
		std::map<LLTOKEN_CODE, int> token_count;
		std::list<std::string> word_list;
		std::list<std::string> punct_list;
		std::list<std::string> space_list;
		std::list<int> number_list;
		token_count[L_ERR_TOKEN] = 0;
		while (true) {
			scan.get_token();
			auto token = scan.code;
			if (token == L_END_OF_FILE)
				break;
			switch (token) {
			case L_NUMBER_TOKEN:
				token_count[L_NUMBER_TOKEN] += 1;
				number_list.push_back(std::atoi(scan.lexeme));
				break;
			case L_WORD_TOKEN:
				token_count[L_WORD_TOKEN] += 1;
				word_list.push_back(scan.lexeme);
				break;
			case L_PUNCT_TOKEN:
				token_count[L_PUNCT_TOKEN] += 1;
				punct_list.push_back(scan.lexeme);
				break;
			case L_SPACE_TOKEN:
				token_count[L_SPACE_TOKEN] += 1;
				space_list.push_back(scan.lexeme);
				break;
			case L_ERR_TOKEN:
				token_count[L_ERR_TOKEN] += 1;
				break;
			default:
				token_count[L_ERR_TOKEN] += 1;
				break;
			}
		}
		std::stringstream ss;
		for (auto word : word_list) {
			ss << word << " ";
		}
		int total = 0;
		for (auto n : number_list) {
			total += n;
		}
		EXPECT_EQ(ss.str(), "something WE ");
		ss.str("");
		for (auto p : punct_list) {
			ss << p << "|";
		}
		EXPECT_EQ(ss.str(), "{|{|{|-|}|}|");
		ss.str("");
		for (auto p : space_list) {
			ss << p << "|";
		}
		EXPECT_EQ(ss.str(), " | |  | |");
		EXPECT_EQ(token_count[L_SPACE_TOKEN], 4);
		EXPECT_EQ(token_count[L_WORD_TOKEN], 2);
		EXPECT_EQ(token_count[L_NUMBER_TOKEN], 3);
		EXPECT_EQ(token_count[L_PUNCT_TOKEN], 6);
		EXPECT_EQ(token_count[L_ERR_TOKEN], 0);
		EXPECT_EQ(total, 244);
	}

	//TEST(FileScan, StringFileTemplatedDataSyntaxCheck) {

	//	FileScan fs("<div class=\"hero-text\">\n\t<h1>Grids & Flexbox</h1>\n\t<p>Are {{TT super awesome \xF0\x9F\x98\x8D</p>", true);
	//	auto data = fs.get_terminal();

	//	EXPECT_EQ(data, "<div class=\"hero-text\">\n\t<h1>Grids & Flexbox</h1>\n\t<p>Are {{TT super awesome \xF0\x9F\x98\x8D</p>");

	//}

	//TEST(FileScan, StringFileTemplatedDataSyntaxCheck2) {

	//	FileScan fs("<div class=\"hero-text\">\n\t<h1>Grids & Flexbox</h1>\n\t<p>Are {TT=33uper_awesome44 } \xF0\x9F\x98\x8D</p>", true);
	//	auto data = fs.get_terminal();

	//	filegenerate file_out("out_check.html");
	//	file_out.write_text(data.c_str());

	//	EXPECT_EQ(data, "<div class=\"hero-text\">\n\t<h1>Grids & Flexbox</h1>\n\t<p>Are {TT= Parse error!: 33 (number token) on line 3 column 16. 33uper_awesome44 } \xF0\x9F\x98\x8D</p>");
	//}

	//TEST(FileScan, StringFileTemplatedDataSyntaxCheck3) {

	//	FileScan fs("<div class=\"hero-text\">\n\t<h1>Grids & Flexbox</h1>\n\t<p>Are {TT=super awesome44 } \xF0\x9F\x98\x8D{TT=super45} </p>", true);
	//	auto data = fs.get_terminal();

	//	filegenerate file_out("out_check2.html");
	//	file_out.write_text(data.c_str());

	//	EXPECT_EQ(data, "<div class=\"hero-text\">\n\t<h1>Grids & Flexbox</h1>\n\t<p>Are {TT=super  Parse error!: awesome (word token) on line 3 column 22. awesome44 } \xF0\x9F\x98\x8D{TT=super45} </p>");
	//}

	//TEST(FileScan, StringFileTemplatedDataSyntaxCheck4) {

	//	FileScan fs("<div class=\"hero-text\">\n\t<h1>Grids & Flexbox</h1>\n\t<p>Are {TT=super_awesome 44 }</p>", true);
	//	auto data = fs.get_terminal();

	//	filegenerate file_out("out_check2.html");
	//	file_out.write_text(data.c_str());

	//	EXPECT_EQ(data, "<div class=\"hero-text\">\n\t<h1>Grids & Flexbox</h1>\n\t<p>Are {TT=super_awesome  Parse error!: 44 (number token) on line 3 column 30. 44 }</p>");
	//}

}



GTEST_API_ int main(int argc, char **argv) {
	printf("Running main() from gtest_main.cc\n");
	testing::InitGoogleTest(&argc, argv);
	//RUN_ALL_TESTS();
	//int in;
	//std::cin >> in;
	return RUN_ALL_TESTS();
}

