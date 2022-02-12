#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <algorithm>
#include <boost/core/ignore_unused.hpp>

#include "lib.h"

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
static std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

static bool compare_ips( const std::vector<uint8_t>& lhs,
                  const std::vector<uint8_t>& rhs )
{
      if ( lhs[0] > rhs[0] ) return true;

      if ( ( lhs[0] == rhs[0] ) && 
           ( lhs[1] >  rhs[1] ) ) return true;

      if ( ( lhs[0] == rhs[0] ) && 
           ( lhs[1] == rhs[1] ) && 
           ( lhs[2] >  rhs[2] ) ) 
            return true;
      if ( ( lhs[0] == rhs[0] ) &&
           ( lhs[1] == rhs[1] ) &&
           ( lhs[2] == rhs[2] ) &&
           ( lhs[3] >  rhs[3] ) )
            return true;
      return false;            
}

static std::vector<uint8_t> convert( const std::vector<std::string>& input )
{
    std::vector<uint8_t> result;
    for( const auto& octet : input )
    {
        result.push_back( static_cast<uint8_t>( std::stoi( octet ) ) );
    }
    return result;
}

static void print_ip_addresses( const std::vector<std::vector<uint8_t> >& ip_pool )
{
    for(std::vector<std::vector<uint8_t> >::const_iterator ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
    {
        for(std::vector<uint8_t>::const_iterator ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
        {
            if (ip_part != ip->cbegin())
            {
                std::cout << ".";

            }
            std::cout << (int)*ip_part;
        }
        std::cout << std::endl;
    }
}

static std::vector<std::vector<uint8_t>> filter_any( const std::vector<std::vector<uint8_t>>& ip_pool, const uint8_t value )
{
    std::vector<std::vector<uint8_t>> filtered;
    copy_if( ip_pool.begin(), ip_pool.end(), back_inserter( filtered ),[value]( const std::vector<uint8_t>& addr ) {
        return std::any_of( addr.begin(), addr.end(), [value]( const uint8_t octet ) { return octet == value; } );
           });
    return filtered;
}

int main( int argc, char const *argv[] )
{
    boost::ignore_unused( argc );
    boost::ignore_unused( argv );
    try
    {
        std::vector<std::vector<uint8_t> > ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
            ip_pool.push_back( convert( split(v.at(0), '.') ) );
        }

        // reverse lexicographically sort
        std::sort( ip_pool.begin(), ip_pool.end(), []( const std::vector<uint8_t> lhs, const std::vector<uint8_t> rhs ) {
            return compare_ips( lhs, rhs );
        });
        print_ip_addresses( ip_pool );

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // filter by first byte and output
        // ip = filter(1)
        std::vector<std::vector<uint8_t> > res1;
        copy_if( ip_pool.begin(), ip_pool.end(), back_inserter(res1),[]( const std::vector<uint8_t>& addr ) { return addr[0] == 1; });
        print_ip_addresses( res1 );

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // filter by first and second bytes and output
        // ip = filter(46, 70)
        std::vector<std::vector<uint8_t>> res2;
        copy_if( ip_pool.begin(), ip_pool.end(), back_inserter(res2),[]( const std::vector<uint8_t>& addr ) {
            return ( addr[0] == 46 ) &&
                   ( addr[1] == 70 ); });
        print_ip_addresses( res2 );

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // filter by any byte and output
        // ip = filter_any(46)
        std::vector<std::vector<uint8_t>> res3 = filter_any( ip_pool, 46 );
        print_ip_addresses( res3 );

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
