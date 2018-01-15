#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <yvals.h>
#include <boost/coroutine2/all.hpp>




#if 0
struct resumable_thing {
    struct promise_type {
        resumable_thing get_return_object() {
            return resumable_thing( coroutine_handle< promise_type >::from_promise( this ) );
        }
        auto initial_suspend() { return suspend_never{}; }
        auto final_suspend() { return suspend_never{}; }
        void return_void() { }
    };

    coroutine_handle< promise_type >  _coroutine = nullptr;

    explicit resumable_thing( coroutine_handle< promise_type >  coroutine ): _coroutine( coroutine ) {
    }
    resumable_thing() = default;
    resumable_thing( resumable_thing const& ) = delete;
    resumable_thing& operator=( resumable_thing const& ) = delete;
    resumable_thing( resumable_thing&& other ) : _coroutine( other._coroutine ) {
        other._coroutine = nullptr;
    }
    resumable_thing& operator=( resumable_thing&& other ) {
        if ( &other != this ) {
            _coroutine = other._coroutine;
            other._coroutine = nullptr;
        }
    }

    ~resumable_thing() {
        if ( _coroutine ) {
            _coroutine.destroy();
        }
    }

    void resume() const { _coroutine.resume(); }
}




resumable_thing
counter() {
    cout << "counter: called\n";
    for ( unsigned int i = 1;; ++i ) {
        co_await suspend_always{};
        cout << "counter: resumed (#" << i << ")\n";
    }
}




int main() {

    cout << "main: calling counter\n";
    resumable_thing the_counter = counter();
    cout << "main: resuming counter\n";
    the_counter.resume();
    the_counter.resume();
    cout << "main: done\n";
}
#endif




// \see http://boost.org/doc/libs/1_65_1/libs/coroutine2/doc/html/coroutine2/coroutine/asymmetric.html
int
main() {

    // pull
#if 1
{
    std::cout << "\nStart Pull" << std::endl;
    typedef boost::coroutines2::coroutine< unsigned long long >  coro_t;
    coro_t::pull_type  source(
        [] ( coro_t::push_type& sink ) {
            int  first = 1;
			sink( first );
			int  second = 1;
            sink( second );
            for ( int i = 0; i < 10; ++i ) {
                const int  third = first + second;
                first = second;
                second = third;
                sink( third );
            }
    } );

    for ( auto i : source ) {
        std::cout << i << " ";
    }
    std::cout << "\nDone Pull" << std::endl;
}
#endif




    // push
#if 1
{
    std::cout << "\nStart Push" << std::endl;
    typedef boost::coroutines2::coroutine< std::string >  coro_t;
    struct FinalEOL {
        ~FinalEOL() { std::cout << std::endl; }
    };
    const int  num = 3;
    const int  width = 15;
    coro_t::push_type  writer(
        [ num, width ] ( coro_t::pull_type& in ) {
        // finish the last line when we leave by whatever means
        const FinalEOL  eol;
        // pull values from upstream, lay them out `num` to a line
        for (;;) {
            for (int i = 0; i < num; ++i) {
                // when we exhaust the input, stop
                if (!in) {
                    return;
                }
                std::cout << std::setw( width ) << in.get();
                // now that we've handled this item, advance to next
                in();
            }
            // after `num` items, line break
            std::cout << std::endl;
        }
    } );

    const std::vector< std::string >  words{
        "peas", "porridge", "hot", "peas",
        "porridge", "cold", "peas", "porridge",
        "in", "the", "pot", "nine",
        "days", "old"
    };
    //std::copy( begin( words ), end( words ), begin( writer ) );
    for ( const std::string& word : words ) {
        writer( word );
    }
    std::cout << "\nDone Push" << std::endl;
}
#endif


	std::cout << "\n^\n";
	std::cin.ignore();

    return EXIT_SUCCESS;
}
