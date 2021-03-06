#ifndef SHINY_LOG_H
#define SHINY_LOG_H

#include <boxer/boxer.h>
#include <logging.h>

#include <cstring>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

namespace {

// Severity names
const char* kSevDebug = "Debug";
const char* kSevInfo = "Information";
const char* kSevMessage = "Message";
const char* kSevWarning = "Warning";
const char* kSevError = "Error";
const char* kSevFatal = "Fatal Error";

boxer::Style boxerStyle(const std::string &sev) {
   if (sev == kSevDebug || sev == kSevInfo || sev == kSevMessage) {
      return boxer::Style::Info;
   }
   if (sev == kSevWarning) {
      return boxer::Style::Warning;
   }
   return boxer::Style::Error;
}

// Width of the severity tag in log output (e.g. [ warning ])
const int kSevNameWidth = 9;

// Delimiter for splitting the title and message of logs to be written by the boxer_write_policy
const std::string kBoxerMessageSplit = "|";

// Used for centering content in the log output
std::string center(std::string input, int width) { 
   return std::string((width - input.length()) / 2, ' ') + input + std::string((width - input.length() + 1) / 2, ' ');
}

// Format time to a string (hh:mm:ss)
std::string formatTime(const tm &time) {
   std::stringstream ss;

   ss << std::setfill('0')
      << std::setw(2) << time.tm_hour << ':'
      << std::setw(2) << time.tm_min << ':'
      << std::setw(2) << time.tm_sec;

   return ss.str();
}

tm getCurrentTime() {
   time_t now(std::time(nullptr));
   tm time;

#if defined(_POSIX_VERSION)
   localtime_r(&now, &time);
#elif defined(_MSC_VER)
   localtime_s(&time, &now);
#else
   time = *localtime(&now);
#endif

   return time;
}

// Extract file name from a path
#define SHINY_FILENAME(_shiny_path_) (strrchr(_shiny_path_, '/') ? strrchr(_shiny_path_, '/') + 1 :\
                                     (strrchr(_shiny_path_, '\\') ? strrchr(_shiny_path_, '\\') + 1 : _shiny_path_))

// Text formatting policy
class text_formating_policy : public templog::formating_policy_base<text_formating_policy> {
public:
   template< class WritePolicy_, int Sev_, int Aud_, class WriteToken_, class ParamList_ >
   static void write(WriteToken_& token, TEMPLOG_SOURCE_SIGN, const ParamList_& parameters) {
#if SHINY_LOG_MSVC_STYLE
      write_obj<WritePolicy_>(token, TEMPLOG_SOURCE_FILE);
      write_obj<WritePolicy_>(token, "(");
      write_obj<WritePolicy_>(token, TEMPLOG_SOURCE_LINE);
      write_obj<WritePolicy_>(token, "): ");
      write_obj<WritePolicy_>(token, "[");
      write_obj<WritePolicy_>(token, center(get_name(static_cast<templog::severity>(Sev_)), kSevNameWidth));
      write_obj<WritePolicy_>(token, "] <");
      write_obj<WritePolicy_>(token, formatTime(getCurrentTime()));
      write_obj<WritePolicy_>(token, "> ");
#else // SHINY_LOG_MSVC_STYLE
      write_obj<WritePolicy_>(token, "[");
      write_obj<WritePolicy_>(token, center(get_name(static_cast<templog::severity>(Sev_)), kSevNameWidth));
      write_obj<WritePolicy_>(token, "] <");
      write_obj<WritePolicy_>(token, formatTime(getCurrentTime()));
      write_obj<WritePolicy_>(token, "> ");
      write_obj<WritePolicy_>(token, SHINY_FILENAME(TEMPLOG_SOURCE_FILE));
      write_obj<WritePolicy_>(token, "(");
      write_obj<WritePolicy_>(token, TEMPLOG_SOURCE_LINE);
      write_obj<WritePolicy_>(token, "): ");
#endif // SHINY_LOG_MSVC_STYLE

      write_params<WritePolicy_>(token, parameters);
   }
};

// Message box formatting policy
class boxer_formating_policy : public templog::formating_policy_base<boxer_formating_policy> {
public:
   template< class WritePolicy_, int Sev_, int Aud_, class WriteToken_, class ParamList_ >
   static void write(WriteToken_& token, TEMPLOG_SOURCE_SIGN_IGNORE, const ParamList_& parameters) {
      write_params<WritePolicy_>(token, parameters);
   }
};

// Message box writing policy
class boxer_write_policy : public templog::non_incremental_write_policy_base<boxer_write_policy,false> {
public:
   template< int Sev_, int Aud_ >
   struct writes { enum { result = true }; };

   static bool is_writing(int /*sev*/, int /*aud*/){return true;}

   static void write_str(const std::string& str) {
      size_t split = str.find(kBoxerMessageSplit);

      std::string sevName = kSevError;
      std::string message = str;
      // Should always be found, but in some off chance that it isn't, we don't want to break here
      if (split != std::string::npos) {
         sevName = str.substr(0, split);
         message = str.substr(split + kBoxerMessageSplit.size());
      }

      boxer::show(message.c_str(), sevName.c_str(), boxerStyle(sevName));
   }
};

// Prevent text logging in release builds
#define SHINY_CERR_SEV_THRESHOLD templog::sev_fatal + 1

// Prevent messages boxes lower than error in release builds
#define SHINY_MSG_BOX_SEV_THRESHOLD templog::sev_error

#if SHINY_DEBUG
#  undef SHINY_CERR_SEV_THRESHOLD
#  define SHINY_CERR_SEV_THRESHOLD templog::sev_debug

#  undef SHINY_MSG_BOX_SEV_THRESHOLD
#  define SHINY_MSG_BOX_SEV_THRESHOLD templog::sev_debug
#endif // SHINY_DEBUG

// Allow internal logging to be disabled
enum LogAudience {
   kInternal = 1,
   kExternal,
   kUser
};

#if SHINY_LOG_INTERNAL
#  define SHINY_LOGGER_AUDIENCE_LIST kInternal, kExternal, kUser
#else
#  define SHINY_LOGGER_AUDIENCE_LIST kExternal, kUser
#endif // SHINY_LOG_INTERNAL

#if SHINY_BUILDING
#  define SHINY_LOG_AUDIENCE kInternal
#else
#  define SHINY_LOG_AUDIENCE kExternal
#endif // SHINY_BUILDING

typedef templog::logger<templog::non_filtering_logger<text_formating_policy, templog::std_write_policy>
                      , SHINY_CERR_SEV_THRESHOLD
                      , templog::audience_list<SHINY_LOGGER_AUDIENCE_LIST>>
                      cerr_logger;

typedef templog::logger<templog::non_filtering_logger<boxer_formating_policy, boxer_write_policy>
                      , SHINY_MSG_BOX_SEV_THRESHOLD
                      , templog::audience_list<kUser>> // Only show message boxes when the user should be notified
                      boxer_logger;

} // namespace

// Simplify logging calls

// Debug   : Used to check values, locations, etc. (sort of a replacement for printf)
// Info    : For logging interesting, but expected, information
// Message : For logging more detailed informational messages
// Warning : For logging information of concern, which may cause issues
// Error   : For logging errors that do not prevent the program from continuing
// Fatal   : For logging fatal errors that prevent the program from continuing

#define SHINY_LOG(_log_message_, _log_title_, _log_severity_) \
do { \
   TEMPLOG_LOG(cerr_logger, _log_severity_, SHINY_LOG_AUDIENCE) << _log_message_; \
} while (0)

#define LOG_DEBUG(_log_message_) SHINY_LOG(_log_message_, kSevDebug, templog::sev_debug)
#define LOG_INFO(_log_message_) SHINY_LOG(_log_message_, kSevInfo, templog::sev_info)
#define LOG_MESSAGE(_log_message_) SHINY_LOG(_log_message_, kSevMessage, templog::sev_message)
#define LOG_WARNING(_log_message_) SHINY_LOG(_log_message_, kSevWarning, templog::sev_warning)
#define LOG_ERROR(_log_message_) SHINY_LOG(_log_message_, kSevError, templog::sev_error)

// Don't use message boxes or FATAL internally
#if !SHINY_BUILDING

// Easy calls to create message boxes

#  define SHINY_LOG_MSG_BOX(_log_message_, _log_title_, _log_severity_) \
do { \
   TEMPLOG_LOG(boxer_logger, _log_severity_, kUser) << _log_title_ << kBoxerMessageSplit << _log_message_; \
} while (0)

#  define LOG_DEBUG_MSG_BOX(_log_message_) SHINY_LOG_MSG_BOX(_log_message_, kSevDebug, templog::sev_debug)
#  define LOG_INFO_MSG_BOX(_log_message_) SHINY_LOG_MSG_BOX(_log_message_, kSevInfo, templog::sev_info)
#  define LOG_MESSAGE_MSG_BOX(_log_message_) SHINY_LOG_MSG_BOX(_log_message_, kSevMessage, templog::sev_message)
#  define LOG_WARNING_MSG_BOX(_log_message_) SHINY_LOG_MSG_BOX(_log_message_, kSevWarning, templog::sev_warning)
#  define LOG_ERROR_MSG_BOX(_log_message_) SHINY_LOG_MSG_BOX(_log_message_, kSevError, templog::sev_error)

// Fatal errors (kill the program, even in release)

#  define LOG_FATAL(_log_message_) \
do { \
   SHINY_LOG(_log_message_, kSevFatal, templog::sev_fatal); \
   abort(); \
} while(0)
#  define LOG_FATAL_MSG_BOX(_log_message_) \
do { \
   SHINY_LOG_MSG_BOX(_log_message_, kSevFatal, templog::sev_fatal); \
   abort(); \
} while(0)

#endif // !SHINY_BUILDING

#endif
