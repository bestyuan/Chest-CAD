#include "IcdUtils.h"

// IcdUtils : general-purpose utilities for ICD
//
// crandall, 4/2003
//

//
// basename: like Unix's basename. (not available under NT, so had to implement one here)
//
// path_name : input path name, possibly with directory path included
// erase_extension : if true, erase the file name extension (the part after the last '.')
// erase_path : if true, erase the file name path 
//
// return the result.
//
std::string IcdUtils::basename(const std::string &path_name, bool erase_extension, bool erase_path) const
{
	std::string result = path_name;

	if(erase_path)
	{
		int ind1 = path_name.rfind('/');
		int ind2 = path_name.rfind('\\');
		int ind = ind1 > ind2 ? ind1 : ind2;

		if(ind == -1)
			result = path_name;
		else
			result = path_name.substr(ind+1);
	}

	if(erase_extension)
	{
		int ind1 = result.rfind('.');
		result = result.substr(0, ind1);
	}

	return result;
}


// rint : round like java.
//
// Round a double towards nearest integer, favoring even integer if halfway.
// doing this to duplicate semantics of Java's rint function.
//
// There's probably a standard library call to do this, but I couldn't find one
// available under both NT and Unix.
//
int IcdUtils::rint(double r) const
{
    if(r - int(r) > 0.5)
        return(int(r) + 1);
    else if(r - int(r) < 0.5)
        return(int(r));
    else
    {
        if(int(r) % 2 == 0)
            return int(r);
        else 
            return int(r) + 1;
    }
}
