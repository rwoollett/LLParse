#pragma once

void NewFunction(llparse::parsegrammar & parse_grammar, const unsigned int &a_rcb, std::stringstream &ss_html, std::stringstream & ss_out, bool &retflag);

void parse_template(llparse::parsegrammar &parse_grammar, std::stringstream &ss_out);

void accept_terminal(llparse::parsegrammar & parse_grammar, std::stringstream &holder);

void parse_TT_spaces(llparse::parsegrammar & parse_grammar, const unsigned int &a_spc, std::stringstream & ss_out);

void NewFunction(std::stringstream & ss_out, llparse::parsegrammar & parse_grammar, const unsigned int &a_spc, const unsigned int &a_dqt, const unsigned int &a_sqt, const unsigned int &a_word, const unsigned int &a_num, const unsigned int &a_rcb);
