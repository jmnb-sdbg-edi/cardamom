/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */

#ifndef INCL_COMMONSVCS_DATASTORE_DATASTOREINTERFACE_HPP
#define INCL_COMMONSVCS_DATASTORE_DATASTOREINTERFACE_HPP

#include <Foundation/commonsvcs/datastore/DataStoreBase.hpp>
#include <Foundation/commonsvcs/datastore/DataStoreObserver.hpp>
#include <Foundation/commonsvcs/datastore/StorageErrorException.hpp>
#include <Foundation/orbsupport/TypeTraits.hpp>
#include <Foundation/orbsupport/Sequence.hpp>

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {


template <class Oid, class Data >
class Functor
{
public:
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_var_type     _Oid_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<Data>::_var_type    _Data_var_type;
    Functor() {}
    virtual ~Functor() {}
    
   virtual void operator()(const std::pair< _Oid_var_type, _Data_var_type > & ) = 0;
};


template <class Oid, class Data > 
class FunctorWrapper
{
public:
  typedef typename Cdmw::OrbSupport::type_traits<Oid>::_var_type     _Oid_var_type;
  typedef typename Cdmw::OrbSupport::type_traits<Data>::_var_type    _Data_var_type;
  
  FunctorWrapper(Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>& f)
    : m_f(f) {}
  ~FunctorWrapper() {} 
  void operator()(const std::pair< _Oid_var_type, _Data_var_type > & x) 
  {
    m_f(x);
  }
private:
  Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>& m_f;
};

/*template <class Oid, class Data, typename Function >
class Functor_T : public Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>
{
public:
  typedef typename Cdmw::OrbSupport::type_traits<Oid>::_var_type     _Oid_var_type;
  typedef typename Cdmw::OrbSupport::type_traits<Data>::_var_type    _Data_var_type;

  Functor_T(Function f) : m_f(f) {}
  virtual void operator()(const std::pair< _Oid_var_type, _Data_var_type> & x )
  {
    m_f(x);
  }
private:
  Function m_f;
};*/

template <class Oid, class Data, class OidCompare=std::less<typename Cdmw::OrbSupport::type_traits<Oid>::_var_type> > 
class DataStoreInterface
{
public:
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_var_type     _Oid_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_in_type      _Oid_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_retn_type    _Oid_retn_type;

    typedef typename Cdmw::OrbSupport::type_traits<Data>::_var_type     _Data_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<Data>::_in_type      _Data_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<Data>::_retn_type    _Data_retn_type;

    typedef Functor<Oid,Data> Functor;

    typedef DataStoreObserver<Oid, Data> DataStoreObserver;

    virtual ~DataStoreInterface() {}

    /**
     * Return true if the DataStore contains a data corresponding to the Object Id key
     *
     *@param oid is the Object Id key
     *@return true if the Data Store contains this data
     */
    virtual bool contains(_Oid_in_type oid) throw() = 0;

    /**
     * Return the data corresponding to the Object Id key
     *
     *@param oid is the Object Id key
     *@exception NotFound if no object found with this oid
     */
    virtual _Data_retn_type select(const _Oid_in_type oid) throw (NotFoundException) = 0;

    /**
     * output in iterator each element of DataStore
     * wich satisfy the predicate
     * NOTE: The implementation does not autorise the given predicate to modify the data
     *       Only Predicate with const parameter will compile
     *       (typical example: const std::pair<Oid, Data>& is OK)
     *
     *@param Predicate p the predicate to verify
     *@param OutputIterator on result 
     */
    template <typename Predicate, typename OutputIterator>
    void find_if(Predicate p, OutputIterator result)
    {
      class FindFunctor__: public Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>
      {
      public:
    FindFunctor__(Predicate p, OutputIterator result) : m_p(p), m_result(result) {}
    virtual void operator()(const std::pair<typename Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>::_Oid_var_type , typename Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>::_Data_var_type > & x )
    {
      if (m_p(x))
        {
          *m_result = x.first;
          m_result++;
        }
    }
      private:
    Predicate m_p;
    OutputIterator m_result;
      };
      FindFunctor__ f__(p, result);
      do_for_each(f__);
    }
  
    /**
     * execute function for each element of DataStore
     * NOTE: The implementation does not autorise the given function to modify the data
     *       Only Function with const parameter will compile
     *       (typical example: const std::pair<Oid, Data>& is OK)
     *
     *@param Function f to execute
     */
    template <typename Function>
    void for_each(Function f)
    {
      class Functor__ : public Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>
      {
      public:
    Functor__(Function f) : m_f(f) {}
    virtual void operator()(const std::pair<typename Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>::_Oid_var_type, typename Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>::_Data_var_type> & x )
    {
      m_f(x);
    }
      private:
      Function m_f;
      };
      Functor__ f__(f);
      do_for_each(f__);
    }

    /**
     * Remove from the DataStore the entry with the given oid key
     *
     *@param oid is the Object Id key
     *@exception NotFoundException indicates that <i>oid</i> is not known
     *@exception StorageErrorException indicates that an error has occured during removal of the data.
     *           The reason field indicates the reason of failure.
     */
    virtual void remove(const _Oid_in_type oid) throw (NotFoundException,StorageErrorException) = 0;
    
    /**
     * insert in the DataStore the entry with the given oid key
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     *@exception AlreadyExistException indicates that <i>oid</i> already exists
     *@exception StorageErrorException indicates that an error has occured during insertion of the data.
     *           The reason field indicates the reason of failure.
     */
    virtual void insert(const _Oid_in_type oid, const _Data_in_type data)
        throw (AlreadyExistException,StorageErrorException) = 0;

    /**
     * update the DataStore entry with the given oid key
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     *@exception NotFoundException indicates that <i>oid</i> is not known
     *@exception StorageErrorException indicates that an error has occured during update of the data.
     *           The reason field indicates the reason of failure.
     */
    virtual void update(const _Oid_in_type oid, const _Data_in_type data)
        throw (NotFoundException,StorageErrorException) = 0;

    /**
     * Give the size of the DataStore
     *
     *@return int the size
     */
    virtual int size() = 0;

    /**
     * Registers an observer to be notified whenever there is an insertion, update, or removal
     * of a record.
     *
     *@param observer a const pointer to a DataStoreObserver object
     *@return true if the Data Store observer is registered, false if failed to register it
     * (it has already been registered, or exceeded maximum allowed number of observers ...)
     */
    virtual bool register_observer(DataStoreObserver * observer) throw() = 0;

    /**
     * Un-registers an already registered observer. The observer will no longer be notified by the datastore.
     *
     *@param observer a const pointer to the DataStoreObserver object
     *@return true if the Data Store observer is unregistered, false if the observer was not previously registered
     */
    virtual bool unregister_observer(DataStoreObserver * observer) throw() = 0;

protected:
    virtual void do_for_each(Cdmw::CommonSvcs::DataStore::Functor<Oid,Data>& f) = 0;

    DataStoreInterface()
    {
    }
    
    
};


}  // End namespace DataStore  
}  // End namespace CommonSvcs 
}  // End namespace Cdmw

#endif /* INCL_COMMONSVCS_DATASTORE_DATASTOREINTERFACE_HPP */
