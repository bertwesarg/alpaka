#!/bin/bash

#
# Copyright 2017 Benjamin Worpitz
#
# This file is part of alpaka.
#
# alpaka is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# alpaka is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with alpaka.
# If not, see <http://www.gnu.org/licenses/>.
#

source ./script/travis/travis_retry.sh

#-------------------------------------------------------------------------------
# e: exit as soon as one command returns a non-zero exit code.
set -e

travis_retry sudo apt-get -y --quiet --allow-unauthenticated install g++-"${ALPAKA_GCC_VER}"
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-"${ALPAKA_GCC_VER}" 50
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-"${ALPAKA_GCC_VER}" 50
if [[ "${ALPAKA_CI_SANITIZERS}" == *"TSan"* ]]
then
    travis_retry sudo apt-get -y --quiet --allow-unauthenticated install libtsan0
fi

which "${CXX}"
${CXX} -v