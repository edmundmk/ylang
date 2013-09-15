//
// source_code.h
//
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#ifndef SOURCE_CODE_H
#define SOURCE_CODE_H


#include <stdlib.h>
#include <string>
#include <deque>



struct token
{
    token( int kind, size_t offset, size_t length );

    int			kind;
    size_t		offset;
	size_t		length;
};


struct source_chunk
{
    source_chunk( size_t offset, size_t length, const char* text );

    size_t		offset;
    size_t		length;
    const char*	text;
};


class source_code
{
public:

	explicit source_code( const char* path );
    ~source_code();

	const char* 	spelling( const token& token );
	std::string 	diagnostic( size_t offset, const char* format, ... );

	source_chunk    next_chunk();
	void			parsed_token( const token& token );
	void			parsed_newline( size_t offset );


private:

	std::string filename;
	char* 		text;
	size_t		length;
	size_t		offset;

	std::deque< size_t > newlines;


};



#endif /* SOURCE_CODE_H */











