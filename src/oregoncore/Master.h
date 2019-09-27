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

#ifndef _MASTER_H
#define _MASTER_H

#include "Common.h"
#include "Policies/Singleton.h"
#include "RegressionTests/RegressionTest.h"

// Start the server
class Master
{
    public:
        Master();
        ~Master();
        int Run(bool runTests);
        static volatile uint32 m_masterLoopCounter;

        bool RunRegressionTests();
    private:
        void _StartDB();

        void _HookSignals();
        void _UnhookSignals();
        static void _OnSignal(int s);

        void MainLoop();
        void clearOnlineAccounts();
};

#define sMaster Oregon::Singleton<Master>::Instance()
#endif
