/**
* \file
* Copyright 2014-2015 Benjamin Worpitz
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

#include <alpaka/traits/Dim.hpp>        // DimT
#include <alpaka/traits/mem/Space.hpp>  // SpaceT

#include <alpaka/core/Common.hpp>       // ALPAKA_FCT_HOST

namespace alpaka
{
    namespace traits
    {
        namespace mem
        {
            //#############################################################################
            //! The memory element type trait.
            //#############################################################################
            template<
                typename TView,
                typename TSfinae = void>
            struct ElemType;

            //#############################################################################
            //! The native pointer get trait.
            //#############################################################################
            template<
                typename TBuf,
                typename TSfinae = void>
            struct GetPtrNative;

            //#############################################################################
            //! The pointer on device get trait.
            //#############################################################################
            template<
                typename TBuf,
                typename TDev,
                typename TSfinae = void>
            struct GetPtrDev;

            //#############################################################################
            //! The pitch in bytes. This is the distance between two consecutive rows.
            //!
            //! The default implementation uses the extent to calculate the pitch.
            //#############################################################################
            template<
                UInt TuiIdx,
                typename TView,
                typename TSfinae = void>
            struct GetPitchBytes
            {
                //-----------------------------------------------------------------------------
                //!
                //-----------------------------------------------------------------------------
                ALPAKA_FCT_HOST static auto getPitchBytes(
                    TView const & buf)
                -> UInt
                {
                    return alpaka::extent::getExtent<TuiIdx, UInt>(buf) * sizeof(ElemType<TView>::type);
                }
            };

            //#############################################################################
            //! The memory set trait.
            //!
            //! Fills the buffer with data.
            //#############################################################################
            template<
                typename TDim,
                typename TSpace,
                typename TSfinae = void>
            struct Set;

            //#############################################################################
            //! The memory copy trait.
            //!
            //! Copies memory from one buffer into another buffer possibly in a different memory space.
            //#############################################################################
            template<
                typename TDim,
                typename TSpaceDst,
                typename TSpaceSrc,
                typename TSfinae = void>
            struct Copy;

            //#############################################################################
            //! The memory buffer view type trait.
            //#############################################################################
            template<
                typename TElem,
                typename TDim,
                typename TDev,
                typename TSfinae = void>
            struct ViewType;

            //#############################################################################
            //! The memory buffer view creation type trait.
            //#############################################################################
            template<
                typename TBuf,
                typename TSfinae = void>
            struct CreateView;

            //#############################################################################
            //! The base trait.
            //#############################################################################
            template<
                typename TBuf,
                typename TSfinae = void>
            struct GetBase;
        }
    }

    namespace mem
    {
        //#############################################################################
        //! The memory element type trait alias template to remove the ::type.
        //#############################################################################
        template<
            typename TView>
        using ElemT = typename traits::mem::ElemType<TView>::type;

        //-----------------------------------------------------------------------------
        //! Gets the native pointer of the memory buffer.
        //!
        //! \param buf The memory buffer.
        //! \return The native pointer.
        //-----------------------------------------------------------------------------
        template<
            typename TBuf>
        ALPAKA_FCT_HOST auto getPtrNative(
            TBuf const & buf)
        -> ElemT<TBuf> const *
        {
            return traits::mem::GetPtrNative<
                TBuf>
            ::getPtrNative(
                buf);
        }
        //-----------------------------------------------------------------------------
        //! Gets the native pointer of the memory buffer.
        //!
        //! \param buf The memory buffer.
        //! \return The native pointer.
        //-----------------------------------------------------------------------------
        template<
            typename TBuf>
        ALPAKA_FCT_HOST auto getPtrNative(
            TBuf & buf)
        -> ElemT<TBuf> *
        {
            return traits::mem::GetPtrNative<
                TBuf>
            ::getPtrNative(
                buf);
        }

        //-----------------------------------------------------------------------------
        //! Gets the pointer to the buffer on the given device.
        //!
        //! \param buf The memory buffer.
        //! \param dev The device.
        //! \return The pointer on the device.
        //-----------------------------------------------------------------------------
        template<
            typename TBuf,
            typename TDev>
        ALPAKA_FCT_HOST auto getPtrDev(
            TBuf const & buf,
            TDev const & dev)
        -> ElemT<TBuf> const *
        {
            return traits::mem::GetPtrDev<
                TBuf,
                TDev>
            ::getPtrDev(
                buf,
                dev);
        }
        //-----------------------------------------------------------------------------
        //! Gets the pointer to the buffer on the given device.
        //!
        //! \param buf The memory buffer.
        //! \param dev The device.
        //! \return The pointer on the device.
        //-----------------------------------------------------------------------------
        template<
            typename TBuf,
            typename TDev>
        ALPAKA_FCT_HOST auto getPtrDev(
            TBuf & buf,
            TDev const & dev)
        -> ElemT<TBuf> *
        {
            return traits::mem::GetPtrDev<
                TBuf,
                TDev>
            ::getPtrDev(
                buf,
                dev);
        }

        //-----------------------------------------------------------------------------
        //! \return The pitch in bytes. This is the distance between two consecutive rows.
        //-----------------------------------------------------------------------------
        template<
            UInt TuiIdx,
            typename TVal,
            typename TView>
        ALPAKA_FCT_HOST auto getPitchBytes(
            TView const & buf)
        -> TVal
        {
            return
                static_cast<TVal>(
                    traits::mem::GetPitchBytes<
                        TuiIdx,
                        TView>
                    ::getPitchBytes(
                        buf));
        }

        //-----------------------------------------------------------------------------
        //! \return The pitch in elements. This is the distance between two consecutive rows.
        //-----------------------------------------------------------------------------
        template<
            UInt TuiIdx,
            typename TVal,
            typename TView>
        ALPAKA_FCT_HOST auto getPitchElements(
            TView const & buf)
        -> TVal
        {
            assert((getPitchBytes<TuiIdx, TVal>(buf) % sizeof(ElemT<TView>)) == 0u);
            return getPitchBytes<TuiIdx, TVal>(buf) / sizeof(ElemT<TView>);
        }

        //-----------------------------------------------------------------------------
        //! Sets the memory to the given value.
        //!
        //! \param buf The memory buffer to fill.
        //! \param byte Value to set for each element of the specified buffer.
        //! \param extents The extents of the buffer to fill.
        //-----------------------------------------------------------------------------
        template<
            typename TView,
            typename TExtents>
        ALPAKA_FCT_HOST auto set(
            TView & buf,
            std::uint8_t const & byte,
            TExtents const & extents)
        -> void
        {
            static_assert(
                dim::DimT<TView>::value == dim::DimT<TExtents>::value,
                "The buffer and the extents are required to have the same dimensionality!");

            traits::mem::Set<
                dim::DimT<TView>,
                SpaceT<TView>>
            ::set(
                buf,
                byte,
                extents);
        }

        //-----------------------------------------------------------------------------
        //! Sets the memory to the given value asynchronously.
        //!
        //! \param buf The memory buffer to fill.
        //! \param byte Value to set for each element of the specified buffer.
        //! \param extents The extents of the buffer to fill.
        //! \param stream The stream to enqueue the buffer fill task into.
        //-----------------------------------------------------------------------------
        template<
            typename TView,
            typename TExtents,
            typename TStream>
        ALPAKA_FCT_HOST auto set(
            TView & buf,
            std::uint8_t const & byte,
            TExtents const & extents,
            TStream const & stream)
        -> void
        {
            static_assert(
                dim::DimT<TView>::value == dim::DimT<TExtents>::value,
                "The buffer and the extents are required to have the same dimensionality!");

            traits::mem::Set<
                dim::DimT<TView>,
                SpaceT<TView>,
                TStream>
            ::set(
                buf,
                byte,
                extents,
                stream);
        }

        //-----------------------------------------------------------------------------
        //! Copies memory possibly between different memory spaces.
        //!
        //! \param bufDst The destination memory buffer.
        //! \param bufSrc The source memory buffer.
        //! \param extents The extents of the buffer to copy.
        //-----------------------------------------------------------------------------
        template<
            typename TBufDst,
            typename TBufSrc,
            typename TExtents>
        ALPAKA_FCT_HOST auto copy(
            TBufDst & bufDst,
            TBufSrc const & bufSrc,
            TExtents const & extents)
        -> void
        {
            static_assert(
                dim::DimT<TBufDst>::value == dim::DimT<TBufSrc>::value,
                "The source and the destination buffers are required to have the same dimensionality!");
            static_assert(
                dim::DimT<TBufDst>::value == dim::DimT<TExtents>::value,
                "The destination buffer and the extents are required to have the same dimensionality!");
            static_assert(
                std::is_same<ElemT<TBufDst>, ElemT<TBufSrc>>::value,
                "The source and the destination buffers are required to have the same element type!");

            traits::mem::Copy<
                dim::DimT<TBufDst>,
                SpaceT<TBufDst>,
                SpaceT<TBufSrc>>
            ::copy(
                bufDst,
                bufSrc,
                extents);
        }

        //-----------------------------------------------------------------------------
        //! Copies memory possibly between different memory spaces asynchronously.
        //!
        //! \param bufDst The destination memory buffer.
        //! \param bufSrc The source memory buffer.
        //! \param extents The extents of the buffer to copy.
        //! \param stream The stream to enqueue the buffer fill task into.
        //-----------------------------------------------------------------------------
        template<
            typename TBufDst,
            typename TBufSrc,
            typename TExtents,
            typename TStream>
        ALPAKA_FCT_HOST auto copy(
            TBufDst & bufDst,
            TBufSrc const & bufSrc,
            TExtents const & extents,
            TStream const & stream)
        -> void
        {
            static_assert(
                dim::DimT<TBufDst>::value == dim::DimT<TBufSrc>::value,
                "The source and the destination buffers are required to have the same dimensionality!");
            static_assert(
                dim::DimT<TBufDst>::value == dim::DimT<TExtents>::value,
                "The destination buffer and the extents are required to have the same dimensionality!");
            static_assert(
                std::is_same<ElemT<TBufDst>, ElemT<TBufSrc>>::value,
                "The source and the destination buffers are required to have the same element type!");

            traits::mem::Copy<
                dim::DimT<TBufDst>,
                SpaceT<TBufDst>,
                SpaceT<TBufSrc>>
            ::copy(
                bufDst,
                bufSrc,
                extents,
                stream);
        }

        //#############################################################################
        //! The memory buffer view type trait alias template to remove the ::type.
        //#############################################################################
        template<
            typename TElem,
            typename TDim,
            typename TDev>
        using ViewT = typename traits::mem::ViewType<TElem, TDim, TDev>::type;

        //-----------------------------------------------------------------------------
        //! Constructor.
        //! \param buf This can be either a memory buffer base or a memory buffer view itself.
        //-----------------------------------------------------------------------------
        template<
            typename TView,
            typename TBuf>
        ALPAKA_FCT_HOST auto createView(
            TBuf const & buf)
        -> decltype(traits::mem::CreateView<TView>::createView(buf))
        {
            return traits::mem::CreateView<
                TView>
            ::createView(
                buf);
        }
        //-----------------------------------------------------------------------------
        //! Constructor.
        //! \param buf This can be either a memory buffer base or a memory buffer view itself.
        //-----------------------------------------------------------------------------
        template<
            typename TView,
            typename TBuf>
        ALPAKA_FCT_HOST auto createView(
            TBuf & buf)
        -> decltype(traits::mem::CreateView<TView>::createView(buf))
        {
            return traits::mem::CreateView<
                TView>
            ::createView(
                buf);
        }
        //-----------------------------------------------------------------------------
        //! Constructor.
        //! \param buf This can be either a memory buffer base or a memory buffer view itself.
        //! \param offsetsElements The offsets in elements.
        //! \param extentsElements The extents in elements.
        //-----------------------------------------------------------------------------
        template<
            typename TView,
            typename TBuf,
            typename TExtents,
            typename TOffsets>
        ALPAKA_FCT_HOST auto createView(
            TBuf const & buf,
            TExtents const & extentsElements,
            TOffsets const & relativeOffsetsElements = TOffsets())
        -> decltype(traits::mem::CreateView<TView>::createView(buf, extentsElements, relativeOffsetsElements))
        {
            return traits::mem::CreateView<
                TView>
            ::createView(
                buf,
                extentsElements,
                relativeOffsetsElements);
        }
        //-----------------------------------------------------------------------------
        //! Constructor.
        //! \param buf This can be either a memory buffer base or a memory buffer view itself.
        //! \param offsetsElements The offsets in elements.
        //! \param extentsElements The extents in elements.
        //-----------------------------------------------------------------------------
        template<
            typename TView,
            typename TBuf,
            typename TExtents,
            typename TOffsets>
        ALPAKA_FCT_HOST auto createView(
            TBuf & buf,
            TExtents const & extentsElements,
            TOffsets const & relativeOffsetsElements = TOffsets())
        -> decltype(traits::mem::CreateView<TView>::createView(buf, extentsElements, relativeOffsetsElements))
        {
            return traits::mem::CreateView<
                TView>
            ::createView(
                buf,
                extentsElements,
                relativeOffsetsElements);
        }

        //-----------------------------------------------------------------------------
        //! Gets the base memory buffer.
        //!
        //! \param buf The memory buffer.
        //! \return The base buffer.
        //-----------------------------------------------------------------------------
        template<
            typename TBuf>
        ALPAKA_FCT_HOST auto getBase(
            TBuf const & buf)
        -> decltype(traits::mem::GetBase<TBuf>::getBase(buf))
        {
            return traits::mem::GetBase<
                TBuf>
            ::getBase(
                buf);
        }

        //-----------------------------------------------------------------------------
        //! Gets the base memory buffer.
        //!
        //! \param buf The memory buffer.
        //! \return The base buffer.
        //-----------------------------------------------------------------------------
        template<
            typename TBuf>
        ALPAKA_FCT_HOST auto getBase(
            TBuf & buf)
        -> decltype(traits::mem::GetBase<TBuf>::getBase(buf))
        {
            return traits::mem::GetBase<
                TBuf>
            ::getBase(
                buf);
        }
    }
}
