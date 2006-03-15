#include <Time/clockservice/Util.hpp>
#include <iostream>

int main()
{
   std::cout<<Cdmw::clock::compute_current_time()<<std::endl;
   return 0;
}
