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

#ifndef __OREGON_REGRESSION_TEST_H_DEFINED__
#define __OREGON_REGRESSION_TEST_H_DEFINED__

#include "Common.h"

class RegressionTestSuite
{
    public:
        RegressionTestSuite();
        ~RegressionTestSuite();

        bool RunAll();
    protected:
        bool Run(bool(RegressionTestSuite::*)(), const char* comment);

        bool TestBreathingIssues();

        uint32 m_failedTestsCounter = 0;
        uint32 m_passedTestsCounter = 0;
};

#endif // __OREGON_REGRESSION_TEST_H_DEFINED__
