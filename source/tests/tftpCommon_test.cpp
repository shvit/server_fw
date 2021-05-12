/**
 * \file tftpCommon_test.cpp
 * \brief Unit-tests for common module
 *
 *  License GPL-3.0
 *
 *  \date   07-may-2021
 *  \author Vitaliy Shirinkin, e-mail: vitaliy.shirinkin@gmail.com
 *
 *  \version 0.1
 */

#include "../tftpCommon.h"
#include "test.h"     

UNIT_TEST_SUITE_BEGIN(Common)

//------------------------------------------------------------------------------

UNIT_TEST_CASE_BEGIN(common_functions, "Check common functions")

  // Check to_string()
#define CHK_TO_STRING(T,NAME) \
    {\
      constexpr auto s1 = tftp::to_string(tftp::T::NAME);\
      TEST_CHECK_TRUE(s1 == #NAME);\
    }

  CHK_TO_STRING(SrvReq, unknown)
  CHK_TO_STRING(SrvReq, read)
  CHK_TO_STRING(SrvReq, write)

  CHK_TO_STRING(TransfMode, unknown)
  CHK_TO_STRING(TransfMode, netascii)
  CHK_TO_STRING(TransfMode, octet)
  CHK_TO_STRING(TransfMode, binary)

  CHK_TO_STRING(LogLvl, emerg)
  CHK_TO_STRING(LogLvl, alert)
  CHK_TO_STRING(LogLvl, crit)
  CHK_TO_STRING(LogLvl, err)
  CHK_TO_STRING(LogLvl, warning)
  CHK_TO_STRING(LogLvl, notice)
  CHK_TO_STRING(LogLvl, info)
  CHK_TO_STRING(LogLvl, debug)

  CHK_TO_STRING(State, need_init);
  CHK_TO_STRING(State, error_and_stop);
  CHK_TO_STRING(State, ack_options);
  CHK_TO_STRING(State, data_tx);
  CHK_TO_STRING(State, data_rx);
  CHK_TO_STRING(State, ack_tx);
  CHK_TO_STRING(State, ack_rx);
  CHK_TO_STRING(State, retransmit);
  CHK_TO_STRING(State, finish);

#undef CHK_TO_STRING

  // Check operator+
  std::string s1= "A"+tftp::State::need_init+"B";
  TEST_CHECK_TRUE(s1=="Aneed_initB");

  std::string s2= "C"+tftp::LogLvl::warning+"D";
  TEST_CHECK_TRUE(s2=="CwarningD");

  std::string s3= "E"+tftp::TransfMode::octet+"F";
  TEST_CHECK_TRUE(s3=="EoctetF");

  std::string s4= "G"+tftp::SrvReq::write+"H";
  TEST_CHECK_TRUE(s4=="GwriteH");

//
UNIT_TEST_CASE_END

//------------------------------------------------------------------------------

UNIT_TEST_SUITE_END
