#ifndef CONFIGTOKEN_HPP
# define CONFIGTOKEN_HPP

# include "AToken.hpp"
# include <cctype>


class ConfigToken : public AToken
{
public:
	typedef enum {
		SCOPE_START,
		SCOPE_END,
		COMMENT,
		NEW_LINE,
		CONNECTION,
		SERVER,
		LOCATION,
		ROOT,
		ERROR_PAGE,
		MAX_BODY_SIZE,
		METHOD,
		GET,
		POST,
		DELTE,
		PUT,
		HEAD,
		INDEX,
		UPLOAD_ENABLE,
		UPLOAD_PATH,
		ON,
		OFF,
		LISTEN,
		CGI_EXTENSION,
		CGI_PATH,
		CGI_METHOD,
		REDIRECT,
		SERVER_NAME,
		AUTO_INDEX,
		DOUBLE_DOT,
		EOF_INSTRUCT,
		IP_ADDRESS,
		INTEGER,
		PATH,
		STRING
	} instrutions;
private:
	static const std::string	_seperators;
	size_t						_scope;
public:
	ConfigToken(std::string const &content);
	size_t						getPos() const;
	static const std::string	identity[];
	void						classify();
	size_t						isSeperator(std::string const &content) const;
	size_t						getSpecialLength(std::string const &content) const;
	void						setType(instrutions const &val);
	const size_t				&scope()	const;
	void						setScope(size_t const &scope);
};

#endif