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

#include "Log.h"
#include "../Master.h"
#include "RegressionTest.h"

bool AddRG_Misc();

RegressionTestSuite::RegressionTestSuite()
{
}

RegressionTestSuite::~RegressionTestSuite()
{
}

bool RegressionTestSuite::RunAll()
{
    sLog.outString("Running Regression Tests...");

    Run(&RegressionTestSuite::TestBreathingIssues, "Breathing issues Maraudon");

    sLog.outString("Regression Tests Finished. Total: %u, Passed: %u, Failed: %u",
                    m_passedTestsCounter + m_failedTestsCounter,
                    m_passedTestsCounter,
                    m_failedTestsCounter);

    return m_failedTestsCounter == 0;
}

bool RegressionTestSuite::Run(bool(RegressionTestSuite::*test)(), const char* comment)
{
    if ((this->*test)())
    {
        sLog.outString("  [OK] Test %s passed.", comment);
        ++m_passedTestsCounter;
        return true;
    }
    else
    {
        sLog.outString("  [ERR] Test %s didnt pass.", comment);
        ++m_failedTestsCounter;
        return false;
    }
}
