#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the Common Development
# and Distribution License Version 1.0 (the "License").
#
# You can obtain a copy of the license at
# http://www.opensource.org/licenses/CDDL-1.0.  See the License for the
# specific language governing permissions and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each file and
# include the License file in a prominent location with the name LICENSE.CDDL.
# If applicable, add the following below this CDDL HEADER, with the fields
# enclosed by brackets "[]" replaced with your own identifying information:
#
# Portions Copyright (c) [yyyy] [name of copyright owner]. All rights reserved.
#
# CDDL HEADER END
#

#
# Copyright (c) 2013--2021, Regents of the University of Minnesota.
# All rights reserved.
#
# Contributors:
#    Richard Berger
#    Christoph Junghans
#    Ryan S. Elliott
#

#
# Release: This file is part of the kim-api.git repository.
#


include(CheckCCompilerFlag)

function(enable_c_compiler_flag_if_supported _flag)
  if(NOT KIM_API_C_FLAGS)
    set(KIM_API_C_FLAGS "")
  endif()
  string(MAKE_C_IDENTIFIER "${_flag}" _cid_flag)
  string(FIND "${KIM_API_C_FLAGS}" "${_flag}" _flag_already_set)
  if(_flag_already_set EQUAL -1)
    check_c_compiler_flag("${_flag}" c_support_for_${_cid_flag})
    if(c_support_for_${_cid_flag})
      set(KIM_API_C_FLAGS "${KIM_API_C_FLAGS} ${_flag}" PARENT_SCOPE)
    endif()
    unset(c_support_for_${_cid_flag} CACHE)
  endif()
endfunction(enable_c_compiler_flag_if_supported)
