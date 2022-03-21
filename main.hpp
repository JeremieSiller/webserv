#ifndef main_hpp
# define main_hpp

# pragma once

# define LOG_RED(x) (std::cout << __FILE__ << ":" << __LINE__ << "\t\033[1;31m" << x << "\033[0m" << std::endl)
# define LOG_GREEN(x) (std::cout << __FILE__ << ":" << __LINE__ << "\t\033[1;32m" << x << "\033[0m" << std::endl)
# define LOG_YELLOW(x) (std::cout << __FILE__ << ":" << __LINE__ << "\t\033[1;33m" << x << "\033[0m" << std::endl)
# define LOG_BLUE(x) (std::cout << __FILE__ << ":" << __LINE__ << "\t\033[1;34m" << x << "\033[0m" << std::endl)

#endif
