/*
**  llparsedate.h
**
**  class definitions for a textual content mailing label
**
*/

#ifndef LLFILESCAN_H 
#define LLFILESCAN_H 
 
#include "../Common/constants.h"
#include "../Scanner/scanner.h"
#include <set>
#include <vector>

enum Symbols {
	// the symbols:
	// Terminal symbols:
	TS_WORD,         // WORD
	TS_NUMBER,       // NUMBER
	TS_SPACE,        // SPACE
	TS_PUNCT,        // PUNCT
	TS_EOS,          // $, in this case corresponds to '\0' i.e. 0 eqauls no data from
	TS_TSTART,       // TT start {{
	TS_TBEGIN,       // Template begin tag to check if template block is given
	TS_TEND,         // TT end }}
	TS_PHASH,        // #
	TS_PEQUAL,       // =
	TS_PSQT,         // '
	TS_PDQT,         // "
	TS_PUSCR,        // _
	TS_PDASH,        // -
	S_ESP,

	TS_INVALID,     // invalid token

	// Non-terminal symbols:
	NTS_A,          // Html elements as text tokenized
	NTS_B,          // Body of template statement (only assign TT= for test)
	NTS_C,          // Optional space after TBEGIN
	NTS_D,          // Template variable
	NTS_E,          // Optional space before TEND
	NTS_F,         // Template syntax grammer
	NTS_G,         // Literal values (digits or text in quotes)
	NTS_H,          // Stmt to assign a variable with a value.
	NTS_I,         // Optional space of assign equals left side
	NTS_J,         // Optional space of assign equals right side
	NTS_K,         // Double quotes
	NTS_L,         // Single quotes
	NTS_M,         // All charactors allowed - included TS symbols and its text equivalent value
	NTS_N,         // Variable names parse with punc simplified. Would be like (_ , -) 
	NTS_O,         // - optional for variables name with punct 
	NTS_P,         // - optional for variables to have digits 
	NTS_Q,         // - optional for variables to have digits 
	NTS_R,         // - optional for variables to have digits 
	NTS_S,
	NTS_T,
	NTS_U,
	NTS_V,          // Variable names parse
	NTS_W,
	NTS_X,
	NTS_Y,
	NTS_Z
};

typedef std::map< enum Symbols, std::set<enum Symbols> > symbol_follows;
typedef std::map< enum Symbols, std::set<enum Symbols> > symbol_firsts;
typedef std::stack<enum Symbols>     symbol_stack;          // symbol stack
typedef std::map< enum Symbols, std::map<enum Symbols, std::vector<enum Symbols>> > grammar_table;
typedef std::map<std::string, enum Symbols> map_named_termsym;

class FileScan: public Scanner 
{
public:
    FileScan ( const std::string &infilename, bool in_string = false );
    virtual ~FileScan();
    void get_terminal();                 // Get the next terminal scanned with filescan and grammar given
	enum Symbols terminal;               // current terminal
	enum Symbols prev_term;              // prior terminal
//	void accept(Symbols accept_term);
	int         term_line;              // source line of terminal
	int         term_col;               // source line col position of terminal
	std::string termeme;                // terminal string

private:
	symbol_stack ss;                             // Parse stack grammer symbols
	grammar_table table;                         // Grammer table for html templating
	symbol_follows table_follows;                // Follow sets for NTS
	symbol_firsts table_firsts;                  // First sets for NTS
	map_named_termsym termsyms;
	int number_iterations;
	int number_chars_read;
	bool is_tt;
	/*
    Description of a valid token
    */
    std::string token_desc ( LLTOKEN_CODE token );
	//std::string terminal_desc(Symbols sym);
	enum Symbols lexer(LLTOKEN_CODE token);
//	void accept ( LLTOKEN_CODE accept_token );

	enum Symbols term_sym(const std::string &ts);
	bool tt_parse{ false };
	enum Symbols last_error { TS_INVALID };
};

#endif /* LLFILESCAN_H */



