#include <boost/core/lightweight_test.hpp>

// Example from the lightweight_test doc page


int sqr( int x )
{
    return x * x;
}


int main()
{
    BOOST_TEST( sqr(2) == 4 );
    BOOST_TEST_EQ( sqr(-3), 9 );

    return boost::report_errors();
}
