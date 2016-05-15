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

#ifndef _M_DELAY_EXECUTOR_H
#define _M_DELAY_EXECUTOR_H

#include <ace/Task.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>

class DelayExecutor : protected ACE_Task_Base
{
    public:
        DelayExecutor();
        ~DelayExecutor() override;

        static DelayExecutor* instance();

        int execute(ACE_Method_Request* new_req);

        using ACE_Task_Base::activate; // silence hidden overloaded function warning
        int activate(int num_threads = 1, ACE_Method_Request* pre_svc_hook = NULL, ACE_Method_Request* post_svc_hook = 0);

        int deactivate();

        bool activated();

        int svc() override;

    private:

        ACE_Activation_Queue queue_;
        ACE_Method_Request* pre_svc_hook_;
        ACE_Method_Request* post_svc_hook_;
        bool activated_;

        void activated(bool s);
};

#endif // _M_DELAY_EXECUTOR_H
