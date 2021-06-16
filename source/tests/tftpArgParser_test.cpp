/**
 * \file tftpArgParser_test.cpp
 * \brief Unit-tests for class ArgParser
 *
 *  License GPL-3.0
 *
 *  \date 10-jun-2021
 *  \author Vitaliy Shirinkin, e-mail: vitaliy.shirinkin@gmail.com
 *
 *  \version 0.2
 */

#include "../tftpArgParser.h"
#include "test.h"

using namespace unit_tests;

UNIT_TEST_SUITE_BEGIN(ArgParse)

//------------------------------------------------------------------------------

class ArgParser_test: public tftp::ArgParser
{
public:
  using ArgParser::ArgType;
  using ArgParser::check_arg;
  using ArgParser::construct_arg;
  using ArgParser::construct_args;
  using ArgParser::get_line_out;
};

//------------------------------------------------------------------------------

UNIT_TEST_CASE_BEGIN(main, "Check main methods")

// 1
START_ITER("Test check_arg()");
{
  // Short
  { auto [r1,r2] = ArgParser_test::check_arg("-g"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::is_short); TEST_CHECK_TRUE(r2 == "g"); }
  { auto [r1,r2] = ArgParser_test::check_arg("-A"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::is_short); TEST_CHECK_TRUE(r2 == "A"); }

  // Long
  { auto [r1,r2] = ArgParser_test::check_arg("--g"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::is_long); TEST_CHECK_TRUE(r2 == "g"); }
  { auto [r1,r2] = ArgParser_test::check_arg("--goo"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::is_long); TEST_CHECK_TRUE(r2 == "goo"); }
  { auto [r1,r2] = ArgParser_test::check_arg("--goo-gle"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::is_long); TEST_CHECK_TRUE(r2 == "goo-gle"); }

  // Normal value
  { auto [r1,r2] = ArgParser_test::check_arg("---"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::normal_value); TEST_CHECK_TRUE(r2 == "---"); }
  { auto [r1,r2] = ArgParser_test::check_arg("----"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::normal_value); TEST_CHECK_TRUE(r2 == "----"); }
  { auto [r1,r2] = ArgParser_test::check_arg("-----"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::normal_value); TEST_CHECK_TRUE(r2 == "-----"); }
  { auto [r1,r2] = ArgParser_test::check_arg("10.10.10.10:1000"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::normal_value); TEST_CHECK_TRUE(r2 == "10.10.10.10:1000"); }
  { auto [r1,r2] = ArgParser_test::check_arg("This"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::normal_value); TEST_CHECK_TRUE(r2 == "This"); }

  // End of parse
  { auto [r1,r2] = ArgParser_test::check_arg("--"); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::end_parse); TEST_CHECK_TRUE(r2 == ""); }

  // Zero
  { auto [r1,r2] = ArgParser_test::check_arg(nullptr); TEST_CHECK_TRUE(r1 == ArgParser_test::ArgType::not_found); TEST_CHECK_TRUE(r2 == ""); }
}

// 2
START_ITER("Test construct_arg()");
{
  // wrong
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("-") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("--") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("---") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("----") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("-----") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("-A") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("-AB") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("-defg") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("--A") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("--AB") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("--defg") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg(" ") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg(" A") == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg(" bcd") == "");

  // normal short
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("a") == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("Z") == "-Z");

  // normal long
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("ab") == "--ab");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("abc") == "--abc");
  TEST_CHECK_TRUE(ArgParser_test::construct_arg("Fuck") == "--Fuck");
}

// 3
START_ITER("Test construct_args()");
{
  // Empty
  TEST_CHECK_TRUE(ArgParser_test::construct_args({}) == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"","",""}) == "");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"","-","--","-1","-dfhfgh"}) == "");

  // OK
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"a"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"V"}) == "-V");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"vers"}) == "--vers");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"S","vers"}) == "{-S|--vers}");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"a","biz","def"}) == "{-a|--biz|--def}");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"a","b","d"}) == "{-a|-b|-d}");

  // Partial
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"","a"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"a",""}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"-","a"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"a","-"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"--","a"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"a","--"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"-X","a"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"a","-X"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"--wer","a"}) == "-a");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"a","--wer"}) == "-a");

  TEST_CHECK_TRUE(ArgParser_test::construct_args({"","a","-","Exp"}) == "{-a|--Exp}");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"-F","a","--des","Exp"}) == "{-a|--Exp}");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"","a","-","Exp","--kek"}) == "{-a|--Exp}");
  TEST_CHECK_TRUE(ArgParser_test::construct_args({"-F","a","--des","Exp"," "}) == "{-a|--Exp}");
}

// 4
START_ITER("Test get_line_out()");
{
  TEST_CHECK_TRUE(ArgParser_test::get_line_out({
      int{0},
      tftp::VecStr{},
      tftp::ArgExistVaue::required,
      std::string{"file"},
      std::string{"Nothing"}}) == "Nothing");

  TEST_CHECK_TRUE(ArgParser_test::get_line_out({
      int{0},
      tftp::VecStr{"--"},
      tftp::ArgExistVaue::required,
      std::string{"file"},
      std::string{"Nothing"}}) == "Nothing");

  TEST_CHECK_TRUE(ArgParser_test::get_line_out({
      int{1},
      tftp::VecStr{"L"},
      tftp::ArgExistVaue::optional,
      std::string{""},
      std::string{"Locale"}}) == "-L [<value>] Locale");

  TEST_CHECK_TRUE(ArgParser_test::get_line_out({
      int{1},
      tftp::VecStr{"l", "L", "local"},
      tftp::ArgExistVaue::required,
      std::string{"file"},
      std::string{"Local file name"}}) == "{-l|-L|--local} <file> Local file name");

  TEST_CHECK_TRUE(ArgParser_test::get_line_out({
      int{1},
      tftp::VecStr{"l", "L"},
      tftp::ArgExistVaue::no,
      std::string{"file"},
      std::string{}}) == "{-l|-L} ...");
}

UNIT_TEST_CASE_END

//------------------------------------------------------------------------------

UNIT_TEST_CASE_BEGIN(full_test, "Check method go_full()")


tftp::ArgItems arg_items
{
  { 1, {"l", "L", "local"},      tftp::ArgExistVaue::required, "file", "Local file path and name"},
  { 2, {"r", "R", "remote"},     tftp::ArgExistVaue::required, "file", "Remote file name"},
  { 3, {"g", "G", "get"},        tftp::ArgExistVaue::no,       "", "Get file from server"},
  { 4, {"p", "P", "put"},        tftp::ArgExistVaue::no,       "", "Put file to server"},
  { 5, {"h", "H", "help", "?"},  tftp::ArgExistVaue::no,       "", "Show help information"},
  { 6, {"v", "V", "verb"},       tftp::ArgExistVaue::no,       "", "Set verbosity mode"},
  { 7, {"m", "M", "mode"},       tftp::ArgExistVaue::required, "mode", "TFTP transfer mode"},
  { 8, {"b", "B", "blksize"},    tftp::ArgExistVaue::required, "N", "TFTP option 'block size' (default 512)"},
  { 9, {"t", "T", "timeout"},    tftp::ArgExistVaue::required, "N", "TFTP option 'timeout' (default 10)"},
  {10, {"w", "W", "windowsize"}, tftp::ArgExistVaue::required, "N", "TFTP option 'windowsize' (default 1)"},
  {11, {"tsize"},                tftp::ArgExistVaue::optional, "N", "TFTP option 'tsize'; without value use calculated for WRQ"},
};

const char * tst_args[]=
{
  "./tftp-cl",
  "B-E-G-I-N",
  "-g", "-G", "--get",
  //"-gv",
  "-p", "-P", "--put",
  "-v", "-V", "--verb",
  "-l", "test_local1.txt", "-L", "test_local2.txt", "--local", "test_local3.txt",
  "-r", "test_remote100.txt", "-R", "test_remote200.txt", "--remote", "test_remote300.txt",
  "-m", "netascii", "-M", "octet", "--mode", "mail",
  "-b", "1300", "-B", "812", "--blksize", "4096",
  "-t", "20", "-T", "99", "--timeout", "7",
  "-w", "15", "-W", "51", "--windowsize", "27",
  "--tsize", "--tsize", "232334345",
  "10.0.0.202:6900",
  "-h", "-H", "--help", "-?",
  "ending",
  "--",                               // end parsing
  "--local", "test_local4.txt", "-H", // pass as simple values
};

//tftp::ArgParserout_help_data(arg_items, std::cout);

auto res = tftp::ArgParser::go(
    arg_items,
    sizeof(tst_args)/sizeof(tst_args[0]),
    const_cast<char **>(tst_args));

TEST_CHECK_TRUE(res.second.size() == 6U);
if(res.second.size() > 0U) TEST_CHECK_TRUE(res.second[0U] == "B-E-G-I-N");
if(res.second.size() > 1U) TEST_CHECK_TRUE(res.second[1U] == "10.0.0.202:6900");
if(res.second.size() > 2U) TEST_CHECK_TRUE(res.second[2U] == "ending");
if(res.second.size() > 3U) TEST_CHECK_TRUE(res.second[3U] == "--local");
if(res.second.size() > 4U) TEST_CHECK_TRUE(res.second[4U] == "test_local4.txt");
if(res.second.size() > 5U) TEST_CHECK_TRUE(res.second[5U] == "-H");

TEST_CHECK_TRUE(res.first.size() == 11U);

{
  auto & v = res.first[ 1];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs = v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-l");
    TEST_CHECK_TRUE(v[0U].second == "test_local1.txt");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-L");
      TEST_CHECK_TRUE(v[1U].second == "test_local2.txt");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--local");
        TEST_CHECK_TRUE(v[2U].second == "test_local3.txt");
      }
    }
  }
}
{
  auto & v = res.first[ 2];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-r");
    TEST_CHECK_TRUE(v[0U].second == "test_remote100.txt");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-R");
      TEST_CHECK_TRUE(v[1U].second == "test_remote200.txt");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--remote");
        TEST_CHECK_TRUE(v[2U].second == "test_remote300.txt");
      }
    }
  }
}
{
  auto & v = res.first[ 3];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-g");
    TEST_CHECK_TRUE(v[0U].second == "");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-G");
      TEST_CHECK_TRUE(v[1U].second == "");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--get");
        TEST_CHECK_TRUE(v[2U].second == "");
      }
    }
  }
}
{
  auto & v = res.first[ 4];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-p");
    TEST_CHECK_TRUE(v[0U].second == "");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-P");
      TEST_CHECK_TRUE(v[1U].second == "");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--put");
        TEST_CHECK_TRUE(v[2U].second == "");
      }
    }
  }
}
{
  auto & v = res.first[ 5];
  TEST_CHECK_TRUE(v.size() == 4U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-h");
    TEST_CHECK_TRUE(v[0U].second == "");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-H");
      TEST_CHECK_TRUE(v[1U].second == "");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--help");
        TEST_CHECK_TRUE(v[2U].second == "");
        if(vs>3U)
        {
          TEST_CHECK_TRUE(v[3U].first == "-?");
          TEST_CHECK_TRUE(v[3U].second == "");
        }
      }
    }
  }
}
{
  auto & v = res.first[ 6];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-v");
    TEST_CHECK_TRUE(v[0U].second == "");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-V");
      TEST_CHECK_TRUE(v[1U].second == "");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--verb");
        TEST_CHECK_TRUE(v[2U].second == "");
      }
    }
  }
}
{
  auto & v = res.first[ 7];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-m");
    TEST_CHECK_TRUE(v[0U].second == "netascii");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-M");
      TEST_CHECK_TRUE(v[1U].second == "octet");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--mode");
        TEST_CHECK_TRUE(v[2U].second == "mail");
      }
    }
  }
}
{
  auto & v = res.first[ 8];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-b");
    TEST_CHECK_TRUE(v[0U].second == "1300");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-B");
      TEST_CHECK_TRUE(v[1U].second == "812");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--blksize");
        TEST_CHECK_TRUE(v[2U].second == "4096");
      }
    }
  }
}
{
  auto & v = res.first[ 9];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-t");
    TEST_CHECK_TRUE(v[0U].second == "20");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-T");
      TEST_CHECK_TRUE(v[1U].second == "99");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--timeout");
        TEST_CHECK_TRUE(v[2U].second == "7");
      }
    }
  }
}
{
  auto & v = res.first[10];
  TEST_CHECK_TRUE(v.size() == 3U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "-w");
    TEST_CHECK_TRUE(v[0U].second == "15");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "-W");
      TEST_CHECK_TRUE(v[1U].second == "51");
      if(vs>2U)
      {
        TEST_CHECK_TRUE(v[2U].first == "--windowsize");
        TEST_CHECK_TRUE(v[2U].second == "27");
      }
    }
  }
}
{
  auto & v = res.first[11];
  TEST_CHECK_TRUE(v.size() == 2U);
  if(auto vs=v.size(); vs>0U)
  {
    TEST_CHECK_TRUE(v[0U].first == "--tsize");
    TEST_CHECK_TRUE(v[0U].second == "");
    if(vs>1U)
    {
      TEST_CHECK_TRUE(v[1U].first == "--tsize");
      TEST_CHECK_TRUE(v[1U].second == "232334345");
    }
  }
}

UNIT_TEST_CASE_END

//------------------------------------------------------------------------------

UNIT_TEST_SUITE_END
