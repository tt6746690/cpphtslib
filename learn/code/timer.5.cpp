/* 
    1. synchronous timer with .wait()
    2. async timer with .async_wait() + io_service.run()
    3. binding arguments to a handler, with boost::bind()
    4. using a member function as a handler with boost::bind(memberfunc, this)
    5. synchronising handlers in multithreaded programs
*/
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::asio;
using namespace boost;
using std::cout;
using std::endl;

class printer
{
  public:
    printer(io_service &io)
        : strand_(io),
          timer1_(io, posix_time::seconds(1)),
          timer2_(io, posix_time::seconds(1)),
          count_(0)
    {
        /*  Boost::bind
                -- for binding emmeber function, second arg is object context
            strand::wrap 
                -- each callback handler wrapped, returning a new handler that automatically dispatches its contained handler through strand object
                -- ensures wrapped callback cannot execute concurrently 
        */
        timer1_.async_wait(strand_.wrap(boost::bind(&printer::print1, this)));
        timer2_.async_wait(strand_.wrap(boost::bind(&printer::print2, this)));
    }

    void print1()
    {
        if (count_ < 10)
        {
            cout << "Timer 1: " << count_ << endl;
            ++count_;

            timer1_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));
            timer1_.async_wait(strand_.wrap(boost::bind(&printer::print1, this)));
        }
    }
    void print2()
    {
        if (count_ < 10)
        {
            cout << "Timer 2: " << count_ << endl;
            ++count_;

            timer2_.expires_at(timer2_.expires_at() + boost::posix_time::seconds(1));
            timer2_.async_wait(strand_.wrap(boost::bind(&printer::print2, this)));
        }
    }

    ~printer()
    {
        cout << "Final count is " << count_ << endl;
    }

  private:
    io_service::strand strand_;
    deadline_timer timer1_;
    deadline_timer timer2_;
    int count_; /* shared resource used by 2 timers */
};

int main()
{
    io_service io;
    printer p(io);

    /* Guarantee
        -- callback handler called from threads currently calling `run()` only
        -- for one thread, calling run() 
            -- ensures callback handler runs in mutex
                -- poor responseness when handler takes long time to respond 
                -- cannot scale on multiprocessor systems 
                -- but good for development 
        -- a thread pool, each calling run(), 
            -- however callback handler may execute concurrently
            -- use io_service::strand, guarantees that for handlers dispatched through it, an execution handler will be allowed to complete before the next one is started
            -- of course, handler may still execute concurrently with handlers not dispatched through strand
    */
    io.run();

    return 0;
}