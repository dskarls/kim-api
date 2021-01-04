//
// CDDL HEADER START
//
// The contents of this file are subject to the terms of the Common
// Development and Distribution License Version 1.0 (the "License").
//
// You can obtain a copy of the license at
// http://www.opensource.org/licenses/CDDL-1.0.  See the License for the
// specific language governing permissions and limitations under the License.
//
// When distributing Covered Code, include this CDDL HEADER in each file and
// include the License file in a prominent location with the name
// LICENSE.CDDL.
// If applicable, add the following below this CDDL HEADER, with the fields
// enclosed by brackets "[]" replaced with your own identifying information:
//
// Portions Copyright (c) [yyyy] [name of copyright owner].
// All rights reserved.
//
// CDDL HEADER END
//

//
// Copyright (c) 2016--2021, Regents of the University of Minnesota.
// All rights reserved.
//
// Contributors:
//    Ryan S. Elliott
//

//
// Release: This file is part of the kim-api.git repository.
//


#include <string>

#ifndef KIM_NUMBERING_HPP_
#include "KIM_Numbering.hpp"
#endif
extern "C" {
#ifndef KIM_NUMBERING_H_
#include "KIM_Numbering.h"
#endif
}  // extern "C"

namespace
{
KIM::Numbering makeNumberingCpp(KIM_Numbering const numbering)
{
  KIM::Numbering const * const numberingCpp
      = reinterpret_cast<KIM::Numbering const *>(&numbering);
  return *numberingCpp;
}

KIM_Numbering makeNumberingC(KIM::Numbering const numbering)
{
  KIM_Numbering const * const numberingC
      = reinterpret_cast<KIM_Numbering const *>(&numbering);
  return *numberingC;
}
}  // namespace

extern "C" {
KIM_Numbering KIM_Numbering_FromString(char const * const str)
{
  return makeNumberingC(KIM::Numbering(std::string(str)));
}

int KIM_Numbering_Known(KIM_Numbering const numbering)
{
  return makeNumberingCpp(numbering).Known();
}

int KIM_Numbering_Equal(KIM_Numbering const lhs, KIM_Numbering const rhs)
{
  return (lhs.numberingID == rhs.numberingID);
}

int KIM_Numbering_NotEqual(KIM_Numbering const lhs, KIM_Numbering const rhs)
{
  return (!KIM_Numbering_Equal(lhs, rhs));
}

char const * KIM_Numbering_ToString(KIM_Numbering const numbering)
{
  return makeNumberingCpp(numbering).ToString().c_str();
}

#include "KIM_Numbering.inc"
KIM_Numbering const KIM_NUMBERING_zeroBased = {ID_zeroBased};
KIM_Numbering const KIM_NUMBERING_oneBased = {ID_oneBased};

void KIM_NUMBERING_GetNumberOfNumberings(int * const numberOfNumberings)
{
  KIM::NUMBERING::GetNumberOfNumberings(numberOfNumberings);
}

int KIM_NUMBERING_GetNumbering(int const index, KIM_Numbering * const numbering)
{
  KIM::Numbering numberingCpp;
  int error = KIM::NUMBERING::GetNumbering(index, &numberingCpp);
  if (error) return error;
  *numbering = makeNumberingC(numberingCpp);
  return false;
}

}  // extern "C"
