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

#pragma once

#include <libdevcore/Worker.h>
#include <libethcore/EthashAux.h>
#include <libethcore/Miner.h>
#include <libprogpow/ProgPow.h>

#include <cuda.h>
#include <nvrtc.h>

#include "CUDAMiner_cuda.h"

#include <functional>

namespace dev
{
namespace eth
{
class CUDAMiner : public Miner
{
public:
    CUDAMiner(unsigned _index, CUSettings _settings, DeviceDescriptor& _device);
    ~CUDAMiner() override;

    static int getNumDevices();
    static void enumDevices(std::map<string, DeviceDescriptor>& _DevicesCollection);

    void search(
        uint8_t const* header, uint64_t target, uint64_t _startN, const dev::eth::WorkPackage& w);

protected:
    bool initDevice() override;

    bool initEpoch_internal() override;

    void kick_miner() override;

private:
    atomic<bool> m_new_work = {false};

    void workLoop() override;

    uint8_t m_kernelCompIx = 0;
    uint8_t m_kernelExecIx = 1;
    CUfunction m_kernel[2];
    std::vector<volatile Search_results*> m_search_buf;
    std::vector<cudaStream_t> m_streams;
    uint64_t m_current_target = 0;

    CUSettings m_settings;

    const uint32_t m_batch_size;
    const uint32_t m_streams_batch_size;

    uint64_t m_allocated_memory_dag = 0; // dag_size is a uint64_t in EpochContext struct
    size_t m_allocated_memory_light_cache = 0;

    void compileKernel(uint64_t prog_seed, uint64_t dag_words, CUfunction& kernel);
    void asyncCompile();
    CUcontext m_context;
};


}  // namespace eth
}  // namespace dev
