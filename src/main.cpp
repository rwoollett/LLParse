
#include "io_utility.h"
#include "llparsetable.h"
#include "parsegrammar.h"
#include "Generate/filegenerate.h"
#include <sstream>
#include "main.h"

using namespace io_utility;

/*
  accept_terminal
  Parse the template variable name
  Parameters: parse_grammar
			  ss_out

*/
void accept_terminal(llparse::parsegrammar & parse_grammar, std::stringstream &ss_out)
{
	parse_grammar.get_terminal();
	ss_out << parse_grammar.gr_error << parse_grammar.terminal_lexeme;
}


/*
  parse_TT_variable
  Parse the template variable name
  Parameters: parse_grammar
			  ss_out

*/
void parse_TT_literal(llparse::parsegrammar & parse_grammar, std::stringstream & ss_out, const unsigned int quote_sym)
{
	const unsigned int a_spc = llparse::hash("spc");
	const unsigned int a_word = llparse::hash("word");
	const unsigned int a_num = llparse::hash("num");
	const unsigned int a_uscr = llparse::hash("uscr");
	const unsigned int a_dash = llparse::hash("dash");
	const unsigned int a_sqt = llparse::hash("sqt");
	const unsigned int a_dqt = llparse::hash("dqt");
	const unsigned int a_lcb = llparse::hash("lcb");
	const unsigned int a_rcb = llparse::hash("rcb");
	const unsigned int a_eq = llparse::hash("eq");
	const unsigned int a_punct = llparse::hash("punct");
	// Process the value quoted
	accept_terminal(parse_grammar, ss_out);
	while (parse_grammar.terminal == a_word ||
		(parse_grammar.terminal == a_sqt && quote_sym == a_dqt) ||
		(parse_grammar.terminal == a_dqt && quote_sym == a_sqt) ||
		parse_grammar.terminal == a_spc ||
		parse_grammar.terminal == a_num ||
		parse_grammar.terminal == a_lcb ||
		parse_grammar.terminal == a_rcb ||
		parse_grammar.terminal == a_eq ||
		parse_grammar.terminal == a_punct ||
		parse_grammar.terminal == a_dash) {
		accept_terminal(parse_grammar, ss_out);
	}
	// Expect a quote_sym to end.
	if (parse_grammar.terminal == quote_sym) {
		accept_terminal(parse_grammar, ss_out);
	}
	else {
		// Show this as an error
		ss_out << " Syntax error :No end quote found. ";
		accept_terminal(parse_grammar, ss_out);
	}
}

/*
  parse_TT_variable
  Parse the template variable name
  Parameters: parse_grammar
			  ss_out
*/
void parse_TT_variable(llparse::parsegrammar & parse_grammar, std::stringstream & ss_out)
{
	const unsigned int a_spc = llparse::hash("spc");
	const unsigned int a_word = llparse::hash("word");
	const unsigned int a_num = llparse::hash("num");
	const unsigned int a_uscr = llparse::hash("uscr");
	const unsigned int a_dash = llparse::hash("dash");
	const unsigned int a_sqt = llparse::hash("sqt");
	const unsigned int a_dqt = llparse::hash("dqt");

	// Process the variable name syntax 
	accept_terminal(parse_grammar, ss_out);
	while (parse_grammar.terminal == a_word ||
		parse_grammar.terminal == a_num ||
		parse_grammar.terminal == a_uscr ||
		parse_grammar.terminal == a_dash) {
		accept_terminal(parse_grammar, ss_out);
	}
}

/*
  parse_TT_spaces
  Parse over spaces
  Parameters: parse_grammar
			  ss_out
  Skips over space and putting then into ss_out

*/
void parse_TT_spaces(llparse::parsegrammar & parse_grammar, std::stringstream & ss_out)
{
	const unsigned int a_spc = llparse::hash("spc");
	while (parse_grammar.terminal == a_spc) {
		accept_terminal(parse_grammar, ss_out);
	}
}

/*
  parse_TT_write
  Parse the write to HTML output syntax template
  Parameters: parse_grammar
			  ss_out

*/
void parse_TT_write(llparse::parsegrammar & parse_grammar, std::stringstream & ss_out)
{
	const unsigned int a_rcb = llparse::hash("rcb");
	const unsigned int a_sqt = llparse::hash("sqt");
	const unsigned int a_dqt = llparse::hash("dqt");
	const unsigned int a_word = llparse::hash("word");
	const unsigned int a_num = llparse::hash("num");

	accept_terminal(parse_grammar, ss_out);
	parse_TT_spaces(parse_grammar, ss_out);
	// Double quotes for a literal value
	if (parse_grammar.terminal == a_dqt) {
		parse_TT_literal(parse_grammar, ss_out, a_dqt);
	}
	else if (parse_grammar.terminal == a_sqt) {
		parse_TT_literal(parse_grammar, ss_out, a_sqt);
	}
	else if (parse_grammar.terminal == a_word) {
		parse_TT_variable(parse_grammar, ss_out);
	}
	else if (parse_grammar.terminal == a_num) {
		accept_terminal(parse_grammar, ss_out);
	}
}

/*
  parse_TT_if
  Parse the if..fi
  Parameters: parse_grammar
			  ss_out

*/
int if_nested = 0;
void parse_TT_if(llparse::parsegrammar & parse_grammar, std::stringstream & ss_out)
{
	const unsigned int a_rcb = llparse::hash("rcb");
	const unsigned int a_lt = llparse::hash("lt");
	const unsigned int a_gt = llparse::hash("gt");
	const unsigned int a_ex = llparse::hash("ex");
	const unsigned int a_eq = llparse::hash("eq");
	const unsigned int a_word = llparse::hash("word");
	const unsigned int a_fi = llparse::hash("fi");
	const unsigned int a_elsif = llparse::hash("elsif");
	const unsigned int a_else = llparse::hash("else");
	const unsigned int a_if = llparse::hash("if");

	if (parse_grammar.terminal == a_if) {
		if_nested++;
	}
	ss_out << "[nest:" << if_nested << "] ";
	// accept if 
	accept_terminal(parse_grammar, ss_out);

	parse_TT_spaces(parse_grammar, ss_out);
	// var1 vaiable
	if (parse_grammar.terminal == a_word) {
		parse_TT_variable(parse_grammar, ss_out);
	}
	else {
		ss_out << " Syntax error : missing first variable in if.";
	}

	parse_TT_spaces(parse_grammar, ss_out);
	// comparison condition
	//Y -> lt V | gt V | ex V | eq V.
	//V -> eq | .
	if (parse_grammar.terminal == a_gt ||
		parse_grammar.terminal == a_lt ||
		parse_grammar.terminal == a_ex ||
		parse_grammar.terminal == a_eq) {
		accept_terminal(parse_grammar, ss_out);
	}
	else {
		ss_out << " Syntax error : missing comparison in if.";
	}
	if (parse_grammar.terminal == a_eq) {
		accept_terminal(parse_grammar, ss_out);
	}

	parse_TT_spaces(parse_grammar, ss_out);
	// var2 variable
	if (parse_grammar.terminal == a_word) {
		parse_TT_variable(parse_grammar, ss_out);
	}
	else {
		ss_out << " Syntax error : missing second variable in if.";
	}

	parse_TT_spaces(parse_grammar, ss_out);
	if (parse_grammar.terminal == a_rcb) {
		parse_template(parse_grammar, ss_out);
		if (parse_grammar.terminal == a_fi) {
			ss_out << "[fi nest:" << if_nested << "] ";
			if_nested--;
			accept_terminal(parse_grammar, ss_out);
		}
		else if (parse_grammar.terminal == a_elsif) {
			ss_out << "[elsif nest:" << if_nested << "] ";
			//accept_terminal(parse_grammar, ss_out);
			parse_TT_if(parse_grammar, ss_out);
		}
		else if (parse_grammar.terminal == a_else) {
			ss_out << "[else nest:" << if_nested << "] ";
			accept_terminal(parse_grammar, ss_out);
			parse_TT_spaces(parse_grammar, ss_out);
			if (parse_grammar.terminal == a_rcb) {
				parse_template(parse_grammar, ss_out);
				if (parse_grammar.terminal == a_fi) {
					ss_out << "[fi nest:" << if_nested << "] ";
					if_nested--;
					accept_terminal(parse_grammar, ss_out);
				}
				else {
					ss_out << " Syntax error : missing an ending fi at else. ";
				}
			}
			else {
				ss_out << " Syntax error : missing } at else.";
			}
		}
		else {
			ss_out << " Syntax error : missing an ending fi";
		}
	}
	else {
		ss_out << " Syntax error : missing }";
	}

}

/*
  parse_TT_for
  Parse the for..next
  Parameters: parse_grammar
			  ss_out

*/
int for_nested = 0;
void parse_TT_for(llparse::parsegrammar & parse_grammar, std::stringstream & ss_out)
{
	const unsigned int a_rcb = llparse::hash("rcb");
	const unsigned int a_word = llparse::hash("word");
	const unsigned int a_for = llparse::hash("for");
	const unsigned int a_next = llparse::hash("next");
	const unsigned int a_colon = llparse::hash("colon");

	for_nested++;
	ss_out << "[nest:" << for_nested << "] ";
	// accept for 
	accept_terminal(parse_grammar, ss_out);

	parse_TT_spaces(parse_grammar, ss_out);
	// Iter vaiable
	if (parse_grammar.terminal == a_word) {
		parse_TT_variable(parse_grammar, ss_out);
	}
	else {
		ss_out << " Syntax error : missing first variable in for.";
	}

	parse_TT_spaces(parse_grammar, ss_out);
	// Colon seperatee
	if (parse_grammar.terminal == a_colon) {
		accept_terminal(parse_grammar, ss_out);
	}
	else {
		ss_out << " Syntax error : missing colon in for.";
	}

	parse_TT_spaces(parse_grammar, ss_out);
	// List variable
	if (parse_grammar.terminal == a_word) {
		parse_TT_variable(parse_grammar, ss_out);
	}
	else {
		ss_out << " Syntax error : missing second variable in for.";
	}

	parse_TT_spaces(parse_grammar, ss_out);
	if (parse_grammar.terminal == a_rcb) {
		parse_template(parse_grammar, ss_out);
		if (parse_grammar.terminal != a_next) {
			ss_out << " Syntax error : missing an ending next";
		}
		else {
			ss_out << "[end nest:" << for_nested << "] ";
			for_nested--;
			accept_terminal(parse_grammar, ss_out);
		}
	}
	else {
		ss_out << " Syntax error : missing }";
	}

}

/*
  parse_TT_statement
  Parse the statements
  Parameters: parse_grammar
			  ss_out

*/
void parse_TT_statement(llparse::parsegrammar & parse_grammar, std::stringstream & ss_out)
{
	const unsigned int a_for = llparse::hash("for");
	const unsigned int a_next = llparse::hash("next");
	const unsigned int a_if = llparse::hash("if");
	const unsigned int a_elsif = llparse::hash("elsif");
	const unsigned int a_else = llparse::hash("else");
	const unsigned int a_fi = llparse::hash("fi");

	accept_terminal(parse_grammar, ss_out);
	parse_TT_spaces(parse_grammar, ss_out);

	if (parse_grammar.terminal == a_for) {
		parse_TT_for(parse_grammar, ss_out);
	}
	else if (parse_grammar.terminal == a_next) {
		return;
	}
	else if (parse_grammar.terminal == a_if) {
		parse_TT_if(parse_grammar, ss_out);
	}
	else if (parse_grammar.terminal == a_fi ||
		parse_grammar.terminal == a_else ||
		parse_grammar.terminal == a_elsif) {
		return;
	}
}

/*
  parse_lcb_TTBody_rcb
  Parse the template symtax body
  Parameters: parse_grammar
			  html_block
			  ss_out

*/
void parse_lcb_TTBody_rcb(llparse::parsegrammar & parse_grammar, std::stringstream & ss_out)
{
	const unsigned int a_tstart = llparse::hash("tstart");
	const unsigned int a_lcb = llparse::hash("lcb");
	const unsigned int a_eq = llparse::hash("eq");
	const unsigned int a_hash = llparse::hash("hash");
	const unsigned int a_rcb = llparse::hash("rcb");
	const unsigned int a_spc = llparse::hash("spc");
	const unsigned int a_sqt = llparse::hash("sqt");
	const unsigned int a_dqt = llparse::hash("dqt");
	const unsigned int a_word = llparse::hash("word");
	const unsigned int a_num = llparse::hash("num");
	const unsigned int a_next = llparse::hash("next");
	const unsigned int a_fi = llparse::hash("fi");
	const unsigned int a_elsif = llparse::hash("elsif");
	const unsigned int a_else = llparse::hash("else");

	std::stringstream holder;
	accept_terminal(parse_grammar, holder);
	// Process the Template syntax 
	if (parse_grammar.terminal == a_tstart) {
		accept_terminal(parse_grammar, holder);
		if (parse_grammar.terminal == a_eq ||
			parse_grammar.terminal == a_hash) {
			if (parse_grammar.terminal == a_eq) {
				parse_TT_write(parse_grammar, holder);
			}
			else if (parse_grammar.terminal == a_hash) {
				parse_TT_statement(parse_grammar, holder);
				if (parse_grammar.terminal == a_next ||
					parse_grammar.terminal == a_fi ||
					parse_grammar.terminal == a_else ||
					parse_grammar.terminal == a_elsif) {
					ss_out << holder.str();
					return; // end of containmnet in for .. next or if .. fi
				}
			}

			parse_TT_spaces(parse_grammar, holder);
			if (parse_grammar.terminal == a_rcb) {
				// This is a complete statement make the mark as good
				accept_terminal(parse_grammar, holder);
			}
			else {
				// Show this as an error
				holder << " Syntax error :No end } found. ";
				accept_terminal(parse_grammar, holder);
			}
		}
	}
	ss_out << holder.str();
}


/*
  parse_template
  Top level to parse the template file
  Parameters: parse_grammar
		      ss_out

*/
void parse_template(llparse::parsegrammar &parse_grammar, std::stringstream &ss_out)
{
	const unsigned int a_tstart = llparse::hash("tstart");
	const unsigned int a_lcb = llparse::hash("lcb");
	const unsigned int a_esc = llparse::hash("esc");
	const unsigned int a_rcb = llparse::hash("rcb");
	const unsigned int a_word = llparse::hash("word");
	const unsigned int a_next = llparse::hash("next");
	const unsigned int a_fi = llparse::hash("fi");
	const unsigned int a_elsif = llparse::hash("elsif");
	const unsigned int a_else = llparse::hash("else");
	std::stringstream ss_html;
	bool processedTT = false;

	accept_terminal(parse_grammar, ss_html);
	while (!processedTT) {
		while (parse_grammar.terminal != parse_grammar.eos_sym &&
			parse_grammar.terminal != a_esc &&
			parse_grammar.terminal != a_lcb) {
			accept_terminal(parse_grammar, ss_html);
		}
		if (parse_grammar.terminal == parse_grammar.eos_sym) {
			ss_out << ss_html.str();
			return; // done
		}

		// Process the Template syntax 
		if (parse_grammar.terminal == a_lcb) {
			ss_out << ss_html.str();
			parse_lcb_TTBody_rcb(parse_grammar, ss_out);
			processedTT = true;
		}
		else {
			accept_terminal(parse_grammar, ss_html); // get esc
			accept_terminal(parse_grammar, ss_html); // get escaped char
		}
	}
	if (parse_grammar.terminal == a_next ||
		parse_grammar.terminal == a_fi ||
		parse_grammar.terminal == a_else ||
		parse_grammar.terminal == a_elsif) {
		return;
	}
	parse_template(parse_grammar, ss_out);

}


int main()
{
	llparse::llparsetable llParseTable;

	print({ "LL Parse table generator" });
	print({ "Pick the filename where the grammar CFG is found." });
	auto filename = pick_file(".", ".txt");
	if (filename.size() <= 0) {
		return 0;
	}

	auto list = read_file(filename, true);
	llParseTable.load_context(list);
	std::vector<std::string> lines = llParseTable.ll_parsetable();
	print({ "Enter parse table output file. Press <Enter> to use default [out.txt]" });
	auto parse_table_file = create_file();
	if (parse_table_file.compare("") == 0) {
		parse_table_file = "out.txt";
	}
	save_to_file(parse_table_file, lines, true);

	// Print out check
	print({ "Check the parse table - quit if not looking correct." });
	for (auto l :lines) {
		print({ l });
	}
	auto choice = get_answer_choice("Press q to exit or c to continue with process of the test file:", { 'Q', 'q', 'c', 'C' });
	if (choice == 'q' || choice == 'Q')
		return 0;

	llparse::parsegrammar parse_grammar(parse_table_file, "test.txt");

	std::stringstream ss_out;
	parse_template(parse_grammar, ss_out);

	filegenerate file_out("out_check.html");
	file_out.write_text(ss_out.str().c_str());

	return 0;
}
