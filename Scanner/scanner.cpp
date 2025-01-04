/*
**  scanner.cpp
**
**  class definitions for a textual content lexical analyzer
**
**  scanner - new constructor to prepare the i/o system called file to read
** ~scanner - destructor to release filename
**  advance - used a unbuffered I/O to buffer the input into the scanner
*/
#define __DEBUG2
#define __DEBUG

#include "scanner.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// #ifdef _WINDOWS
// #include <io.h>
// #include <clocale>
// #else /* _WINDOWS */
// #include <unistd.h>
// #endif /* _WINDOWS */
#include <cstdio>

#include <cstring>
#include <iostream>
#include <string>

Scanner::Scanner(const std::string &infilename, bool in_string) : code(L_START_OF_FILE), previous(L_END_OF_FILE), on_line(0), on_col(0),
                                                                  i_src(0), i_lex(0), read_size_buffer(0), file_scan(NULL), line_count(0),
                                                                  col_pos_count(0), curr_ch(0), next_ch(0), input_string(in_string),
                                                                  filename(infilename), number_iterations(0), number_chars_read(0)
{
  lexeme[0] = '\0';
  buffer[0] = '\0';

  if (input_string == true)
  {
    // Get buffer from string in constructor
    i_src = 0;      // set no data read yet
    line_count = 1; // line number on 1
    col_pos_count = 1;

    // Read the buffer ready for scanning
#ifdef DEBUG2
    std::cout << "String Buffer size: " << BUFFER_SIZE << std::endl;
#endif
    // Check the input string is not going to larger than our buffer.
    // Only using a sensible size so wont be in large sizes.
    // If oversized it will be truncated and not added on later the same as file contents are.
    // Only using one buffer for string inputs
    std::size_t slen = infilename.length();
    read_size_buffer = 0;
    if (slen > BUFFER_SIZE)
    {
      read_size_buffer = BUFFER_SIZE;
    }
    else
    {
      read_size_buffer = (int)slen;
    }
    strncpy(buffer, infilename.c_str(), read_size_buffer);
    buffer[read_size_buffer] = '\0';
#ifdef DEBUG2
    std::cout << "String amount read " << slen << std::endl;
    std::cout << "buffer             " << buffer << std::endl;
    std::cout << "buffer length      " << strlen(buffer) << std::endl;
#endif
    initialize_table();
    advance(); // begin
  }
  else
  {
    // Get file handle and buffer from the file
    char errbuffer[30];
    file_scan = fopen(infilename.c_str(), "r");
    if (file_scan == NULL)
    {
      code = L_ERR_TOKEN;
      std::string msg(infilename + " is not able to be read." + errbuffer);
      strncpy(lexeme, msg.c_str(), msg.length());
      lexeme[msg.length()] = '\0';
    }
    else
    {
      i_src = 0;      // set no data read yet
      line_count = 1; // line number on 1
      col_pos_count = 1;

      // Read the buffer ready for scanning
#ifdef DEBUG2
      std::cout << "Buffer size: " << sizeof(buffer) << std::endl;
#endif
      read_size_buffer = fread(buffer, sizeof(char), sizeof(buffer), file_scan);
#ifdef DEBUG2
      std::cout << "amount read " << read_size_buffer << std::endl;
#endif
      number_chars_read += read_size_buffer;
      initialize_table();
      advance(); // begin
    }
  }
}

Scanner::~Scanner()
{
  // Make sure read file is close with file handle
  if (file_scan != NULL)
  {
#ifdef DEBUG
    std::cout << "file h : " << file_scan << " closed" << std::endl;
#endif
    fclose(file_scan);
  }
}
/*---------------------------------------------------------------------------*
**  "advance" sets curr_ch to the next character in the source file.
**
**  returns:  the current character
*/
char Scanner::advance()
{
  // file_scan is read to get a buufer when empty input found
  int forward = i_src + 1;
#ifdef DEBUG2
  std::cout << "forward check =  " << forward << std::endl;
#endif

  if (read_size_buffer == 0)
  {
    curr_ch = 0;
    next_ch = 0;
    return 0; // EOF
  }

  curr_ch = buffer[i_src];

  if (input_string == true)
  { // only when reading a file do it in buffers
    if (forward == read_size_buffer)
    {
      next_ch = 0;
      i_src = 0;
      read_size_buffer = 0; // No more buffers to be read so zero read set
    }
    else
    {
      next_ch = buffer[forward];
      ++i_src;
    }
  }
  else
  {
    if (forward == read_size_buffer)
    { // end of a buffer for forward
#ifdef DEBUG2
      std::cout << "Read forward buffer size: " << sizeof(buffer) << std::endl;
#endif
      read_size_buffer = fread(buffer, sizeof(char), sizeof(buffer), file_scan);
      next_ch = 0;
      number_chars_read += read_size_buffer;
#ifdef DEBUG2
      std::cout << "amount read " << read_size_buffer << std::endl;
#endif

      if (read_size_buffer == 0)
      {
        next_ch = 0;
      }
      else
      {
        next_ch = buffer[0];
      }

      i_src = 0;
    }
    else
    {
      next_ch = buffer[forward];
      ++i_src;
    }
  }

  if (curr_ch == '\n')
  {
    line_count++;
    col_pos_count = 1;
  }
  else
  {
    if (curr_ch == '\t')
    {
      col_pos_count += 3;
    }
    ++col_pos_count;
  }

  return curr_ch;
}
/*---------------------------------------------------------------------------*
 * *  "initialize_table"
 **
 */
void Scanner::initialize_table()
{
  // initialize the symbols stack
  ss.push(STS_EOS);
  ss.push(SNTS_S);

  // setup the lexical scanner parsing table
  table[SNTS_S][STS_ALPHA] = 1;
  table[SNTS_S][STS_DIGIT] = 1;
  table[SNTS_S][STS_PUNCT] = 1;
  table[SNTS_S][STS_SPACE] = 1;
  table[SNTS_S][STS_EOS] = 1;

  table[SNTS_T][STS_ALPHA] = 5;
  table[SNTS_T][STS_DIGIT] = 4;
  table[SNTS_T][STS_PUNCT] = 3;
  table[SNTS_T][STS_SPACE] = 2;
  table[SNTS_T][STS_EOS] = 6;

  table[SNTS_A][STS_ALPHA] = 7;
  table[SNTS_A][STS_DIGIT] = 8;
  table[SNTS_A][STS_PUNCT] = 8;
  table[SNTS_A][STS_SPACE] = 8;
  table[SNTS_A][STS_EOS] = 8;

  // table[SNTS_T1][STS_ALPHA] = 9;
  table[SNTS_T1][STS_DIGIT] = 11;
  table[SNTS_T1][STS_PUNCT] = 10;
  table[SNTS_T1][STS_SPACE] = 9;
  table[SNTS_T1][STS_EOS] = 12;

  table[SNTS_D][STS_ALPHA] = 14;
  table[SNTS_D][STS_DIGIT] = 13;
  table[SNTS_D][STS_PUNCT] = 14;
  table[SNTS_D][STS_SPACE] = 14;
  table[SNTS_D][STS_EOS] = 14;

  table[SNTS_T2][STS_ALPHA] = 17;
  // table[SNTS_T2][STS_DIGIT] = 24;
  table[SNTS_T2][STS_PUNCT] = 16;
  table[SNTS_T2][STS_SPACE] = 15;
  table[SNTS_T2][STS_EOS] = 18;

  table[SNTS_W][STS_ALPHA] = 20;
  table[SNTS_W][STS_DIGIT] = 20;
  table[SNTS_W][STS_PUNCT] = 20;
  table[SNTS_W][STS_SPACE] = 19;
  table[SNTS_W][STS_EOS] = 20;

  table[SNTS_T3][STS_ALPHA] = 23;
  table[SNTS_T3][STS_DIGIT] = 21;
  table[SNTS_T3][STS_PUNCT] = 22;
  // table[SNTS_T3][STS_SPACE] = 23;
  table[SNTS_T3][STS_EOS] = 24;
}
/********************************************************************
 *  showtoken - will output the token
 *******************************************************************/

std::string Scanner::showsymbol(ScannerSymbols symbol)
{

  std::string message(lexeme);
  switch (symbol)
  {
  case STS_ALPHA:
    message += " ALPHA";
    break;
  case STS_DIGIT:
    message += " DIGIT";
    break;
  case STS_SPACE:
    message += " SPACE";
    break;
  case STS_PUNCT:
    message += " PUNCT";
    break;
  case SNTS_S:
    message = " SNTS_S";
    break;
  case SNTS_T:
    message = " SNTS_T";
    break;
  case SNTS_A:
    message = " SNTS_A";
    break;
  case SNTS_T1:
    message = " SNTS_T1";
    break;
  case SNTS_D:
    message = " SNTS_D";
    break;
  case SNTS_T2:
    message = " SNTS_T2";
    break;
  case SNTS_W:
    message = " SNTS_W";
    break;
  case SNTS_T3:
    message = " SNTS_T3";
    break;
  case STS_EOS:
    message = " STS_EOS";
    break;
  default: // do nothing
    message = " default";
    break;
  }

  return message;
}

/*---------------------------------------------------------------------------*
**  "symbolslexer" The scanner general symbol found by characters
**
**  returns:  Symbols enum of terminal symbol
*/
unsigned char Scanner::ischar(char ch)
{
  return static_cast<unsigned char>(ch);
}

enum ScannerSymbols Scanner::symbolslexer()
{
  unsigned char ch = static_cast<unsigned char>(curr_ch); // ischar(curr_ch);
  // std::setlocale(LC_ALL, "en_GB.iso88591");
  if (ch >= 0x00 && ch <= 0xff)
  { // only char values
    if (isspace(ch))
    {
      return STS_SPACE;
    }
    else if (ispunct(ch))
    {
      return STS_PUNCT;
    }
    else if (isalpha(ch))
    {
      return STS_ALPHA;
    }
    else if (isdigit(ch))
    {
      return STS_DIGIT;
    }
    else if (ch == 0)
    {
      return STS_EOS;
    }
    else if (isgraph(ch))
    {
      return STS_ALPHA;
    }
    else
    {
      return STS_ALPHA;
    }
  }
  else
  {
    return STS_ALPHA; // INVALID;
  }
}

/*---------------------------------------------------------------------------*
**  "lexer" Converts a valid token to the corresponding terminal symbol
**
**  returns:  Symbols enum of terminal symbol
*/
LLTOKEN_CODE Scanner::tokenlexer(ScannerSymbols symbol)
{
  switch (symbol)
  {
  case STS_ALPHA:
    return L_WORD_TOKEN;
  case STS_DIGIT:
    return L_NUMBER_TOKEN;
  case STS_PUNCT:
    return L_PUNCT_TOKEN;
  case STS_SPACE:
    return L_SPACE_TOKEN;
  case STS_EOS:
    return L_END_OF_FILE; // end of stack: the $ terminal symbol
  default:
    return L_ERR_TOKEN;
  }
}

/*---------------------------------------------------------------------------*
**  "get_token" extracts the next token from the source file
**
**  returns:  nothing
*/
void Scanner::get_token()
{
  previous = code;
  ScannerSymbols curr_symbol;

  while (!ss.empty() /*.size() > 0*/)
  {
    curr_symbol = symbolslexer();
    number_iterations++;

    // TS top of stack and input TS equal
    if (curr_symbol == ss.top())
    {
      on_line = line_count;
      on_col = col_pos_count - (i_lex + 1);

      lexeme[i_lex] = curr_ch; // add current input (TS == curr_ch), advance input
      advance();
      ss.pop(); // pop stack and move on with grammer rules
      i_lex++;  // increment lexeme for next curr_ch (could be within the content for a TOKEN to find

      // Return with ready lexeme and code token found for callee
      if (symbolslexer() != curr_symbol || curr_symbol == STS_PUNCT)
      {
        code = tokenlexer(curr_symbol);
        lexeme[i_lex] = 0;
#ifdef DEBUG
        std::cout << "Matched symbols: " << showsymbol(curr_symbol) << std::endl;
#endif
        i_lex = 0;
        return;
      }
    }
    else
    {

      // NTS top of stack - current symbol from input (TS) - determines state number
      switch (table[ss.top()][curr_symbol])
      {
      case 1: // 1. LexicalSymbol Grammer start NTS T
        ss.pop();
        ss.push(SNTS_T);
        break;

      case 2: // T : space W
        ss.pop();
        ss.push(SNTS_W);
        ss.push(STS_SPACE);
        break;

      case 3: // T : punct T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_PUNCT);
        break;

      case 4: // T : digit D
        ss.pop();
        ss.push(SNTS_D);
        ss.push(STS_DIGIT);
        break;

      case 5: // T : alpha A
        ss.pop();
        ss.push(SNTS_A);
        ss.push(STS_ALPHA);
        break;

      case 6: // T : eps
        ss.pop();
        break;

      case 7: // A : alpha  A
        ss.pop();
        ss.push(SNTS_A);
        ss.push(STS_ALPHA);
        break;

      case 8: // A : T1
        ss.pop();
        ss.push(SNTS_T1);
        break;

      case 9: // T1 : space T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_SPACE);
        break;

      case 10: // T1 : punct T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_PUNCT);
        break;

      case 11: // T1 : digit T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_DIGIT);
        break;

      case 12: // T1 : eps
        ss.pop();
        break;

      case 13: // D : digit D
        ss.pop();
        ss.push(SNTS_D);
        ss.push(STS_DIGIT);
        break;

      case 14: // D : T2
        ss.pop();
        ss.push(SNTS_T2);
        break;

      case 15: // T2 : space T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_SPACE);
        break;

      case 16: // T2 : punct T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_PUNCT);
        break;

      case 17: // T2 : alpha T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_ALPHA);
        break;

      case 18: // T2 : eps
        ss.pop();
        break;

      case 19: // W : space W
        ss.pop();
        ss.push(SNTS_W);
        ss.push(STS_SPACE);
        break;

      case 20: // W : T3
        ss.pop();
        ss.push(SNTS_T3);
        break;

      case 21: // T3 : digit T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_DIGIT);
        break;

      case 22: // T3 : punct T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_PUNCT);
        break;

      case 23: // T3 : alpha T
        ss.pop();
        ss.push(SNTS_T);
        ss.push(STS_ALPHA);
        break;

      case 24: // T3 : eps
        ss.pop();
        break;

      default: // should not get here if above is correct
        std::cout << "tokenizer parsing table defaulted: Expecting " << showsymbol(curr_symbol) << " on line " << on_line << " column " << on_col << std::endl;
        ss.pop();
        advance();
        break;
      }
    }
  }

  code = L_END_OF_FILE;
  std::cout << "Number of chars read: " << number_chars_read << std::endl;
  std::cout << "Number of iterations: " << number_iterations << std::endl;
}
