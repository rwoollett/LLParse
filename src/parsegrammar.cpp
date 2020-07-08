/*
**  file scan
**
**  class definitions for a textual content lexical analyzer
**
*/
#define __DEBUG

#include "parsegrammar.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <set>
#include <iterator>
#include <algorithm>
#include "io_utility.h"

#define __DEBUG2

namespace llparse {
	using namespace io_utility;

	parsegrammar::parsegrammar(const std::string &parse_table, const std::string &infilename, bool in_string): 
		Scanner(infilename, in_string),	ss{}, table{}, table_follows{}, table_firsts{}, 
		number_iterations(0), number_chars_read(0),
		is_tt{ false }
	{
		esp_sym = hash("~Esp");
		eos_sym = hash("$");
		map_termsyms[eos_sym] = "$";

		// initialize the symbols stack
		auto list = read_file(parse_table, true);

		bool terminals, ptable, firsts, follows;
		terminals = ptable = firsts = follows = false;
		if (list.size() > 0) {
			for (auto line : list) {
				if (line.size() > 0) {
					if (line.compare("Terminal Symbols") == 0) {
						terminals = true;
						ptable = firsts = follows = false;
					}
					else if (line.compare(0, 12, "Parse table:") == 0) {
						ptable = true;
						terminals = firsts = follows = false;
					}
					else if (line.compare("Firsts") == 0) {
						firsts = true;
						terminals = ptable = follows = false;
					}
					else if (line.compare("Follows") == 0) {
						follows = true;
						terminals = ptable = firsts = false;
					}
					if (terminals) {
						auto ts_detail = split_string(line, ' ');
						if (ts_detail.size() == 3) {
							auto lltoken = token_category(ts_detail[0]);
							if (lltoken == L_ERR_TOKEN) {
								continue;
							}
							auto ts = ts_detail[1];
							auto chars = ts_detail[2];
							unsigned int tsym = hash(ts.c_str()); // term sym as int
							terminal_token_pair pair(lltoken, hash(chars.c_str()));
							if (token_terminal_symbols.count(pair) == 0) {
								token_terminal_symbols[pair] = tsym;
								map_termsyms[tsym] = ts;
								//map_termsym_patterns[tsym] = chars;
							}
						}


					}
					else if (ptable) {
						auto nts_detail = split_string(line, '#');
						if (nts_detail.size() != 2) {
							auto ts_detail = split_string(line, ' ');
							if (ts_detail.size() == 3 && ts_detail[0].compare(0, 5, "Parse") == 0 
								&& ts_detail[1].compare(0, 6, "table:") == 0) {
								auto gr_nts = ts_detail[2]; // Setup the start of grammar NTS from this line
								gr_start_ntsym = hash(gr_nts.c_str()); // non term sym as int
								//map_nontermsyms[gr_start_ntsym] = gr_nts;
								//table_follows2[gr_start_ntsym].insert(eos_sym); // endable
							}
							continue;
						}
						auto nts = split_string(nts_detail[0], '|');
						if (nts.size() != 2) {
							continue;
						}
						unsigned int ntsym = hash(nts[0].c_str()); // non term sym as int
						unsigned int tsym = hash(nts[1].c_str()); // term sym as int
						auto rhs_syms = split_string(nts_detail[1], ' ');
						unsigned int push_sym;
						for (auto sym : rhs_syms) {
							push_sym = hash(sym.c_str());// term_sym(sym);
							table[ntsym][tsym].push_back(push_sym);
						}
					}
					if (firsts || follows) {
						auto nts_detail = split_string(line, '|');
						if (nts_detail.size() != 2) {
							continue;
						}
						unsigned int ntsym = hash(nts_detail[0].c_str()); // non term sym as int
						auto rhs_syms = split_string(nts_detail[1], ' ');
						unsigned int i_sym;
						for (auto sym : rhs_syms) {
							i_sym = hash(sym.c_str()); // term sym as int
							if (firsts) {
								table_firsts[ntsym].insert(i_sym);
							}
							else if (follows) {
								table_follows[ntsym].insert(i_sym);
							}
						}
					}
				}
			}
		}
		else {
			print({ "Empty parse table file." });
		}

		ss.push(eos_sym);
		// From parse table stack the first NTS of grammar
		ss.push(gr_start_ntsym);
		get_token();
	}

	parsegrammar::~parsegrammar()
	{
	}
	/*---------------------------------------------------------------------------*
	**  "lexer" Converts a valid token to the corresponding terminal symbol
	**
	**  returns:  Symbols enum of terminal symbol
	*/
	unsigned int parsegrammar::lexer(LLTOKEN_CODE token)
	{
		terminal_token_pair pair(token, hash(lexeme));
		if (token_terminal_symbols.count(pair) > 0) {
			return token_terminal_symbols[pair];
		}
		terminal_token_pair pair_default(token, hash("default"));
		if (token_terminal_symbols.count(pair_default) > 0) {
			return token_terminal_symbols[pair_default];
		}
		switch (token) {
		case L_NUMBER_TOKEN:
		case L_WORD_TOKEN:
		case L_SPACE_TOKEN:
		case L_PUNCT_TOKEN:
			return 0;
		case L_END_OF_FILE:
			return eos_sym; // end of stack: the $ terminal symbol
		//case L_ERR_TOKEN:
		//	return TS_INVALID; // end of stack: the $ terminal symbol
		default:
			return 0; // no known terminal found
		}
	}

	/********************************************************************
	 *  accept - will check if next token read is as expected
	 *******************************************************************/
	void parsegrammar::accept(LLTOKEN_CODE accept_token)
	{
		if (code == accept_token) {
			get_token();
		}
		else {
			std::cout << "accept: Expecting " << token_desc(accept_token) << " on line " << on_line << " " << on_col << ". \n";
		}
	}

	/*---------------------------------------------------------------------------*
	**  returns:  Description of the token
	*/
	std::string parsegrammar::token_desc(LLTOKEN_CODE token)
	{
		std::stringstream str;
		switch (token) {
		case L_NUMBER_TOKEN:
			str << lexeme << " (number token)";
			break;
		case L_WORD_TOKEN:
			str << lexeme << " (word token)";
			break;
		case L_SPACE_TOKEN:
			str << lexeme << " (space token)";
			break;
		case L_PUNCT_TOKEN:
			str << lexeme << " (punctuation token)";
			break;
		case L_END_OF_FILE:
			str << lexeme << " (end-of-file token)";
			break;
		case L_ERR_TOKEN:
			str << lexeme << " (error token)";
			break;
		default:
			str << lexeme << " (error token)";
			break;
		}
		return str.str();
	}

	/*---------------------------------------------------------------------------*
	**  "get_file_data" extracts the scanned tokens from the file
	**
	**  returns:  nothing
	*/
	void parsegrammar::get_terminal()
	{
		std::stringstream ss_error;
		gr_error = "";
		prev_term = terminal;
		unsigned int curr_terminal;

		while (!ss.empty()/*.size() > 0*/) {
			number_iterations++;
			curr_terminal = lexer(code);
			//if (ss.top() == eos_sym) {
			//	break; // EOS reached
			//}
			// TS top of stack and input TS equal
			if (curr_terminal == ss.top()) {
				terminal = curr_terminal;
				term_line = on_line;
				term_col = on_col;
				terminal_lexeme = lexeme;
				gr_error = ss_error.str();

				get_token();
				ss.pop();   // pop stack and move on with grammer rules
				return;

			}
			else {

				//if (ss.top() == NTS_F && lexer(code) == TS_TBEGIN) {
				//	tt_parse = true;
				//}

				if (table[ss.top()].count(lexer(code)) > 0 &&
					table[ss.top()][lexer(code)].size() > 0) {
					auto top = ss.top();
					if (table[top][lexer(code)][0] == esp_sym) {
						ss.pop();
					}
					else {
						ss.pop();
						auto gr_rule = table[top][lexer(code)];
						for (std::vector<unsigned int>::reverse_iterator sym = gr_rule.rbegin(); sym != gr_rule.rend(); ++sym) {
							ss.push(*sym);
						}
					}
				}
				else {
					//if (ss.top() == 55555) {
					//	// Expecting a punctuation of a # or = here
					//	// If not we let this be ok, and remove the stack to continue the current TS symbol to flow
					//	// through in NTS A
					//	// The template syntax NTS B is a place where to find the start if it is close enough
					//	while (ss.top() != 55555) {
					//		ss.pop();
					//	}
					//	tt_parse = false;
					//}
					//else {
						// Should not get here if template syntax is free of syntax grammer errors
					if (!(tt_parse && lexer(code) == last_error)) {
						ss_error << " Parse error!: " << token_desc(code) <<
							" on line " << on_line << " column " << on_col << ". ";
						last_error = lexer(code);
					}
					// What is error NTS Symbol? it is ss.top()
					if (table_firsts.count(ss.top()) == 1) {
						std::set<unsigned int> skipped;
						std::set<unsigned int> intersect;
						std::set<unsigned int> sync_set;
						// Skip input until follow(A) is found for top
						if (table_follows.count(ss.top()) == 1) {
							std::set<unsigned int> &fls = table_follows[ss.top()];
							if (table_firsts.count(ss.top()) == 1) {
								std::set_union(
									fls.begin(), fls.end(),
									table_firsts[ss.top()].begin(), table_firsts[ss.top()].end(),
									std::inserter(sync_set, sync_set.begin()));
							}
							else {
								sync_set = fls;
							}
						}
						else {
							sync_set = table_firsts[ss.top()];
						}
						// If we have TTBegin, add TTEnd to sync set 
						//if (tt_parse) {
						//	sync_set.insert(TS_TEND);
						//	sync_set.insert(TS_SPACE);
						//}
						unsigned int skip = lexer(code);
						while (sync_set.count(skip) == 0 && skip != eos_sym) {
							ss_error << lexeme;
							skipped.insert(skip);
							get_token();// skip it
							skip = lexer(code);
						}
						skipped.insert(skip); // will be what symbol skipped upto in sync_set
						if (table_firsts.count(ss.top()) == 1) {
							std::set_intersection(
								table_firsts[ss.top()].begin(), table_firsts[ss.top()].end(),
								skipped.begin(), skipped.end(),
								std::inserter(intersect, intersect.begin()));
						}
						if (intersect.size() == 0) {
							ss.pop(); // only follows have symbol so pop
						}
					}
					else {
						ss_error << lexeme;
						get_token();
						ss.pop();
					}
					if (ss.size() == 0) {
						ss.push(eos_sym);
					}
					//if (ss.top() == TS_TEND) {
					//	tt_parse = false;
					//}
					//if (ss.size() > 0 && ss.top() == TS_EOS) {
					//	ss.push(NTS_A);
					//}
//					}
				}
			}
		}

		std::cout << "Number of iterations in filescan: " << number_iterations << std::endl;

	}

}