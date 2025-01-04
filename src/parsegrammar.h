/*
**  parsegrammar.h
**
**  class definitions for a textual content mailing label
**
*/

#ifndef PARSEGRAMMAR_H 
#define PARSEGRAMMAR_H 
 
#include "../LLScanner/Common/constants.h"
#include "../LLScanner/Scanner/scanner.h"
#include "llparsetable.h"
#include <set>
#include <vector>

namespace llparse {

	typedef std::stack<unsigned int>     symbol_stack;                                               // symbol stack
	typedef std::map<unsigned int, std::map<unsigned int, std::vector<unsigned int> >> parse_table; // parse table;
	typedef std::map< unsigned int, std::set<unsigned int> > symbol_follows;
	typedef std::map< unsigned int, std::set<unsigned int> > symbol_firsts;

	class parsegrammar : public Scanner
	{
	public:
		parsegrammar(const std::string &parse_table, const std::string &infilename, bool in_string = false);
		virtual ~parsegrammar();
	
		void get_terminal();                 // Get all file data into a string
		unsigned int terminal;               // current terminal
		unsigned int prev_term;              // prior terminal
		int         term_line;              // source line of terminal
		int         term_col;               // source line col position of terminal
		std::string terminal_lexeme;                // terminal string
		unsigned int eos_sym{ 0 };
		std::string gr_error{""};

	private:
		symbol_stack ss;                             // Parse stack grammer symbols
		parse_table table;                         // Grammer table for html templating
		symbol_follows table_follows;                // Follow sets for NTS
		symbol_firsts table_firsts;                  // First sets for NTS
		map_token_terminal_symbol token_terminal_symbols;
		std::map<unsigned int, std::string> map_termsyms;
		unsigned int esp_sym{ 0 };
		unsigned int gr_start_ntsym{ 0 };
		int number_iterations;
		int number_chars_read;
		bool is_tt;
		/*
		Description of a valid token
		*/
		std::string token_desc(LLTOKEN_CODE token);
		unsigned int lexer(LLTOKEN_CODE token);
		void accept(LLTOKEN_CODE accept_token);

		bool tt_parse{ false };
		unsigned int last_error { 0 };
	};

};
#endif /* PARSEGRAMMAR_H */



