/*
**  file scan
**
**  class definitions for a textual content lexical analyzer
**
*/
#define __DEBUG

#include "FileScan.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <set>
#include <iterator>
#include <algorithm>
#include "../Common/io_utility.h"

#define __DEBUG2


enum Symbols nterm_sym(const char &ch) {
	switch (ch) {
	case 'A': return NTS_A;
	case 'B': return NTS_B;
	case 'C': return NTS_C;
	case 'D': return NTS_D;
	case 'E': return NTS_E;
	case 'F': return NTS_F;
	case 'G': return NTS_G;
	case 'H': return NTS_H;
	case 'I': return NTS_I;
	case 'J': return NTS_J;
	case 'K': return NTS_K;
	case 'L': return NTS_L;
	case 'M': return NTS_M;
	case 'N': return NTS_N;
	case 'O': return NTS_O;
	case 'P': return NTS_P;
	case 'Q': return NTS_Q;
	case 'R': return NTS_R;
	case 'S': return NTS_S;
	case 'T': return NTS_T;
	case 'U': return NTS_U;
	case 'V': return NTS_V;
	case 'W': return NTS_W;
	case 'X': return NTS_X;
	case 'Y': return NTS_Y;
	case 'Z': return NTS_Z;
	default: return TS_INVALID;
	}
}


FileScan::FileScan(const std::string &infilename, bool in_string) : Scanner(infilename, in_string),
ss{}, table{}, table_follows{}, table_firsts{}, termsyms{},
number_iterations(0), number_chars_read(0),
is_tt{ false }
{
	// initialize the symbols stack
	using namespace io_utility;
	auto list = read_file("parsetable.txt", true);
	termsyms["word"] = TS_WORD;
	termsyms["space"] = TS_SPACE;
	termsyms["number"] = TS_NUMBER;
	termsyms["punct"] = TS_PUNCT;
	termsyms["$"] = TS_EOS;
	termsyms["tstart"] = TS_TSTART;
	termsyms["tbegin"] = TS_TBEGIN;
	termsyms["tend"] = TS_TEND;
	termsyms["pequal"] = TS_PEQUAL;
	termsyms["psqt"] = TS_PSQT;
	termsyms["pdqt"] = TS_PDQT;
	termsyms["puscr"] = TS_PUSCR;
	termsyms["pdash"] = TS_PDASH;
	termsyms["~Esp"] = S_ESP;

	bool ptable, firsts, follows;
	ptable = firsts = follows = false;
	if (list.size() > 0) {
		for (auto line : list) {
			if (line.size() > 0) {
				if (line.compare("Parse table") == 0) {
					ptable = true;
					firsts = follows = false;
				}
				else if (line.compare("Firsts") == 0) {
					firsts = true;
					ptable = follows = false;
				}
				else if (line.compare("Follows") == 0) {
					follows = true;
					ptable = firsts = false;
				}
				if (ptable) {
					auto nts_detail = split_string(line, '#');
					if (nts_detail.size() != 2) {
						continue;
					}
					auto nts = split_string(nts_detail[0], '|');
					if (nts.size() == 0) {
						continue;
					}
					auto enumnts = nterm_sym(nts[0][0]);
					if (enumnts == TS_INVALID) { // Should be NTS
						continue;
					}
					auto enumts = term_sym(nts[1]);
					if (enumts == TS_INVALID) { // Should be TS
						continue;
					}

					auto rhs_syms = split_string(nts_detail[1], ' ');
					Symbols push_sym;
					for (auto sym : rhs_syms) {
						push_sym = term_sym(sym);
						if (push_sym == TS_INVALID) { // Not a ts, could be a NTS
							push_sym = nterm_sym(sym[0]);
							if (push_sym == TS_INVALID) { // Should be NTS
								continue;
							}
						}
						table[enumnts][enumts].push_back(push_sym);
					}
				}
				if (firsts || follows) {
					//A | pequal psqt number pdqt punct space tbegin tstart tend word ~Esp
					auto nts_detail = split_string(line, '|');
					if (nts_detail.size() != 2) {
						continue;
					}
					auto enumnts = nterm_sym(nts_detail[0][0]);
					if (enumnts == TS_INVALID) { // Should be NTS
						continue;
					}
					auto rhs_syms = split_string(nts_detail[1], ' ');
					Symbols i_sym;
					for (auto sym : rhs_syms) {
						i_sym = term_sym(sym);
						if (i_sym == TS_INVALID) { // Not a ts, could be a NTS
							continue;
						}
						if (firsts) {
							table_firsts[enumnts].insert(i_sym);
						}
						else if (follows) {
							table_follows[enumnts].insert(i_sym);
						}
					}
				}
			}
		}
	}
	else {
		print({ "Empty parse table file." });
	}

	ss.push(TS_EOS);
	ss.push(NTS_A);
	get_token(); // begin

}

FileScan::~FileScan()
{
}
/*---------------------------------------------------------------------------*
**  "lexer" Converts a valid token to the corresponding terminal symbol
**
**  returns:  Symbols enum of terminal symbol
*/
enum Symbols FileScan::lexer(LLTOKEN_CODE token)
{
	switch (token) {
	case L_NUMBER_TOKEN:
		return TS_NUMBER;
	case L_WORD_TOKEN:
	{
		if (strlen(lexeme) == 2 && strncmp(lexeme, "TT", 2) == 0) {
			return TS_TBEGIN;
		} else {
			return TS_WORD;
		}
	}
	case L_SPACE_TOKEN:
		return TS_SPACE;
	case L_PUNCT_TOKEN:
	{
		if (strlen(lexeme) == 1) {
			if (strncmp(lexeme, "#", 1) == 0) {
				return TS_PHASH;
			}
			else if (strncmp(lexeme, "=", 1) == 0) {
				return TS_PEQUAL;
			}
			else if (strncmp(lexeme, "'", 1) == 0) {
				return TS_PSQT;
			}
			else if (strncmp(lexeme, "\"", 1) == 0) {
				return TS_PDQT;
			}
			else if (strncmp(lexeme, "_", 1) == 0) {
				return TS_PUSCR;
			}
			else if (strncmp(lexeme, "-", 1) == 0) {
				return TS_PDASH;
			}
			else if (strncmp(lexeme, "{", 1) == 0) {
				return TS_TSTART;
			}
			else if (strncmp(lexeme, "}", 1) == 0) {
				return TS_TEND;
			}
			//		} else if (strlen(lexeme) == 2) {
			//if (strncmp(lexeme, "{{", 2) == 0) {
			//	return TS_TSTART;
			//}
			//else if (strncmp(lexeme, "}}", 2) == 0) {
			//	return TS_TEND;
			//}
		}
		return TS_PUNCT;
	}
	case L_END_OF_FILE:
		return TS_EOS; // end of stack: the $ terminal symbol
	//case L_ERR_TOKEN:
	//	return TS_INVALID; // end of stack: the $ terminal symbol
	default:
		return TS_WORD;
	}
}


/*---------------------------------------------------------------------------*
**  returns:  Description of the token
*/
std::string FileScan::token_desc(LLTOKEN_CODE token)
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
void FileScan::get_terminal()
{
	std::stringstream ss_str;
	prev_term = terminal;
	Symbols curr_terminal;

	while (!ss.empty()/*.size() > 0*/) {
		curr_terminal = lexer(code);
		number_iterations++;

		// TS top of stack and input TS equal
		if (curr_terminal == ss.top()) {
			ss_str << lexeme;

			terminal = curr_terminal;
			term_line = on_line;
			term_col = on_col;
			termeme = lexeme;

			get_token(); // get next terminal before return to calle
			ss.pop();   // pop stack and move on with grammer rules
			return;

		} else {

			if (ss.top() == NTS_F && lexer(code) == TS_TBEGIN) {
				tt_parse = true;
			}


			if (table[ss.top()].count(lexer(code)) > 0 &&
				table[ss.top()][lexer(code)].size() > 0) {
				auto top = ss.top();
				if (table[top][lexer(code)][0] == S_ESP) {
					ss.pop();
				}
				else {
					ss.pop();
					auto gr_rule = table[top][lexer(code)];
					for (std::vector<enum Symbols>::reverse_iterator sym = gr_rule.rbegin(); sym != gr_rule.rend(); ++sym) {
						ss.push(*sym);
					}
				}
			}
			else {
				if (ss.top() == NTS_B) {
					// Expecting a punctuation of a # or = here
					// If not we let this be ok, and remove the stack to continue the current TS symbol to flow
					// through in NTS A
					// The template syntax NTS B is a place where to find the start if it is close enough
					while (ss.top() != NTS_A) {
						ss.pop();
					}
					tt_parse = false;
				}
				else {
					// Should not get here if template syntax is free of syntax grammer errors
					if (!(tt_parse && lexer(code) == last_error)) {
						ss_str << " Parse error!: " << token_desc(code) <<
							" on line " << on_line << " column " << on_col << ". ";
						last_error = lexer(code);
					}
					// What is error NTS Symbol? it is ss.top()
					if (table_firsts.count(ss.top()) == 1) {
						std::set<enum Symbols> skipped;
						std::set<enum Symbols> intersect;
						std::set<enum Symbols> sync_set;
						// Skip input until follow(A) is found for top
						if (table_follows.count(ss.top()) == 1) {
							std::set<enum Symbols> &fls = table_follows[ss.top()];
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
						if (tt_parse) {
							sync_set.insert(TS_TEND);
							sync_set.insert(TS_SPACE);
						}
						Symbols skip = lexer(code);
						while (sync_set.count(skip) == 0) {
							ss_str << lexeme;
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
						ss_str << lexeme;
						get_token();
						ss.pop();
					}
					if (ss.size() == 0) {
						ss.push(TS_EOS);
					}
					if (ss.top() == TS_TEND) {
						tt_parse = false;
					}
					if (ss.size() > 0 && ss.top() == TS_EOS) {
						ss.push(NTS_A);
					}
				}
			}
		}
	}

	std::cout << "Number of iterations in filescan: " << number_iterations << std::endl;
}

enum Symbols FileScan::term_sym(const std::string &ts) {
	if (termsyms.count(ts) > 0) {
		return termsyms[ts];
	}
	else {
		return TS_INVALID;
	}
}


