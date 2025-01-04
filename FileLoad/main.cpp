//
//--------------------------------------------------------------------
//
// RunParse.cpp : Defines the entry point for the console application.
//
//--------------------------------------------------------------------
//
#include "../Utilities/Utilities.h"
#include "../Common/constants.h"

#include "../Scanner/scanner.h"

#include "../FileScan/FileScan.h"
#include "../Common/io_utility.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

int verbose = 0;
int string_input = 0;
std::stringstream text_input;
std::string out_file_name = "file_out.html";
std::string program_name;

void usage();
void do_file(const char *const name);
std::string terminal_desc(Symbols sym);

/********************************************************************
 *  accept - will check if next token read is as expected
 *******************************************************************/
void accept(Symbols accept_term, FileScan &fs)
{
  if (fs.terminal == accept_term)
  {
    fs.get_terminal();
  }
  else
  {
    std::cout << "Expecting" << terminal_desc(accept_term)
              << " found " << fs.termeme << terminal_desc(fs.terminal)
              << " on line " << fs.term_line << " " << fs.term_col
              << ". \n";
  }
}

/*---------------------------------------------------------------------------*
**  returns:  Description of the token
*/
std::string terminal_desc(Symbols sym)
{
  std::stringstream str;
  switch (sym)
  {
  case TS_NUMBER:
    str << " (number)";
    break;
  case TS_TBEGIN:
    str << " (TT begin)";
    break;
  case TS_WORD:
    str << " (word)";
    break;
  case TS_SPACE:
    str << " (space)";
    break;
  case TS_PUNCT:
    str << " (punctuation)";
    break;
  case TS_PHASH:
    str << " (#)";
    break;
  case TS_PEQUAL:
    str << " (=)";
    break;
  case TS_PSQT:
    str << " (')";
    break;
  case TS_PDQT:
    str << " (\")";
    break;
  case TS_PUSCR:
    str << " (_)";
    break;
  case TS_PDASH:
    str << " (-)";
    break;
  case TS_TSTART:
    str << " ({)";
    break;
  case TS_TEND:
    str << " (})";
    break;
  case TS_EOS:
    str << " (end-of-file)";
    break;
  default:
    str << " (unexpected)";
    break;
  }
  return str.str();
}

//
//------------------------------------------------------------------------
// Usage
//------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{

  program_name = argv[0];

  while ((argc > 1) && (argv[1][0] == '-'))
  {

    switch (argv[1][1])
    {
    case 'v':
      verbose = 1;
      break;
    case 'o':
      out_file_name = &argv[1][2];
      break;
    case 's':
      string_input = 1;
      text_input << &argv[1][2];
      break;
    default:
      std::cerr << "Bad option " << argv[1] << std::endl;
      usage();
    }

    ++argv;
    --argc;
  }

  io_utility::save_to_file(out_file_name, {});

  if (argc == 1)
  {
    do_file("index2.html");
  }
  else
  {
    while (argc > 1)
    {
      do_file(argv[1]);
      ++argv;
      --argc;
    }
  }
  return 0;
}
//
//------------------------------------------------------------------------
// Usage
//------------------------------------------------------------------------
//
void do_file(const char *const name)
{
  CUtilities utilities;
  double duration;

  std::cout << " Verbose " << verbose << " Input text used: " << string_input << " String text: " << text_input.str() << " Output " << out_file_name << std::endl;
  std::cout << "Requested file to scan is " << name << "\n";
  std::cout << "Requested file to generate is " << out_file_name << "\n";

  // Timer of the procesrs
  utilities.start();

  try
  {
    bool in_string = false;
    std::string in_file_name(name);
    if (string_input)
    {
      in_string = true;
      in_file_name = text_input.str();
    }
    FileScan fs(in_file_name, in_string);

    std::stringstream ss_str;
    fs.get_terminal();
    while (fs.terminal != TS_EOS)
    {
      //			accept(TS_NUMBER, fs); // test the accept

      switch (fs.terminal)
      {
      case TS_WORD:
        ss_str << fs.termeme;
        accept(TS_WORD, fs);
        break;
      case TS_PUNCT:
        ss_str << fs.termeme;
        accept(TS_PUNCT, fs);
        break;
      // case TS_NUMBER:
      //	break;
      default:
        ss_str << fs.termeme;
        fs.get_terminal();
        break;
      }
    }

    std::cout << ss_str.str();
    io_utility::log_file(out_file_name, {ss_str.str()});
  }
  catch (std::runtime_error &err)
  {
    std::cout << "Error: # " << err.what() << "\n";
  }

  // Show how long it took
  duration = utilities.stop(); // end stop watch
  std::cout << "The generation of file took " << duration << " seconds.\n";
  // std::string wait;
  // std::getline(std::cin, wait);
}
//
//------------------------------------------------------------------------
// Usage
//------------------------------------------------------------------------
//
void usage()
{
  std::cerr << "Usage is " << program_name << " [options] [file-list]" << std::endl;
  std::cerr << "Options" << std::endl;
  std::cerr << "  -v              verbose" << std::endl;
  std::cerr << "  -s<quote text>  Use string in quotes (\"\") as input text. No file-list is read." << std::endl;
  std::cerr << "  -o<name>        Set output file name" << std::endl;
  exit(8);
}
