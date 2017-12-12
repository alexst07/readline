#ifndef READLINE_LOG_DEBUG_H
#define READLINE_LOG_DEBUG_H

#include <fstream>
#include <string>
#include <functional>

namespace readline {

#define LOG Log::Instance()

class Log {
 public:
   static Log& Instance(const std::string fname = "") {
     if (!instance_) {
       instance_ = new Log(fname);
     }

     return *instance_;
   }

  ~Log() {
#ifdef DEBUGLOG
    logfile_.close();
#endif
  }

  template<class T>
  Log& operator<< (const T& val) {
#ifdef DEBUGLOG
    logfile_ << val << std::flush;
#endif
    return *this;
  }

 private:
  Log(const std::string fname) {
#ifdef DEBUGLOG
   logfile_.open(fname);
#endif
  }

  static Log* instance_;
  std::ofstream logfile_;
};

}  // namespace readline

#endif  // READLINE_LOG_DEBUG_H
