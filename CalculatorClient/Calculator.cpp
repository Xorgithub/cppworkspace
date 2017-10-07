// **********************************************************************
//
// Copyright (c) 2003-2016 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
//
// Ice version 3.6.3
//
// <auto-generated>
//
// Generated from file `Calculator.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#include "Calculator.h"
#include <IceUtil/PushDisableWarnings.h>
#include <Ice/LocalException.h>
#include <Ice/ObjectFactory.h>
#include <Ice/Outgoing.h>
#include <Ice/OutgoingAsync.h>
#include <Ice/BasicStream.h>
#include <IceUtil/Iterator.h>
#include <IceUtil/PopDisableWarnings.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 306
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 3
#       error Ice patch level mismatch!
#   endif
#endif

namespace
{

const ::std::string __MCal__Cal__add_name = "add";

const ::std::string __MCal__Cal__sub_name = "sub";

}
::IceProxy::Ice::Object* ::IceProxy::MCal::upCast(::IceProxy::MCal::Cal* p) { return p; }

void
::IceProxy::MCal::__read(::IceInternal::BasicStream* __is, ::IceInternal::ProxyHandle< ::IceProxy::MCal::Cal>& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::MCal::Cal;
        v->__copyFrom(proxy);
    }
}

::Ice::Int
IceProxy::MCal::Cal::add(::Ice::Int __p_num1, ::Ice::Int __p_num2, const ::Ice::Context* __ctx)
{
    __checkTwowayOnly(__MCal__Cal__add_name);
    ::IceInternal::Outgoing __og(this, __MCal__Cal__add_name, ::Ice::Normal, __ctx);
    try
    {
        ::IceInternal::BasicStream* __os = __og.startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_num1);
        __os->write(__p_num2);
        __og.endWriteParams();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    if(!__og.invoke())
    {
        try
        {
            __og.throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
            throw __uue;
        }
    }
    ::Ice::Int __ret;
    ::IceInternal::BasicStream* __is = __og.startReadParams();
    __is->read(__ret);
    __og.endReadParams();
    return __ret;
}

::Ice::AsyncResultPtr
IceProxy::MCal::Cal::begin_add(::Ice::Int __p_num1, ::Ice::Int __p_num2, const ::Ice::Context* __ctx, const ::IceInternal::CallbackBasePtr& __del, const ::Ice::LocalObjectPtr& __cookie)
{
    __checkAsyncTwowayOnly(__MCal__Cal__add_name);
    ::IceInternal::OutgoingAsyncPtr __result = new ::IceInternal::OutgoingAsync(this, __MCal__Cal__add_name, __del, __cookie);
    try
    {
        __result->prepare(__MCal__Cal__add_name, ::Ice::Normal, __ctx);
        ::IceInternal::BasicStream* __os = __result->startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_num1);
        __os->write(__p_num2);
        __result->endWriteParams();
        __result->invoke();
    }
    catch(const ::Ice::Exception& __ex)
    {
        __result->abort(__ex);
    }
    return __result;
}

#ifdef ICE_CPP11

::Ice::AsyncResultPtr
IceProxy::MCal::Cal::__begin_add(::Ice::Int __p_num1, ::Ice::Int __p_num2, const ::Ice::Context* __ctx, const ::IceInternal::Function<void (::Ice::Int)>& __response, const ::IceInternal::Function<void (const ::Ice::Exception&)>& __exception, const ::IceInternal::Function<void (bool)>& __sent)
{
    class Cpp11CB : public ::IceInternal::Cpp11FnCallbackNC
    {
    public:

        Cpp11CB(const ::std::function<void (::Ice::Int)>& responseFunc, const ::std::function<void (const ::Ice::Exception&)>& exceptionFunc, const ::std::function<void (bool)>& sentFunc) :
            ::IceInternal::Cpp11FnCallbackNC(exceptionFunc, sentFunc),
            _response(responseFunc)
        {
            CallbackBase::checkCallback(true, responseFunc || exceptionFunc != nullptr);
        }

        virtual void completed(const ::Ice::AsyncResultPtr& __result) const
        {
            ::MCal::CalPrx __proxy = ::MCal::CalPrx::uncheckedCast(__result->getProxy());
            ::Ice::Int __ret;
            try
            {
                __ret = __proxy->end_add(__result);
            }
            catch(const ::Ice::Exception& ex)
            {
                Cpp11FnCallbackNC::exception(__result, ex);
                return;
            }
            if(_response != nullptr)
            {
                _response(__ret);
            }
        }
    
    private:
        
        ::std::function<void (::Ice::Int)> _response;
    };
    return begin_add(__p_num1, __p_num2, __ctx, new Cpp11CB(__response, __exception, __sent));
}
#endif

::Ice::Int
IceProxy::MCal::Cal::end_add(const ::Ice::AsyncResultPtr& __result)
{
    ::Ice::AsyncResult::__check(__result, this, __MCal__Cal__add_name);
    ::Ice::Int __ret;
    if(!__result->__wait())
    {
        try
        {
            __result->__throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
        }
    }
    ::IceInternal::BasicStream* __is = __result->__startReadParams();
    __is->read(__ret);
    __result->__endReadParams();
    return __ret;
}

::Ice::Int
IceProxy::MCal::Cal::sub(::Ice::Int __p_num1, ::Ice::Int __p_num2, const ::Ice::Context* __ctx)
{
    __checkTwowayOnly(__MCal__Cal__sub_name);
    ::IceInternal::Outgoing __og(this, __MCal__Cal__sub_name, ::Ice::Normal, __ctx);
    try
    {
        ::IceInternal::BasicStream* __os = __og.startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_num1);
        __os->write(__p_num2);
        __og.endWriteParams();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    if(!__og.invoke())
    {
        try
        {
            __og.throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
            throw __uue;
        }
    }
    ::Ice::Int __ret;
    ::IceInternal::BasicStream* __is = __og.startReadParams();
    __is->read(__ret);
    __og.endReadParams();
    return __ret;
}

::Ice::AsyncResultPtr
IceProxy::MCal::Cal::begin_sub(::Ice::Int __p_num1, ::Ice::Int __p_num2, const ::Ice::Context* __ctx, const ::IceInternal::CallbackBasePtr& __del, const ::Ice::LocalObjectPtr& __cookie)
{
    __checkAsyncTwowayOnly(__MCal__Cal__sub_name);
    ::IceInternal::OutgoingAsyncPtr __result = new ::IceInternal::OutgoingAsync(this, __MCal__Cal__sub_name, __del, __cookie);
    try
    {
        __result->prepare(__MCal__Cal__sub_name, ::Ice::Normal, __ctx);
        ::IceInternal::BasicStream* __os = __result->startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_num1);
        __os->write(__p_num2);
        __result->endWriteParams();
        __result->invoke();
    }
    catch(const ::Ice::Exception& __ex)
    {
        __result->abort(__ex);
    }
    return __result;
}

#ifdef ICE_CPP11

::Ice::AsyncResultPtr
IceProxy::MCal::Cal::__begin_sub(::Ice::Int __p_num1, ::Ice::Int __p_num2, const ::Ice::Context* __ctx, const ::IceInternal::Function<void (::Ice::Int)>& __response, const ::IceInternal::Function<void (const ::Ice::Exception&)>& __exception, const ::IceInternal::Function<void (bool)>& __sent)
{
    class Cpp11CB : public ::IceInternal::Cpp11FnCallbackNC
    {
    public:

        Cpp11CB(const ::std::function<void (::Ice::Int)>& responseFunc, const ::std::function<void (const ::Ice::Exception&)>& exceptionFunc, const ::std::function<void (bool)>& sentFunc) :
            ::IceInternal::Cpp11FnCallbackNC(exceptionFunc, sentFunc),
            _response(responseFunc)
        {
            CallbackBase::checkCallback(true, responseFunc || exceptionFunc != nullptr);
        }

        virtual void completed(const ::Ice::AsyncResultPtr& __result) const
        {
            ::MCal::CalPrx __proxy = ::MCal::CalPrx::uncheckedCast(__result->getProxy());
            ::Ice::Int __ret;
            try
            {
                __ret = __proxy->end_sub(__result);
            }
            catch(const ::Ice::Exception& ex)
            {
                Cpp11FnCallbackNC::exception(__result, ex);
                return;
            }
            if(_response != nullptr)
            {
                _response(__ret);
            }
        }
    
    private:
        
        ::std::function<void (::Ice::Int)> _response;
    };
    return begin_sub(__p_num1, __p_num2, __ctx, new Cpp11CB(__response, __exception, __sent));
}
#endif

::Ice::Int
IceProxy::MCal::Cal::end_sub(const ::Ice::AsyncResultPtr& __result)
{
    ::Ice::AsyncResult::__check(__result, this, __MCal__Cal__sub_name);
    ::Ice::Int __ret;
    if(!__result->__wait())
    {
        try
        {
            __result->__throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
        }
    }
    ::IceInternal::BasicStream* __is = __result->__startReadParams();
    __is->read(__ret);
    __result->__endReadParams();
    return __ret;
}

const ::std::string&
IceProxy::MCal::Cal::ice_staticId()
{
    return ::MCal::Cal::ice_staticId();
}

::IceProxy::Ice::Object*
IceProxy::MCal::Cal::__newInstance() const
{
    return new Cal;
}

::Ice::Object* MCal::upCast(::MCal::Cal* p) { return p; }

namespace
{
const ::std::string __MCal__Cal_ids[2] =
{
    "::Ice::Object",
    "::MCal::Cal"
};

}

bool
MCal::Cal::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__MCal__Cal_ids, __MCal__Cal_ids + 2, _s);
}

::std::vector< ::std::string>
MCal::Cal::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__MCal__Cal_ids[0], &__MCal__Cal_ids[2]);
}

const ::std::string&
MCal::Cal::ice_id(const ::Ice::Current&) const
{
    return __MCal__Cal_ids[1];
}

const ::std::string&
MCal::Cal::ice_staticId()
{
#ifdef ICE_HAS_THREAD_SAFE_LOCAL_STATIC
    static const ::std::string typeId = "::MCal::Cal";
    return typeId;
#else
    return __MCal__Cal_ids[1];
#endif
}

::Ice::DispatchStatus
MCal::Cal::___add(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.startReadParams();
    ::Ice::Int __p_num1;
    ::Ice::Int __p_num2;
    __is->read(__p_num1);
    __is->read(__p_num2);
    __inS.endReadParams();
    ::Ice::Int __ret = add(__p_num1, __p_num2, __current);
    ::IceInternal::BasicStream* __os = __inS.__startWriteParams(::Ice::DefaultFormat);
    __os->write(__ret);
    __inS.__endWriteParams(true);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
MCal::Cal::___sub(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.startReadParams();
    ::Ice::Int __p_num1;
    ::Ice::Int __p_num2;
    __is->read(__p_num1);
    __is->read(__p_num2);
    __inS.endReadParams();
    ::Ice::Int __ret = sub(__p_num1, __p_num2, __current);
    ::IceInternal::BasicStream* __os = __inS.__startWriteParams(::Ice::DefaultFormat);
    __os->write(__ret);
    __inS.__endWriteParams(true);
    return ::Ice::DispatchOK;
}

namespace
{
const ::std::string __MCal__Cal_all[] =
{
    "add",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "sub"
};

}

::Ice::DispatchStatus
MCal::Cal::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< const ::std::string*, const ::std::string*> r = ::std::equal_range(__MCal__Cal_all, __MCal__Cal_all + 6, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __MCal__Cal_all)
    {
        case 0:
        {
            return ___add(in, current);
        }
        case 1:
        {
            return ___ice_id(in, current);
        }
        case 2:
        {
            return ___ice_ids(in, current);
        }
        case 3:
        {
            return ___ice_isA(in, current);
        }
        case 4:
        {
            return ___ice_ping(in, current);
        }
        case 5:
        {
            return ___sub(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
MCal::Cal::__writeImpl(::IceInternal::BasicStream* __os) const
{
    __os->startWriteSlice(ice_staticId(), -1, true);
    __os->endWriteSlice();
}

void
MCal::Cal::__readImpl(::IceInternal::BasicStream* __is)
{
    __is->startReadSlice();
    __is->endReadSlice();
}

void 
MCal::__patch(CalPtr& handle, const ::Ice::ObjectPtr& v)
{
    handle = ::MCal::CalPtr::dynamicCast(v);
    if(v && !handle)
    {
        IceInternal::Ex::throwUOE(::MCal::Cal::ice_staticId(), v);
    }
}