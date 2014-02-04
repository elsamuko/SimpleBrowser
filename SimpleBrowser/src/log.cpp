
#include "log.h"
#include "datetime.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <thread>
#include <mutex>
#include <cstdlib>

static std::mutex mutex;

std::string threadId() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str().substr( 4, 4 );
}

std::string indent( const std::string& s, std::streamoff count ) {
    std::stringstream ret;
    std::stringstream ss( s );
    std::string item;
    std::string whitespaces( ( int ) count, ' ' );

    if( std::getline( ss, item ) ) {
        ret << item << std::endl;

        while( std::getline( ss, item ) ) {
            ret << whitespaces << item << std::endl;
        }
    }

    return ret.str();
}


bool writeLog( const char* level, const char* /*file*/, int sourceline, const char* function, std::string content ) {

    std::stringstream logline;

    // time
    logline.width( 6 );
    logline << datetime::now();
    logline << level;
    // logline << " T" << threadId();

    logline.width( 30 );
    logline << function << "(";
    logline.width( 4 );
    logline.setf( std::ios::right );
    logline << sourceline << "): ";

    // indent paragraph by current size
    logline.seekg( 0, std::ios::end );
    logline << indent( content, logline.tellg() );

    std::lock_guard<std::mutex> lock( mutex );

    std::cout << logline.str();

    std::string temp = getenv( "temp" );
    std::ofstream ofs( temp + "/SimpleBrowser.log", std::ios::out | std::ios::app );

    if( ofs.is_open() ) {
        ofs << logline.str();
        return true;
    } else {
        return false;
    }
}

