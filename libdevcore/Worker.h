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
/** @file Worker.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <signal.h>
#include <atomic>
#include <cassert>
#include <string>
#include <thread>

#include "Guards.h"

extern bool g_exitOnError;

namespace dev
{
enum class WorkerState
{
    Starting,
    Started,
    Stopping,
    Stopped,
    Killing
};

class Worker
{
public:
    Worker(std::string _name) : m_name(std::move(_name)) {}

    Worker(Worker const&) = delete;
    Worker& operator=(Worker const&) = delete;

    virtual ~Worker();

    /// Starts worker thread; causes startedWorking() to be called.
    void startWorking();

    /// Triggers worker thread it should stop
    void triggerStopWorking();

    /// Stop worker thread; causes call to stopWorking() and waits till thread has stopped.
    void stopWorking();

    /// Whether or not this worker should stop
    bool shouldStop() const { return m_state != WorkerState::Started; }

    std::string name() { return m_name; }

private:
    virtual void workLoop() = 0;

    std::string m_name;

    mutable Mutex x_work;                 ///< Lock for the network existence.
    std::unique_ptr<std::thread> m_work;  ///< The network thread.
    std::atomic<WorkerState> m_state = {WorkerState::Starting};
};

}  // namespace dev
