
// IcdUtils : general-purpose utilities for ICD
//
// crandall, 4/2003
//

#include<string>

class IcdUtils
{
    public:
          // returns the base file name of another (possibly fully-qualified) file name
        std::string basename(const std::string &path_name, bool erase_extension=false, bool erase_path=true) const;

          // rounds, using the same convention as java (for replication purposes)
        int rint(double r) const;

};
