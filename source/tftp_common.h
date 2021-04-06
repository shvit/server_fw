/**
 * \file tftp_common.h
 * \brief Common TFTP server header
 *
 *  Base classes for TFTP server
 *
 *  License GPL-3.0
 *
 *  \date   01-dec-2019
 *  \author Vitaliy Shirinkin, e-mail: vitaliy.shirinkin@gmail.com
 *
 *  \version 0.1
 */

#ifndef SOURCE_TFTP_COMMON_H_
#define SOURCE_TFTP_COMMON_H_

#include <cassert>
#include <cxxabi.h> // for current class/type name
#include <functional>
#include <ostream>
#include <string>
#include <syslog.h>
#include <vector>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <iomanip>


namespace tftp
{

// -----------------------------------------------------------------------------

// constants
constexpr const in_port_t  default_tftp_port       = 69;
constexpr const int        default_tftp_syslog_lvl = 6;
constexpr const uint16_t   default_fb_dialect      = 3;
constexpr std::string_view default_fb_lib_name     = "libfbclient.so";

// -----------------------------------------------------------------------------

class Base;

class Srv;

class session;

class data_mgr;

class Settings;

using pSettings = std::shared_ptr<Settings>;

using Buf = std::vector<char>;

/** \bief Optional class storage with default value
 *
 *  Info: std::optional<T> does't have default value
 */
template<typename T>
using Opt = std::tuple<bool, T>;

using OptInt = Opt<int>;

// -----------------------------------------------------------------------------

/** \bief Server request enum
 */
enum class SrvReq: uint16_t
{
  unknown=0, ///< Unknown request
  read=1,    ///< Read request
  write=2,   ///< Write request
};

// -----------------------------------------------------------------------------

/** \bief Server transfer mode enum
 *
 *  Notify: "mail" mode not supported
 */
enum class TransfMode: uint16_t
{
  unknown=0,  ///< Unknown transfer mode
  netascii=1, ///< Text transfer mode
  octet=2,    ///< Octet (binary) transfer mode
  binary,     ///< Binary transfer mode (fake mode, auto change to octet)
//mail,
};

// -----------------------------------------------------------------------------

/** \bief Callabck for custom logging message from server
 *
 *  \param [in] Logging level
 *  \param [in] Message text
 */
using fLogMsg = std::function<void(const int, std::string_view)>;

// -----------------------------------------------------------------------------

template<typename... Ts>
struct is_container_helper {};

template<typename T, typename _ = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<
        T,
        std::conditional_t<
            false,
            is_container_helper<
                typename T::value_type,
                typename std::enable_if_t<std::is_trivially_copyable_v<typename T::value_type> &&
                                          !std::is_pointer_v<typename T::value_type>,
                                          void>,
                decltype(std::declval<T>().cbegin()),
                decltype(std::declval<T>().cend())
                >,
            void
            >
        > : public std::true_type {};

template<class T>
constexpr bool is_container_v = is_container<T>::value;


// -----------------------------------------------------------------------------

/** Get string with typename class T
 *  \return Typename
 */
template<typename T>
auto curr_type() -> std::string
{
  int stat{0};
  char * tmp_name = abi::__cxa_demangle(typeid(T).name(), 0, 0, & stat);
  std::string ret{tmp_name};
  if(tmp_name) free(tmp_name);
  return ret;
}

// -----------------------------------------------------------------------------

std::string_view to_string(const SrvReq);       ///< Conversion 'SrvReq' to text

std::string_view to_string(const TransfMode); ///< Conversion 'TransfMode' to text

std::string_view to_string_lvl(const int);       ///< Conversion debug level to text

// -----------------------------------------------------------------------------

/** \brief Conversion: Socket address struct to string
 *
 *  Address can be as 'struct sockaddr_in', 'struct sockaddr_in6'
 *  \param [in] addr_begin Address buffer - begin iterator
 *  \param [in] addr_end Address buffer - end iterator
 *  \return String with L3 address and port info; sample "0.0.0.0:69"
 */
std::string sockaddr_to_str(
    Buf::const_iterator addr_begin,
    Buf::const_iterator addr_end);

// -----------------------------------------------------------------------------

/// Preprocessor defines for simple logging
#define THIS_CLASS_METHOD() curr_type<std::remove_pointer_t<decltype(this)>>().append("::").append(__func__).append("()")
#define LOG(LEVEL,MSG) log(LEVEL, THIS_CLASS_METHOD() + " " + MSG);

/// Preprocessor defines for runtime error (if bug snuck)
#define ERROR_CLASS_METHOD__RUNTIME(msg) \
    {\
      std::string full_msg{curr_type<std::remove_pointer_t<decltype(this)>>()};\
      full_msg.append("::").append(__PRETTY_FUNCTION__).append(": ").append(msg);\
      this->log(LOG_ERR, full_msg);\
      throw std::runtime_error(full_msg);\
    };

} // namespace tftp

#endif /* SOURCE_TFTP_COMMON_H_ */
