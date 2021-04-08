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

#include <cxxabi.h> // for current class/type name
#include <functional>
#include <vector>
#include <memory>


namespace tftp
{

// -----------------------------------------------------------------------------

// constants
constexpr uint16_t    default_tftp_port        = 69;
constexpr int         default_tftp_syslog_lvl  = 6;
constexpr uint16_t    default_fb_dialect       = 3;
constexpr std::string_view default_fb_lib_name = "libfbclient.so";

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

/** \brief Logging level (for logging messages)
 */
enum class LogLvl: int
{
  emerg   = 0, // LOG_EMERG 0   // system is unusable
  alert   = 1, // LOG_ALERT 1   // action must be taken immediately
  crit    = 2, // LOG_CRIT  2   // critical conditions
  err     = 3, // LOG_ERR   3   // error conditions
  warning = 4, // LOG_WARNING 4 // warning conditions
  notice  = 5, // LOG_NOTICE  5 // normal but significant condition
  info    = 6, // LOG_INFO  6   // informational
  debug   = 7, // LOG_DEBUG 7   // debug-level messages
};

// -----------------------------------------------------------------------------

/** \bief Callabck for custom logging message from server
 *
 *  \param [in] Logging level
 *  \param [in] Message text
 */
using fLogMsg = std::function<void(const LogLvl, std::string_view)>;

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
  if(!tmp_name) return "???";
  std::string ret{tmp_name};
  free(tmp_name);
  return ret;
}

// -----------------------------------------------------------------------------

#define CASE_OPER_TO_STR_VIEW(NAME) \
    case std::decay_t<decltype(val)>::NAME: return #NAME;

/** \brief Convert value of 'Request Type' to text string
 *
 *  \param [in] val Source argument
 *  \return String
 */
constexpr auto to_string(const SrvReq & val) -> std::string_view
{
  switch(val)
  {
    CASE_OPER_TO_STR_VIEW(unknown);
    CASE_OPER_TO_STR_VIEW(read);
    CASE_OPER_TO_STR_VIEW(write);
    default: return "UNK_SRV_REQ";
  }
}

/** \brief Convert value of 'Transfer Mode' to text string
 *
 *  \param [in] val Source argument
 *  \return String
 */
constexpr auto to_string(const TransfMode & val) -> std::string_view
{
  switch(val)
  {
    CASE_OPER_TO_STR_VIEW(unknown);
    CASE_OPER_TO_STR_VIEW(netascii);
    CASE_OPER_TO_STR_VIEW(octet);
    CASE_OPER_TO_STR_VIEW(binary);
    default: return "UNK_TRANSF_MODE";
  }
}

/** \brief Convert value of 'TLogging Level' to text string
 *
 *  \param [in] val Source argument
 *  \return String
 */
constexpr auto to_string(const LogLvl & val) -> std::string_view
{
  switch(val)
  {
    CASE_OPER_TO_STR_VIEW(emerg);
    CASE_OPER_TO_STR_VIEW(alert);
    CASE_OPER_TO_STR_VIEW(crit);
    CASE_OPER_TO_STR_VIEW(err);
    CASE_OPER_TO_STR_VIEW(warning);
    CASE_OPER_TO_STR_VIEW(notice);
    CASE_OPER_TO_STR_VIEW(info);
    CASE_OPER_TO_STR_VIEW(debug);
    default: return "UNK_LOG_LVL";
  }
}

#undef CASE_OPER_TO_STR_VIEW

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

#define LOG(LEVEL,MSG) log(LogLvl::LEVEL, THIS_CLASS_METHOD() + " " + MSG);

#define L_DBG(MSG) LOG(debug,   MSG);
#define L_INF(MSG) LOG(info,    MSG);
#define L_NTC(MSG) LOG(notice,  MSG);
#define L_WRN(MSG) LOG(warning, MSG);
#define L_ERR(MSG) LOG(err,     MSG);

/// Preprocessor defines for runtime error (if bug snuck)
#define ERROR_CLASS_METHOD__RUNTIME(msg) \
    {\
      std::string full_msg{curr_type<std::remove_pointer_t<decltype(this)>>()};\
      full_msg.append("::").append(__PRETTY_FUNCTION__).append(": ").append(msg);\
      this->log(LogLvl::err, full_msg);\
      throw std::runtime_error(full_msg);\
    };

} // namespace tftp

#endif /* SOURCE_TFTP_COMMON_H_ */