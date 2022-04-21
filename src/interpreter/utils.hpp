#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <dirent.h>

std::string	buildDirectoryListing(std::string const &dir, std::string const &abs_path) {
	std::string ret = "";
	ret += "<html>\n";
	ret += "<head><title>Index of "+ abs_path + "</title></head>\n";
	ret += "<body>\n";
	ret += "<h1>Index of " + abs_path + "</h1><hr><pre><a href=\"../\">../</a>\n";
	DIR *d;
	struct dirent *dd;
	d = opendir(dir.c_str());
	if (d) {
		while ((dd = readdir(d)) != NULL)
		{
			if (dd->d_name[0] != '.') {
				ret += "<a href=\"" + abs_path + "/" + dd->d_name;
				if (dd->d_type == DT_DIR)
					ret += "/";
				ret += "\">" + std::string(dd->d_name);
				if (dd->d_type == DT_DIR)
					ret += "/";
				ret += "</a>\n";
			}
		}
		closedir(d);
	}
	else {
		return "";
	}
	ret += "</body>\n";
	ret += "</html>";
	return ret;
}

std::vector<std::string>	split_string(const std::string &s, const char &c) {
	std::stringstream ss;
	std::string	segment;

	ss << s;
	std::vector<std::string> seglist;
	while  (std::getline(ss, segment, c)) {
		if (segment != "")
			seglist.push_back(segment);
	}
	return seglist;
}

int		count_continues_matches(const std::vector<std::string> &path, const std::vector<std::string> &uri) {
	std::vector<std::string>::const_iterator	pit = path.begin();
	std::vector<std::string>::const_iterator	uit = uri.begin();
	size_t	count = 0;
	while (pit != path.end() && uit != uri.end()) {
		if (*pit != *uit)
			break;
		pit++;
		uit++;
		count++;
	}
	if (pit == path.begin() && path.empty() == false)
		return -1;
	return count;
}

inline bool ends_with(std::string const & value, std::string const & ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

inline ssize_t	get_file_size(FILE *fp) {
	if (!fp)
		return (-1);
	fseek(fp, 0L, SEEK_END);
	size_t ret = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return ret;
}

#endif