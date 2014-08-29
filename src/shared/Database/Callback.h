/*
 * This file is part of the OregonCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OREGON_CALLBACK_H
#define OREGON_CALLBACK_H

#include <ace/Future.h>
#include <ace/Future_Set.h>
#include "QueryResult.h"

typedef ACE_Future<QueryResult_AutoPtr> QueryResultFuture;

/*! A simple template using ACE_Future to manage callbacks from the thread and object that
    issued the request. <ParamType> is variable type of parameter that is used as parameter
    for the callback function.
*/
template <typename ParamType>
class QueryCallback
{
    public:
        QueryCallback() {}
        
        void SetFutureResult(QueryResultFuture value)
        {
            result = value;
        }
        
        QueryResultFuture GetFutureResult()
        {
            return result;    
        }
        
        int IsReady()
        {
            return result.ready();
        }
        
        void GetResult(QueryResult_AutoPtr res)
        {
            result.get(res);
        }
        
        void FreeResult()
        {
            result.cancel();
        }
        
        void SetParam(ParamType value)
        {
            param = value;
        }
        
        ParamType GetParam()
        {
            return param;
        }
        
    private:
        QueryResultFuture result;
        ParamType param;
};

#endif