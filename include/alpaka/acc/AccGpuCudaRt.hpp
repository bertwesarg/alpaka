/**
* \file
* Copyright 2014-2016 Benjamin Worpitz, Rene Widera
*
* This file is part of alpaka.
*
* alpaka is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* alpaka is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with alpaka.
* If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef ALPAKA_ACC_GPU_CUDA_ENABLED

#include <alpaka/core/Common.hpp>

#if !BOOST_LANG_CUDA
    #error If ALPAKA_ACC_GPU_CUDA_ENABLED is set, the compiler has to support CUDA!
#endif

// Base classes.
#include <alpaka/workdiv/WorkDivCudaBuiltIn.hpp>
#include <alpaka/idx/gb/IdxGbCudaBuiltIn.hpp>
#include <alpaka/idx/bt/IdxBtCudaBuiltIn.hpp>
#include <alpaka/atomic/AtomicCudaBuiltIn.hpp>
#include <alpaka/atomic/AtomicHierarchy.hpp>
#include <alpaka/math/MathCudaBuiltIn.hpp>
#include <alpaka/block/shared/dyn/BlockSharedMemDynCudaBuiltIn.hpp>
#include <alpaka/block/shared/st/BlockSharedMemStCudaBuiltIn.hpp>
#include <alpaka/block/sync/BlockSyncCudaBuiltIn.hpp>
#include <alpaka/rand/RandCuRand.hpp>
#include <alpaka/time/TimeCudaBuiltIn.hpp>

// Specialized traits.
#include <alpaka/acc/Traits.hpp>
#include <alpaka/dev/Traits.hpp>
#include <alpaka/kernel/Traits.hpp>
#include <alpaka/pltf/Traits.hpp>
#include <alpaka/idx/Traits.hpp>

// Implementation details.
#include <alpaka/dev/DevCudaRt.hpp>
#include <alpaka/core/Cuda.hpp>

#include <boost/predef.h>

#include <typeinfo>

namespace alpaka
{
    namespace exec
    {
        template<
            typename TDim,
            typename TIdx,
            typename TKernelFnObj,
            typename... TArgs>
        class ExecGpuCudaRt;
    }
    namespace acc
    {
        //#############################################################################
        //! The GPU CUDA accelerator.
        //!
        //! This accelerator allows parallel kernel execution on devices supporting CUDA.
        template<
            typename TDim,
            typename TIdx>
        class AccGpuCudaRt final :
            public workdiv::WorkDivCudaBuiltIn<TDim, TIdx>,
            public idx::gb::IdxGbCudaBuiltIn<TDim, TIdx>,
            public idx::bt::IdxBtCudaBuiltIn<TDim, TIdx>,
            public atomic::AtomicHierarchy<
                atomic::AtomicCudaBuiltIn, // grid atomics
                atomic::AtomicCudaBuiltIn, // block atomics
                atomic::AtomicCudaBuiltIn  // thread atomics
            >,
            public math::MathCudaBuiltIn,
            public block::shared::dyn::BlockSharedMemDynCudaBuiltIn,
            public block::shared::st::BlockSharedMemStCudaBuiltIn,
            public block::sync::BlockSyncCudaBuiltIn,
            public rand::RandCuRand,
            public time::TimeCudaBuiltIn
        {
        public:
            //-----------------------------------------------------------------------------
            ALPAKA_FN_ACC_CUDA_ONLY AccGpuCudaRt(
                vec::Vec<TDim, TIdx> const & threadElemExtent) :
                    workdiv::WorkDivCudaBuiltIn<TDim, TIdx>(threadElemExtent),
                    idx::gb::IdxGbCudaBuiltIn<TDim, TIdx>(),
                    idx::bt::IdxBtCudaBuiltIn<TDim, TIdx>(),
                    atomic::AtomicHierarchy<
                        atomic::AtomicCudaBuiltIn, // atomics between grids
                        atomic::AtomicCudaBuiltIn, // atomics between blocks
                        atomic::AtomicCudaBuiltIn  // atomics between threads
                    >(),
                    math::MathCudaBuiltIn(),
                    block::shared::dyn::BlockSharedMemDynCudaBuiltIn(),
                    block::shared::st::BlockSharedMemStCudaBuiltIn(),
                    block::sync::BlockSyncCudaBuiltIn(),
                    rand::RandCuRand(),
                    time::TimeCudaBuiltIn()
            {}

        public:
            //-----------------------------------------------------------------------------
            ALPAKA_FN_ACC_CUDA_ONLY AccGpuCudaRt(AccGpuCudaRt const &) = delete;
            //-----------------------------------------------------------------------------
            ALPAKA_FN_ACC_CUDA_ONLY AccGpuCudaRt(AccGpuCudaRt &&) = delete;
            //-----------------------------------------------------------------------------
            ALPAKA_FN_ACC_CUDA_ONLY auto operator=(AccGpuCudaRt const &) -> AccGpuCudaRt & = delete;
            //-----------------------------------------------------------------------------
            ALPAKA_FN_ACC_CUDA_ONLY auto operator=(AccGpuCudaRt &&) -> AccGpuCudaRt & = delete;
            //-----------------------------------------------------------------------------
            ~AccGpuCudaRt() = default;
        };
    }

    namespace acc
    {
        namespace traits
        {
            //#############################################################################
            //! The GPU CUDA accelerator accelerator type trait specialization.
            template<
                typename TDim,
                typename TIdx>
            struct AccType<
                acc::AccGpuCudaRt<TDim, TIdx>>
            {
                using type = acc::AccGpuCudaRt<TDim, TIdx>;
            };
            //#############################################################################
            //! The GPU CUDA accelerator device properties get trait specialization.
            template<
                typename TDim,
                typename TIdx>
            struct GetAccDevProps<
                acc::AccGpuCudaRt<TDim, TIdx>>
            {
                //-----------------------------------------------------------------------------
                ALPAKA_FN_HOST static auto getAccDevProps(
                    dev::DevCudaRt const & dev)
                -> acc::AccDevProps<TDim, TIdx>
                {
                    cudaDeviceProp cudaDevProp;
                    ALPAKA_CUDA_RT_CHECK(cudaGetDeviceProperties(
                        &cudaDevProp,
                        dev.m_iDevice));

                    return {
                        // m_multiProcessorCount
                        static_cast<TIdx>(cudaDevProp.multiProcessorCount),
                        // m_gridBlockExtentMax
                        extent::getExtentVecEnd<TDim>(
                            vec::Vec<dim::DimInt<3u>, TIdx>(
                                static_cast<TIdx>(cudaDevProp.maxGridSize[2]),
                                static_cast<TIdx>(cudaDevProp.maxGridSize[1]),
                                static_cast<TIdx>(cudaDevProp.maxGridSize[0]))),
                        // m_gridBlockCountMax
                        std::numeric_limits<TIdx>::max(),
                        // m_blockThreadExtentMax
                        extent::getExtentVecEnd<TDim>(
                            vec::Vec<dim::DimInt<3u>, TIdx>(
                                static_cast<TIdx>(cudaDevProp.maxThreadsDim[2]),
                                static_cast<TIdx>(cudaDevProp.maxThreadsDim[1]),
                                static_cast<TIdx>(cudaDevProp.maxThreadsDim[0]))),
                        // m_blockThreadCountMax
                        static_cast<TIdx>(cudaDevProp.maxThreadsPerBlock),
                        // m_threadElemExtentMax
                        vec::Vec<TDim, TIdx>::all(std::numeric_limits<TIdx>::max()),
                        // m_threadElemCountMax
                        std::numeric_limits<TIdx>::max()};
                }
            };
            //#############################################################################
            //! The GPU CUDA accelerator name trait specialization.
            template<
                typename TDim,
                typename TIdx>
            struct GetAccName<
                acc::AccGpuCudaRt<TDim, TIdx>>
            {
                //-----------------------------------------------------------------------------
                ALPAKA_FN_HOST static auto getAccName()
                -> std::string
                {
                    return "AccGpuCudaRt<" + std::to_string(TDim::value) + "," + typeid(TIdx).name() + ">";
                }
            };
        }
    }
    namespace dev
    {
        namespace traits
        {
            //#############################################################################
            //! The GPU CUDA accelerator device type trait specialization.
            template<
                typename TDim,
                typename TIdx>
            struct DevType<
                acc::AccGpuCudaRt<TDim, TIdx>>
            {
                using type = dev::DevCudaRt;
            };
        }
    }
    namespace dim
    {
        namespace traits
        {
            //#############################################################################
            //! The GPU CUDA accelerator dimension getter trait specialization.
            template<
                typename TDim,
                typename TIdx>
            struct DimType<
                acc::AccGpuCudaRt<TDim, TIdx>>
            {
                using type = TDim;
            };
        }
    }
    namespace kernel
    {
        namespace traits
        {
            //#############################################################################
            //! The GPU CUDA accelerator executor type trait specialization.
            template<
                typename TDim,
                typename TIdx,
                typename TWorkDiv,
                typename TKernelFnObj,
                typename... TArgs>
            struct CreateTaskExec<
                acc::AccGpuCudaRt<TDim, TIdx>,
                TWorkDiv,
                TKernelFnObj,
                TArgs...>
            {
                //-----------------------------------------------------------------------------
                ALPAKA_FN_HOST static auto createTaskExec(
                    TWorkDiv const & workDiv,
                    TKernelFnObj const & kernelFnObj,
                    TArgs const & ... args)
#ifdef BOOST_NO_CXX14_RETURN_TYPE_DEDUCTION
                -> exec::ExecGpuCudaRt<
                    TDim,
                    TIdx,
                    TKernelFnObj,
                    TArgs...>
#endif
                {
                    return
                        exec::ExecGpuCudaRt<
                            TDim,
                            TIdx,
                            TKernelFnObj,
                            TArgs...>(
                                workDiv,
                                kernelFnObj,
                                args...);
                }
            };
        }
    }
    namespace pltf
    {
        namespace traits
        {
            //#############################################################################
            //! The CPU CUDA executor platform type trait specialization.
            template<
                typename TDim,
                typename TIdx>
            struct PltfType<
                acc::AccGpuCudaRt<TDim, TIdx>>
            {
                using type = pltf::PltfCudaRt;
            };
        }
    }
    namespace idx
    {
        namespace traits
        {
            //#############################################################################
            //! The GPU CUDA accelerator idx type trait specialization.
            template<
                typename TDim,
                typename TIdx>
            struct IdxType<
                acc::AccGpuCudaRt<TDim, TIdx>>
            {
                using type = TIdx;
            };
        }
    }
}

#endif
