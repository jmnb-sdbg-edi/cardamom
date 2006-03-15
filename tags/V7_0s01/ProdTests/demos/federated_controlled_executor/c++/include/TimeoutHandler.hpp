#include <Time/clockservice/CosClockService.skel.hpp>

class TimeoutHandler :
    public POA_CosClockService::PeriodicExecution::Periodic
{
public:
    TimeoutHandler(CosClockService::ControlledClock_ptr clock);
    virtual ~TimeoutHandler();

public:
  //virtual bool do_work(const CORBA::Any&);
   virtual CORBA::Boolean do_work(const CORBA::Any&)
      throw (CORBA::SystemException);

private:
    int  m_count;
    
    CosClockService::ControlledClock_var  m_clock;
};
