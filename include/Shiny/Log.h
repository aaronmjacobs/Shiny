#ifndef SHINY_LOG_H
#define SHINY_LOG_H

#include "Shiny/Defines.h"

#include <boxer/boxer.h>
#include <templog/logging.h>

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

// Format time to a string (mm-dd-yyyy hh:mm:ss)
std::string formatTime(struct tm *tm) {
   std::stringstream ss;

   ss << std::setfill('0')
      /*<< std::setw(2) << tm->tm_mon + 1 << '-'
      << std::setw(2) << tm->tm_mday << '-'
      << std::setw(4) << tm->tm_year + 1900 << ' '*/
      << std::setw(2) << tm->tm_hour << ':'
      << std::setw(2) << tm->tm_min << ':'
      << std::setw(2) << tm->tm_sec;

   return ss.str();
}

// Format time to a string (mm-dd-yyyy-hh)
std::string formatTimeForFilename(struct tm *tm) {
   std::stringstream ss;

   ss << std::setfill('0')
   << std::setw(2) << tm->tm_mon + 1 << '-'
   << std::setw(2) << tm->tm_mday << '-'
   << std::setw(4) << tm->tm_year + 1900 << '-'
   << std::setw(2) << tm->tm_hour;

   return ss.str();
}

// Extract file name from a path
#define SHINY_FILENAME(_shiny_path_) (strrchr(_shiny_path_, '/') ? strrchr(_shiny_path_, '/') + 1 :\
                                     (strrchr(_shiny_path_, '\\') ? strrchr(_shiny_path_, '\\') + 1 : _shiny_path_))

// Text formatting policy (console, file, etc.)
class text_formating_policy : public templog::formating_policy_base<text_formating_policy> {
public:
   template< class WritePolicy_, int Sev_, int Aud_, class WriteToken_, class ParamList_ >
   static void write(WriteToken_& token, TEMPLOG_SOURCE_SIGN, const ParamList_& parameters) {
      auto t = std::time(nullptr);
      auto tm = std::localtime(&t);

      write_obj<WritePolicy_>(token, "[");
      write_obj<WritePolicy_>(token, center(get_name(static_cast<templog::severity>(Sev_)), kSevNameWidth));
      write_obj<WritePolicy_>(token, "] <");
      write_obj<WritePolicy_>(token, formatTime(tm));
      write_obj<WritePolicy_>(token, "> (");
      write_obj<WritePolicy_>(token, SHINY_FILENAME(TEMPLOG_SOURCE_FILE));
      write_obj<WritePolicy_>(token, ": ");
      write_obj<WritePolicy_>(token, TEMPLOG_SOURCE_LINE);
      write_obj<WritePolicy_>(token, ") ");
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

// File writing policy
class file_write_policy : public templog::non_incremental_write_policy_base<file_write_policy,true> {
public:
   template< int Sev_, int Aud_ >
   struct writes { enum { result = true }; };

   static bool is_writing(int /*sev*/, int /*aud*/){return true;}

   static void write_str(const std::string& str) {
      auto t = std::time(nullptr);
      auto tm = std::localtime(&t);

      std::ofstream out(formatTimeForFilename(tm) + ".log", std::ios_base::app);
      if (out) {
         out << str;
      }
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
#define CERR_SEV_THRESHOLD templog::sev_fatal + 1
#define FILE_SEV_THRESHOLD templog::sev_fatal + 1

// Prevent messages boxes lower than error in release builds
#define MSG_BOX_SEV_THRESHOLD templog::sev_error

#if !defined(NDEBUG)
#  undef CERR_SEV_THRESHOLD
#  define CERR_SEV_THRESHOLD templog::sev_debug

#  undef MSG_BOX_SEV_THRESHOLD
#  define MSG_BOX_SEV_THRESHOLD templog::sev_debug

#  if defined(SHINY_LOG_TO_FILE)
#     undef FILE_SEV_THRESHOLD
#     define FILE_SEV_THRESHOLD templog::sev_info
#  endif // defined(SHINY_LOG_TO_FILE)
#endif // !defined(NDEBUG)

// Allow internal logging to be disabled
enum LogAudience {
   kInternal = 1,
   kExternal,
   kUser
};

#if defined(SHINY_LOG_INTERNAL)
#  define SHINY_LOGGER_AUDIENCE_LIST kInternal, kExternal, kUser
#else
#  define SHINY_LOGGER_AUDIENCE_LIST kExternal, kUser
#endif // defined(SHINY_LOG_INTERNAL)

#if defined(SHINY_BUILDING)
#  define SHINY_LOG_AUDIENCE kInternal
#else
#  define SHINY_LOG_AUDIENCE kExternal
#endif // defined(SHINY_BUILDING)

typedef templog::logger<templog::non_filtering_logger<text_formating_policy, templog::std_write_policy>
                      , CERR_SEV_THRESHOLD
                      , templog::audience_list<SHINY_LOGGER_AUDIENCE_LIST>>
                      cerr_logger;

typedef templog::logger<templog::non_filtering_logger<text_formating_policy, file_write_policy>
                      , FILE_SEV_THRESHOLD
                      , templog::audience_list<SHINY_LOGGER_AUDIENCE_LIST>>
                      file_logger;

typedef templog::logger<templog::non_filtering_logger<boxer_formating_policy, boxer_write_policy>
                      , MSG_BOX_SEV_THRESHOLD
                      , templog::audience_list<kUser>> // Only show message boxes when the user should be notified
                      boxer_logger;

} // namespace

// Send the log to each of our loggers (and let them do the filtering)
#define LOG(_log_message_, _log_title_, _log_severity_, _log_audience_) \
do { \
   TEMPLOG_LOG(cerr_logger, _log_severity_, _log_audience_) << _log_message_; \
   TEMPLOG_LOG(file_logger, _log_severity_, _log_audience_) << _log_message_; \
   TEMPLOG_LOG(boxer_logger, _log_severity_, _log_audience_) << _log_title_ << kBoxerMessageSplit << _log_message_; \
} while (0)

// Simplify logging calls

// Debug   : Used to check values, locations, etc. (sort of a replacement for printf)
// Info    : For logging interesting, but expected, information
// Message : For logging more detailed informational messages
// Warning : For logging information of concern, which may cause issues
// Error   : For logging errors that do not prevent the program from continuing
// Fatal   : For logging fatal errors that prevent the program from continuing

#define LOG_DEBUG(_log_message_) LOG(_log_message_, kSevDebug, templog::sev_debug, SHINY_LOG_AUDIENCE)
#define LOG_INFO(_log_message_) LOG(_log_message_, kSevInfo, templog::sev_info, SHINY_LOG_AUDIENCE)
#define LOG_MESSAGE(_log_message_) LOG(_log_message_, kSevMessage, templog::sev_message, SHINY_LOG_AUDIENCE)
#define LOG_WARNING(_log_message_) LOG(_log_message_, kSevWarning, templog::sev_warning, SHINY_LOG_AUDIENCE)
#define LOG_ERROR(_log_message_) LOG(_log_message_, kSevError, templog::sev_error, SHINY_LOG_AUDIENCE)

// Don't use message boxes or FATAL internally
#if !defined(SHINY_BUILDING)

// Easy calls to create message boxes

#  define LOG_DEBUG_MSG_BOX(_log_message_) LOG(_log_message_, kSevDebug, templog::sev_debug, kUser)
#  define LOG_INFO_MSG_BOX(_log_message_) LOG(_log_message_, kSevInfo, templog::sev_info, kUser)
#  define LOG_MESSAGE_MSG_BOX(_log_message_) LOG(_log_message_, kSevMessage, templog::sev_message, kUser)
#  define LOG_WARNING_MSG_BOX(_log_message_) LOG(_log_message_, kSevWarning, templog::sev_warning, kUser)
#  define LOG_ERROR_MSG_BOX(_log_message_) LOG(_log_message_, kSevError, templog::sev_error, kUser)

// Fatal errors (kill the program, even in release)

#  define LOG_FATAL(_log_message_) \
do { \
   LOG(_log_message_, kSevFatal, templog::sev_fatal, SHINY_LOG_AUDIENCE); \
   abort(); \
} while(0)
#  define LOG_FATAL_MSG_BOX(_log_message_) \
do { \
   LOG(_log_message_, kSevFatal, templog::sev_fatal, kUser); \
   abort(); \
} while(0)

#endif // !defined(SHINY_BUILDING)

#endif
