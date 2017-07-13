#include <sstream>


template <typename InputIterator>
std::string join_strings(InputIterator begin,
						InputIterator end, 
						const std::string& separator=", ",
						const std::string& initiator="",
						const std::string& concluder="")
{
	std::ostringstream ss;

	ss << initiator; 
	
    if(begin != end)
    {
        ss << *begin++; // see 3.
    }    

    while(begin != end) // see 3.
    {
        ss << separator;
        ss << *begin++;
    }

    ss << concluder;
    return ss.str();
}