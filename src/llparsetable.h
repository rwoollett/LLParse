/*
**  llparsetable.h
**
**
*/

#ifndef LLPARSETABLE_H 
#define LLPARSETABLE_H 

#include <map>
#include <set>
#include <vector>
#include <stack>
#include <string>
#include "Scanner/scanner.h"

namespace llparse {
	/**
	 * Function hash
	 * Parameters: char of a terminal name to get hashed unique value
	 * Return: unsigned int
	*/
	unsigned int
		hash(
			const char* s,
			unsigned int seed = 0);
	/**
	 * Function token_category
	 * Parameters: string (name of lltoken in configuration of grammar tokens
	 * Return: LLTOKEN_CODE
	*/
	LLTOKEN_CODE token_category(const std::string &category_token);
	/**
	 * Function token_category_string
	 * Parameters: LLTOKEN_CODE
	 * Return: string (name of lltoken in configuration of grammar tokens
	*/
	std::string token_category_string(LLTOKEN_CODE token);

	typedef std::map< unsigned int, std::vector<std::vector<unsigned int>> > grammar_table2;
	typedef std::map< unsigned int, std::set<unsigned int> > symbol_follows2;
	typedef std::map< unsigned int, std::set<unsigned int> > symbol_firsts2;
	typedef std::pair<LLTOKEN_CODE, unsigned int> terminal_token_pair;
	typedef std::map<terminal_token_pair, unsigned int> map_token_terminal_symbol;

	class llparsetable
	{
		// Private data variables
		grammar_table2 gr2;
		symbol_follows2 table_follows2;
		symbol_firsts2 table_firsts2;
		map_token_terminal_symbol token_terminal_symbols;
		std::map<unsigned int, std::string> map_termsyms;
		std::map<unsigned int, std::string> map_termsym_patterns;
		std::map<unsigned int, std::string> map_nontermsyms;
		unsigned int esp_sym{0};
		unsigned int eos_sym{ 0 };
		unsigned int gr_start_ntsym{ 0 };

	public:
		llparsetable() {
			esp_sym = hash("~Esp");
			eos_sym = hash("$");
			map_termsyms[eos_sym] = "$";

		};
		~llparsetable() {};

		void load_context(const std::vector<std::string>& list);
		std::vector<std::string> ll_parsetable();

		void rhs_symbol2(unsigned int sym_hash, int count, const std::string &sym);
		std::string print_symbol2(unsigned int);
	};


};
#endif /* LLPARSETABLE_H */