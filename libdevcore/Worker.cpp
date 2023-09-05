/*
    This file is part of ethcoreminer.

    ethcoreminer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ethcoreminer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ethcoreminer.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file Worker.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#include <chrono>
#include <thread>

#include "Log.h"
#include "Worker.h"

using namespace std;
using namespace dev;

void Worker::startWorking()
{
    //	cnote << "startWorking for thread" << m_name;
    Guard l(x_work);
    if (m_work)
    {
        WorkerState ex = WorkerState::Stopped;
        m_state.compare_exchange_weak(ex, WorkerState::Starting, std::memory_order_relaxed);
    }
    else
    {
        m_state = WorkerState::Starting;
        m_work.reset(new thread([&]() {
            setThreadName(m_name.c_str());
            //			cnote << "Thread begins";
            while (m_state != WorkerState::Killing)
            {
                WorkerState ex = WorkerState::Starting;
                bool ok = m_state.compare_exchange_weak(
                    ex, WorkerState::Started, std::memory_order_relaxed);
                //				cnote << "Trying to set Started: Thread was" << (unsigned)ex << "; "
                //<< ok;
                (void)ok;

                try
                {
                    workLoop();
                }
                catch (std::exception const& _e)
                {
                    clog(WarnChannel) << "Exception thrown in Worker thread: " << _e.what();
                    if (g_exitOnError)
                    {
                        clog(WarnChannel) << "Terminating due to --exit";
                        raise(SIGTERM);
                    }
                }

                // ex = WorkerState::Stopping;
                // m_state.compare_exchange_weak(ex, WorkerState::Stopped,
                // std::memory_order_relaxed));

                ex = m_state.exchange(WorkerState::Stopped);
                //				cnote << "State: Stopped: Thread was" << (unsigned)ex;
                if (ex == WorkerState::Killing || ex == WorkerState::Starting)
                    m_state.exchange(ex);

                while (m_state == WorkerState::Stopped)
                    this_thread::sleep_for(chrono::milliseconds(20));
            }
        }));
        //		cnote << "Spawning" << m_name;
    }
    while (m_state == WorkerState::Starting)
        this_thread::sleep_for(chrono::microseconds(20));
}

void Worker::triggerStopWorking()
{
    DEV_GUARDED(x_work)
    if (m_work)
    {
        WorkerState ex = WorkerState::Started;
        m_state.compare_exchange_weak(ex, WorkerState::Stopping, std::memory_order_relaxed);
    }
}

void Worker::stopWorking()
{
    DEV_GUARDED(x_work)
    if (m_work)
    {
        WorkerState ex = WorkerState::Started;
        m_state.compare_exchange_weak(ex, WorkerState::Stopping, std::memory_order_relaxed);

        while (m_state != WorkerState::Stopped)
            this_thread::sleep_for(chrono::microseconds(20));
    }
}

Worker::~Worker()
{
    DEV_GUARDED(x_work)
    if (m_work)
    {
        m_state.exchange(WorkerState::Killing);
        m_work->join();
        m_work.reset();
    }
}
