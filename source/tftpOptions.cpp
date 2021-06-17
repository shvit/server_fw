/**
 * \file tftpOptions.cpp
 * \brief TFTP Options class module
 *
 *  License GPL-3.0
 *
 *  \date 29-may-2021
 *  \author Vitaliy Shirinkin, e-mail: vitaliy.shirinkin@gmail.com
 *
 *  \version 0.2
 */

#include "tftpCommon.h"
#include "tftpOptions.h"

namespace tftp
{

//------------------------------------------------------------------------------

Options::Options():
    request_type_{SrvReq::unknown},
    filename_{},
    transfer_mode_{TransfMode::unknown},
    blksize_   {false, constants::dflt_blksize},
    timeout_   {false, constants::dflt_timeout},
    tsize_     {false, constants::dflt_tsize},
    windowsize_{false, constants::dflt_windowsize}
{
}

//------------------------------------------------------------------------------

Options::~Options()
{
}

//------------------------------------------------------------------------------

auto Options::request_type() const -> const SrvReq &
{
  return request_type_;
}

//------------------------------------------------------------------------------

auto Options::filename() const -> const std::string &
{
  return filename_;
}

//------------------------------------------------------------------------------

auto Options::transfer_mode() const -> const TransfMode &
{
  return transfer_mode_;
}

//------------------------------------------------------------------------------

auto Options::blksize() const -> const int &
{
  return std::get<1>(blksize_);
}

//------------------------------------------------------------------------------

auto Options::timeout() const -> const int &
{
  return std::get<1>(timeout_);
}

//------------------------------------------------------------------------------

auto Options::tsize() const -> const int &
{
  return std::get<1>(tsize_);
}

//------------------------------------------------------------------------------

auto Options::windowsize() const -> const int &
{
  return std::get<1>(windowsize_);
}

//------------------------------------------------------------------------------

bool Options::was_set_blksize() const
{
  return std::get<0>(blksize_);
}

bool Options::was_set_timeout() const
{
  return std::get<0>(timeout_);
}

bool Options::was_set_tsize() const
{
  return std::get<0>(tsize_);
}

bool Options::was_set_windowsize() const
{
  return std::get<0>(windowsize_);
}

bool Options::was_set_any() const
{
  return was_set_blksize() ||
         was_set_timeout() ||
         was_set_tsize() ||
         was_set_windowsize();
}

//------------------------------------------------------------------------------

#define OPT_L_INF(MSG) if(log != nullptr) L_INF(MSG);
#define OPT_L_WRN(MSG) if(log != nullptr) L_WRN(MSG);
#define OPT_L_ERR(MSG) if(log != nullptr) L_ERR(MSG);

bool Options::buffer_parse(
    const SmBuf & buf,
    const size_t & buf_size,
    fLogMsg log)
{
  operator=(Options{});

  bool ret = buf_size >= sizeof(int16_t);

  // Init request type
  size_t curr_pos=0U;
  if(ret)
  {
    if(auto rq_type = buf.get_be<int16_t>(curr_pos);
       (ret = ret && ((rq_type == (int16_t)SrvReq::read) ||
                      (rq_type == (int16_t)SrvReq::write))))
    {
      request_type_ = (SrvReq) rq_type;
      OPT_L_INF("Recognize request type '"+
            std::string{to_string(request_type_)}+"'");
      curr_pos += 2U;
    }
    else
    {
      OPT_L_WRN("Wrong request type ("+std::to_string(rq_type)+")");
    }
  }

  // Init filename
  if(ret)
  {
    filename_ = buf.get_string(curr_pos, buf_size-curr_pos);
    if((ret = (filename_.size() > 0U)))
    {
      OPT_L_INF("Recognize filename '"+filename_+"'");
      curr_pos += filename_.size()+1U;
    }
    else
    {
      OPT_L_WRN("Wrong filename (empty!)");
    }
  }

  // Init TFTP transfer mode
  if(ret)
  {
    std::string curr_mod = buf.get_string(curr_pos, buf_size-curr_pos);
    if(set_transfer_mode(curr_mod, log))
    {
      OPT_L_INF("Recognize tranfser mode '"+curr_mod+"'");
    }
    curr_pos += curr_mod.size()+1U;
    /*
    if((ret = (curr_mod.size() > 0U)))
    {
      do_lower(curr_mod);
      if(curr_mod == "netascii") transfer_mode_ = TransfMode::netascii;
      else
      if(curr_mod == "octet") transfer_mode_ = TransfMode::octet;
      else
      if(curr_mod == "binary") transfer_mode_ = TransfMode::binary;
      else
      if(curr_mod == "mail") transfer_mode_ = TransfMode::mail;

      if(transfer_mode_ != TransfMode::unknown)
      {
        OPT_L_INF("Recognize tranfser mode '"+curr_mod+"'");
      }
      else
      {
        OPT_L_WRN("Wrong tranfser mode '"+curr_mod+"'! ");
      }
      curr_pos += curr_mod.size()+1U;
    }
    else
    {
      OPT_L_WRN("Wrong transfer mode (empty!)");
    }
    */
  }

  // Init options
  if(ret)
  {
    while(curr_pos < buf_size)
    {
      std::string str_opt{buf.get_string(
          curr_pos,
          buf_size-curr_pos)};
      do_lower(str_opt);
      curr_pos += str_opt.size()+1U;
      if(curr_pos >= buf_size) break;

      std::string str_val{buf.get_string(
          curr_pos,
          buf_size-curr_pos)};
      curr_pos += str_val.size()+1U;

      bool fl=false;
      if(str_opt == constants::name_blksize) { fl=true; set_blksize(str_val, log); }
      else
      if(str_opt == constants::name_timeout) { fl=true; set_timeout(str_val, log); }
      else
      if(str_opt == constants::name_tsize) { fl=true; set_tsize(str_val, log); }
      else
      if(str_opt == constants::name_windowsize) { fl=true; set_windowsize(str_val, log); }
      else
      {
        OPT_L_WRN("Unknown option '"+str_opt+"'='"+str_val+"'");
      }
      if(fl) OPT_L_INF("Recognize option '"+str_opt+"' value '"+str_val+"'");
    }
  }

  return ret;
}

//------------------------------------------------------------------------------

void Options::set_blksize(
    const std::string & val,
    fLogMsg log)
{
  if(!is_digit_str(val))
  {
    OPT_L_ERR("Wrong option 'blksize' value is '"+val+"'");
    return;
  }

  auto tmp_int = std::stoi(val);

  if(tmp_int < 1)
  {
    OPT_L_ERR("Wrong option 'blksize' too small ("+std::to_string(tmp_int)+")");
  }
  else
  if(tmp_int > 65500)
  {
    L_ERR("Wrong option 'blksize' too large ("+std::to_string(tmp_int)+")");
  }

  blksize_ = {true, tmp_int};
}

//------------------------------------------------------------------------------

void Options::set_timeout(
    const std::string & val,
    fLogMsg log)
{
  if(!is_digit_str(val))
  {
    OPT_L_ERR("Wrong option 'timeout' value is '"+val+"'");
    return;
  }

  auto tmp_int = std::stoi(val);

  if(tmp_int < 1)
  {
    OPT_L_ERR("Wrong option 'timeout' too small ("+std::to_string(tmp_int)+")");
  }
  else
  if(tmp_int > 3600)
  {
    L_ERR("Wrong option 'timeout' too large ("+std::to_string(tmp_int)+")");
  }

  timeout_ = {true, tmp_int};
}

//------------------------------------------------------------------------------

void Options::set_windowsize(
    const std::string & val,
    fLogMsg log)
{
  if(!is_digit_str(val))
  {
    OPT_L_ERR("Wrong option 'windowsize' value is '"+val+"'");
    return;
  }

  auto tmp_int = std::stoi(val);

  if(tmp_int < 1)
  {
    OPT_L_ERR("Wrong option 'windowsize' too small ("+std::to_string(tmp_int)+")");
  }

  windowsize_ = {true, tmp_int};
}

//------------------------------------------------------------------------------

void Options::set_tsize(
    const std::string & val,
    fLogMsg log)
{
  if(!is_digit_str(val))
  {
    OPT_L_ERR("Wrong option 'tsize' value is '"+val+"'");
    return;
  }

  auto tmp_int = std::stoi(val);

  if(tmp_int < 0)
  {
    OPT_L_ERR("Wrong option 'blksize' too small ("+std::to_string(tmp_int)+")");
  }

  tsize_ = {true, tmp_int};
}

//------------------------------------------------------------------------------

bool Options::set_transfer_mode(
    const std::string & val,
    fLogMsg log)
{
  bool ret = false;

  if(val.size() > 0U)
  {
    std::string curr_mod{val};
    do_lower(curr_mod);

    size_t fl = 0U;
    if(curr_mod == "netascii") { ++fl; transfer_mode_ = TransfMode::netascii; }
    else
    if(curr_mod == "octet") { ++fl; transfer_mode_ = TransfMode::octet; }
    else
    if(curr_mod == "binary") { ++fl; transfer_mode_ = TransfMode::binary; }
    else
    if(curr_mod == "mail") { ++fl; transfer_mode_ = TransfMode::mail; }

    if(!(ret = fl))
    {
      OPT_L_WRN("Wrong tranfser mode '"+curr_mod+"'! ");
    }
  }
  else
  {
    OPT_L_WRN("Wrong transfer mode (empty!)");
  }

  return ret;
}

//------------------------------------------------------------------------------

#undef OPT_L_INF
#undef OPT_L_WRN

} // namespace tftp

