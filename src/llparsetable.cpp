#include "llparsetable.h"
#include "Common/io_utility.h"
#include <functional>

namespace llparse {


	unsigned int
		hash(
			const char* s,
			unsigned int seed) 
	{
		unsigned int hash = seed;
		while (*s)
		{
			hash = hash * 101 + *s++;
		}
		return hash;
	}


	using namespace io_utility;

	LLTOKEN_CODE token_category(const std::string &category_token)
	{
		if (category_token.compare("punct") == 0) {
			return L_PUNCT_TOKEN;
		}
		else if (category_token.compare("digit") == 0) {
			return L_NUMBER_TOKEN;
		}
		else if (category_token.compare("space") == 0) {
			return L_SPACE_TOKEN;
		}
		else if (category_token.compare("alpha") == 0) {
			return L_WORD_TOKEN;
		}
		return L_ERR_TOKEN;
	}

	std::string token_category_string(LLTOKEN_CODE token)
	{
		if (token == L_PUNCT_TOKEN) {
			return "punct";
		}
		else if (token == L_NUMBER_TOKEN) {
			return "digit";
		}
		else if (token == L_SPACE_TOKEN) {
			return "space";
		}
		else if (token == L_WORD_TOKEN) {
			return "alpha";
		}
		return "_";
	}

	void llparsetable::load_context(const std::vector<std::string> &list) {

		if (list.size() > 0) {
			//  punct j -
			//	punct u {
			//	punct v }
			//	digit n default
			//	space s default
			//	alpha w default
			//	alpha t TT
			//
			//  E -> s v | v.
			//  B -> e D.

			// The symbol used - grouped into punct, digit, space and alpha in this version
			// Required: and the first four lines
			LLTOKEN_CODE lltoken;
			bool rules = false;
			for (auto line : list) {
				auto ts_detail = split_string(line, ' ');
				if (ts_detail.size() > 1 && ts_detail[0].compare("Grammar:") == 0 ) {
					rules = true; // empty line and continue to rules
					auto gr_nts = ts_detail[1]; // Setup the start of grammar NTS from this line
					gr_start_ntsym = hash(gr_nts.c_str()); // non term sym as int
					map_nontermsyms[gr_start_ntsym] = gr_nts;
					table_follows2[gr_start_ntsym].insert(eos_sym); // endable
					continue;
				}
				if (!rules) {
					if (ts_detail.size() == 3) {
						lltoken = token_category(ts_detail[0]);
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
							map_termsym_patterns[tsym] = chars;
						}
					}

				}
				else {
					//A -> p F A | w A | s A | n A | m A | o A | .
					auto nts_detail = split_string(line, '>');
					if (nts_detail.size() == 2) {
						auto nts = split_string(nts_detail[0], '-');
						if (nts.size() > 0) {
							unsigned int ntsym = hash(nts[0].c_str()); // non term sym as int
							map_nontermsyms[ntsym] = nts[0];
							auto nts_rules = split_string(nts_detail[1], '|');
							int count = 0;
							for (auto r : nts_rules) {
								gr2[ntsym].push_back({});
								// o A | .
								auto syms = split_string(r, ' ');
								for (auto ss : syms) {
									if (ss.size() == 1 && ss[0] == '.' && syms.size() == 1) {// a lone . means NTS has an eps_sym ie A-> .... |.
										gr2[ntsym][count].push_back(esp_sym);
									}
									else {
										rhs_symbol2(ntsym, count, ss); 
									}
								}
								count++;
							}
						}

					}
				}
			}
		}

		// LL1 First and Follows 
		for (auto t : map_nontermsyms) {
			table_firsts2.insert({ t.first,{} });
		}

		bool changed = true;
		while (changed) { // keep doing until First() sets dont change
			changed = false;
			for (auto t : map_nontermsyms) { // map of NTS and rules
				for (auto w : gr2[t.first]) { // each rule for NTS
					if (w.size() > 0) {
						if (gr2.count(w[0]) == 0) { // first symbol in rhs is NOT NTS - ie not found in map of NTS to list of rules
							auto e_chk = table_firsts2[t.first].insert(w[0]); // add TS to First (t) (esp is added as a first when found to mark it nullable)
							if (!changed && e_chk.second)
								changed = true;
						}
					}
				}
			}
			for (auto t : map_nontermsyms) { // map of NTS and rules
				for (auto w : gr2[t.first]) { // each rule for NTS
					if (w.size() > 0) {
						if (gr2.count(w[0]) > 0) { // first symbol in rhs is NTS - ie found in map of NTS to list of rules IS NTS
							for (auto n : table_firsts2[w[0]]) {
								auto e_chk = table_firsts2[t.first].insert(n);  // add all First of (w[0]) to First(t)
								if (!changed && e_chk.second)
									changed = true;
							}
						}
					}
				}
			}

			for (auto t : map_nontermsyms) { // map of NTS and rules
				for (auto w : gr2[t.first]) { // each rule for NTS
					if (w.size() > 0) {
						int cont = 0, done = 0;
						while (cont < w.size() - 1 && done == cont) {        // each symbol in rhs ensure the i < j and 1 to i is only NTS
							if ((gr2.count(w[cont]) > 0) &&                   // symbol in rhs is NTS 
								(table_firsts2[w[cont]].count(esp_sym) > 0) &&  // does NTS have esp in firsts (can it be nullable)
								(gr2.count(w[cont + 1]) > 0)) {               // is next symbol an NTS. Add First(cont + 1) (the next NTS) 
								for (auto n : table_firsts2[w[cont + 1]]) {
									auto e_chk = table_firsts2[t.first].insert(n);               // add all First of (cont + 1)) to First(t)
									if (!changed && e_chk.second)
										changed = true;
								}
								done++;
							}
							cont++;
						}
					}
				}
			}
		}

		// LL1 Follows 
		changed = true;
		while (changed) { // keep doing until First() sets dont change
			changed = false;
			for (auto t : map_nontermsyms) { // map of NTS and rules
				for (auto w : gr2[t.first]) { // each rule for NTS
					if (w.size() > 0) {
						int cont = 0;
						while (cont < w.size() - 1) {        // each symbol in rhs ensure 2 or more
															 //if (t != w[cont + 1]) {
							if ((gr2.count(w[cont]) > 0)) {// symbol in rhs is NTS 
								if (gr2.count(w[cont + 1]) > 0) {// is next symbol an NTS. 
																// does next NTS have esp in firsts (can it be nullable)
									int next_nts = cont + 1, done = 0;
									done = next_nts;
									while (next_nts < w.size() && done == next_nts) {        // each symbol in rhs ensure the i < j and 1 to i is only NTS
										if (table_firsts2[w[next_nts]].count(esp_sym) > 0) {
											done++;
										}
										for (auto n : table_firsts2[w[next_nts]]) {
											if (n != esp_sym) {
												auto e_chk = table_follows2[w[cont]].insert(n);               // add all Follow(t) to Follow(cont)
												if (!changed && e_chk.second)
													changed = true;
											}
										}
										next_nts++;
									}
									if (done == next_nts) { // the last iteration of NTSs where all eps so add follow(t) to cont
										for (auto n : table_follows2[t.first]) {
											auto e_chk = table_follows2[w[cont]].insert(n); // add all Follow(t) to Follow(cont) as all next nts are nullable
											if (!changed && e_chk.second)
												changed = true;
										}

									}
									// does last NTS have esp in firsts (can it be nullable)
									if (cont == w.size() - 2) {
										for (auto n : table_follows2[t.first]) {
											auto e_chk = table_follows2[w[cont + 1]].insert(n);           // add all Follow(t) to Follow(cont + 1) 
											if (!changed && e_chk.second)
												changed = true;
										}
									}
								}
								else { // next symbol is a TS add First(symbol) to Follow(cont)
									if (w[cont + 1] != esp_sym) {
										auto e_chk = table_follows2[w[cont]].insert(w[cont + 1]);
										if (!changed && e_chk.second)
											changed = true;
									}
								}
							}
							else if ((gr2.count(w[cont]) == 0) &&             // symbol in rhs is TS 
								(gr2.count(w[cont + 1]) > 0)) {               // is next symbol an NTS. 
								if (cont == w.size() - 2)                    // last NTS 
									for (auto n : table_follows2[t.first]) {
										auto e_chk = table_follows2[w[cont + 1]].insert(n);     // add all Follow(t) to Follow(cont + 1)
										if (!changed && e_chk.second)
											changed = true;
									}
							}
							//}
							cont++;
						}
						// Need to check rhs with one symbol and not eps (really a rudundant rule that is not needed, such as B->C)

					}
				}
			}
		}


	}

	std::vector<std::string> llparsetable::ll_parsetable() {
		std::vector<std::string> lines;

		lines.push_back("Terminal Symbols");
		for (auto item : token_terminal_symbols) {
			auto token = item.first.first;
			auto ts = map_termsyms[item.second];
			auto ts_pattern = map_termsym_patterns[item.second];
			lines.push_back(token_category_string(token) + " " + ts + " " + ts_pattern);
		}

		lines.push_back("Parse table: " + map_nontermsyms[gr_start_ntsym]);

		std::string line;
		bool add_ts = false;
		for (auto t : map_nontermsyms) { // map of NTS and rules
			for (auto w : gr2[t.first]) { // each rule for NTS
				if (w.size() == 0) {
					continue;
				}
				for (auto ts : map_termsyms) { // map of TS
					add_ts = false;
					if (table_firsts2[t.first].count(ts.first) > 0 && w[0] == ts.first) {
						add_ts = true;
					}
					else if (table_firsts2[t.first].count(esp_sym) > 0 && w[0] == esp_sym && table_follows2[t.first].count(ts.first)) {
						add_ts = true;
					}
					else if (/*list_ntermsyms_name.count(w[0]) > 0 &&*/ map_nontermsyms.count(w[0]) > 0 && table_firsts2[w[0]].count(ts.first) > 0) {
						add_ts = true;
					}
					else if (/*list_ntermsyms_name.count(w[0]) > 0 && */ map_nontermsyms.count(w[0]) > 0 && table_firsts2[w[0]].count(esp_sym) > 0 && table_follows2[w[0]].count(ts.first)) {
						add_ts = true;
					}
					if (add_ts) {
						line = "";
						line += t.second + " | " + ts.second;
						//line += list_ntermsyms_name[t.first] + " | " + print_symbol(ts.first);
						line += " # ";
						for (auto sym : w) {
							line += print_symbol2(sym) + " ";
						}
						lines.push_back(line);
					}
				}
			}
		}

		lines.push_back("Firsts");
		for (auto t : table_firsts2) { // map of NTS and rules
			line = "";
			line += map_nontermsyms[t.first] + " | ";
			for (auto sym : t.second) {
				line += print_symbol2(sym) + " ";
			}
			lines.push_back(line);
		}

		lines.push_back("Follows");
		for (auto t : table_follows2) { // map of NTS and rules
			line = "";
			line += map_nontermsyms[t.first] + " | ";
			for (auto sym : t.second) {
				line += print_symbol2(sym) + " ";
			}
			lines.push_back(line);
		}

		return lines;
	}

	void llparsetable::rhs_symbol2(unsigned int sym_hash, int count, const std::string &sym)
	{
		auto sym_parts = split_string(sym, '.'); // remove the dot at end of rhs 
		if (sym_parts.size() == 0) {
			return; // could be a space before the dot, so sym is only the dot at end of rhs symbols
		}
		auto only_sym = sym_parts[0]; // this will be first always and when and if dot present
		auto rhs_sym_hash = hash(only_sym.c_str());
		if (map_termsyms.count(rhs_sym_hash) > 0) {
			gr2[sym_hash][count].push_back(rhs_sym_hash);
		}
		else { // it is a NTS and can be added to map_nontermsyms now if not found
			if (map_nontermsyms.count(rhs_sym_hash) == 0) {
				map_nontermsyms[rhs_sym_hash] = only_sym;
			}
			gr2[sym_hash][count].push_back(rhs_sym_hash);
		}
	}

	std::string llparsetable::print_symbol2(unsigned int sym)
	{
		if (map_termsyms.count(sym) > 0) {
			return map_termsyms[sym];
		}
		else {
			if (map_nontermsyms.count(sym) > 0) {
				return map_nontermsyms[sym];
			}
			else if (sym == esp_sym) {
				return "~Esp";
			}
			else {
				return "_";
			}
		}
	}

}