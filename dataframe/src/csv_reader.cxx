//-----------------------------------------------------------------------------
//  csv_reader.cxx:
//  Implementation for CSV manip, and some string utilities
//  Author: Luke de Oliveira (luke.deoliveira@yale.edu)
//-----------------------------------------------------------------------------

#include "include/csv_reader.hh"
namespace agile
{

//----------------------------------------------------------------------------
std::istream& operator >> ( std::istream& ins, record_t& record )
{   
    record.clear();
    
    std::string line;
    std::getline( ins, line );
    std::stringstream ss( line );
    std::string field;
    while (getline( ss, field, ',' ))
    {     
        std::stringstream fs( field );
        double f = 0.0;  
        fs >> f;   
        record.push_back( f );
    }
    return ins;
}
//----------------------------------------------------------------------------
std::istream& operator >> ( std::istream& ins, data_t& data )
{
    data.clear();

    record_t record;
    while (ins >> record)
    {
        data.push_back( record );
    }
    return ins;  
}
//----------------------------------------------------------------------------
std::string trim(const std::string& str, const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
    {
        return ""; // no content
    }      
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}
//----------------------------------------------------------------------------

void get_data_fast(std::string& path, data_t& data, bool skipfirst)
{

	data.clear();

	FILE *f = fopen(path.c_str(), "r");
	
	size_t size = 1024;
	char *lineptr = (char*)std::malloc(sizeof(char) * size);
	char *cptr;

	if (skipfirst)
		getline(&lineptr, &size, f);

	while(getline(&lineptr, &size, f) > -1) {
		record_t r;
		cptr = lineptr;
		do {
			double num;
			sscanf(++cptr, "%lf", &num);
			r.push_back(num);
			cptr = std::strchr(cptr, ',');
		} while (cptr);

		data.push_back(std::move(r));
	}
	fclose(f);
}

}

