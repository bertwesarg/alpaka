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

#include <alpaka/host/Event.hpp>    // EventHost

#include <alpaka/traits/Acc.hpp>    // AccType

namespace alpaka
{
    namespace accs
    {
        namespace serial
        {
            namespace detail
            {
                class AccSerial;
                class DevSerial;

                //#############################################################################
                //! The serial accelerator event.
                //#############################################################################
                using EventSerial = host::detail::EventHost<DevSerial>;
            }
        }
    }

    namespace traits
    {
        namespace acc
        {
            //#############################################################################
            //! The serial accelerator event accelerator type trait specialization.
            //#############################################################################
            template<>
            struct AccType<
                accs::serial::detail::EventSerial>
            {
                using type = accs::serial::detail::AccSerial;
            };
        }
    }
}
