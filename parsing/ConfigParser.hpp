#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <vector>
#include <exception>
#include "ConfigToken.hpp"

class ConfigParser
{
private:
    std::vector<ConfigToken>    &_tokens;
    void    _removeCommentsAndLineBreaks();
    void    _setScopes();
    void    _checkScopes() const;
    void    _checkSyntax();
    void    _checkKeywords();
/*  execptions */
private:
    class   ScopeNotClosed;
    class   isInWrongScope;
    class   syntaxError;
public:
    ConfigParser(std::vector<ConfigToken> &tokens);
    ~ConfigParser();
};
  

#endif