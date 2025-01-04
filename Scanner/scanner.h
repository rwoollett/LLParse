/*
**  scanner.h
**
**  class definitions for a textual content lexical analyzer
**  advance - used a C I/O to buffer the input in a char buffer
**  scanner is for token finding for dates
*/

#ifndef LLSCANNER_H
#define LLSCANNER_H

#include "../Common/constants.h"
#include <map>
#include <stack>


// The parser token enum wanted by the lexical scanner - these would be dfined
// the the parser
typedef enum {
    L_START_OF_FILE,
    L_ERR_TOKEN,
    L_END_OF_FILE,
    L_NUMBER_TOKEN,
    L_WORD_TOKEN,
    L_PUNCT_TOKEN,
	L_SPACE_TOKEN
}
LLTOKEN_CODE;

enum ScannerSymbols {
    // the general symbols used by lexical anlyser that can be used by
    // parser tokens to build up a token in the scanner
    // Terminal symbols:
    STS_ALPHA,       // ALPHA accepted character
    STS_DIGIT,       // Digit
    STS_PUNCT,       // Punctuation
    STS_SPACE,       // Space, whitespace
    STS_EOS,         // $, in this case corresponds to '\0'
	STS_XCHAR,       // extended char

    // Non-terminal symbols - these would be defined by the parser grammer to build
    // up a token using an ll(1) ebnf
    SNTS_S,
	SNTS_T,
	SNTS_A,
	SNTS_T1,
    SNTS_D,
	SNTS_T2,
	SNTS_W,
	SNTS_T3
}
;

typedef std::map< enum ScannerSymbols, std::map<enum ScannerSymbols, int> > scanner_symbol_table;
typedef std::stack<enum ScannerSymbols>     scanner_symbol_stack;          // symbol stack


class Scanner
{
public:
    LLTOKEN_CODE  code;                   // current token
    LLTOKEN_CODE  previous;               // prior token
    int         on_line;                // source line of token
    int         on_col;                 // source line col position of token
    char        lexeme[80];             // token string

    void get_token();

    Scanner ( const std::string &infilename, bool in_string = false );
    virtual ~Scanner();

private:
    long i_src;                         // current location in source
    long i_lex;                         // current location in token lexeme
    int read_size_buffer;              // size read for buffer2
    FILE* file_scan;                      // file input descriptor
    int  line_count;                    // current line in source
    int  col_pos_count;                 // current line col position in source
    char curr_ch;                       // current char in source text
    char next_ch;                       // next char in source text
    bool input_string;                   // input string is filename or a buffer true = a buffer false = filename
    std::string filename;              // filename of the source file
    scanner_symbol_table table;
    scanner_symbol_stack ss;
    char buffer[BUFFER_SIZE];          // buffer one of input
	int number_iterations;
	int number_chars_read;

    char advance();
	unsigned char ischar(char ch);
	enum ScannerSymbols symbolslexer();
    LLTOKEN_CODE tokenlexer ( ScannerSymbols symbol );
    std::string showsymbol ( ScannerSymbols symbol );
    void initialize_table();


};


#endif  /* LLSCANNER_H */



