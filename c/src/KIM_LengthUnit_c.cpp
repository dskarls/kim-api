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

#ifndef KIM_LENGTH_UNIT_HPP_
#include "KIM_LengthUnit.hpp"
#endif
extern "C" {
#ifndef KIM_LENGTH_UNIT_H_
#include "KIM_LengthUnit.h"
#endif
}


namespace
{
KIM::LengthUnit makeLengthUnitCpp(KIM_LengthUnit const lengthUnit)
{
  KIM::LengthUnit const * const lengthUnitCpp
      = reinterpret_cast<KIM::LengthUnit const *>(&lengthUnit);
  return *lengthUnitCpp;
}

KIM_LengthUnit makeLengthUnitC(KIM::LengthUnit const lengthUnit)
{
  KIM_LengthUnit const * const lengthUnitC
      = reinterpret_cast<KIM_LengthUnit const *>(&lengthUnit);
  return *lengthUnitC;
}
}  // namespace

extern "C" {
KIM_LengthUnit KIM_LengthUnit_FromString(char const * const str)
{
  return makeLengthUnitC(KIM::LengthUnit(std::string(str)));
}

int KIM_LengthUnit_Known(KIM_LengthUnit const lengthUnit)
{
  return makeLengthUnitCpp(lengthUnit).Known();
}

int KIM_LengthUnit_Equal(KIM_LengthUnit const lhs, KIM_LengthUnit const rhs)
{
  return (lhs.lengthUnitID == rhs.lengthUnitID);
}

int KIM_LengthUnit_NotEqual(KIM_LengthUnit const lhs, KIM_LengthUnit const rhs)
{
  return (!KIM_LengthUnit_Equal(lhs, rhs));
}

char const * KIM_LengthUnit_ToString(KIM_LengthUnit const lengthUnit)
{
  return makeLengthUnitCpp(lengthUnit).ToString().c_str();
}

#include "KIM_LengthUnit.inc"
KIM_LengthUnit const KIM_LENGTH_UNIT_unused = {ID_unused};
KIM_LengthUnit const KIM_LENGTH_UNIT_A = {ID_A};
KIM_LengthUnit const KIM_LENGTH_UNIT_Bohr = {ID_Bohr};
KIM_LengthUnit const KIM_LENGTH_UNIT_cm = {ID_cm};
KIM_LengthUnit const KIM_LENGTH_UNIT_m = {ID_m};
KIM_LengthUnit const KIM_LENGTH_UNIT_nm = {ID_nm};

void KIM_LENGTH_UNIT_GetNumberOfLengthUnits(int * const numberOfLengthUnits)
{
  KIM::LENGTH_UNIT::GetNumberOfLengthUnits(numberOfLengthUnits);
}

int KIM_LENGTH_UNIT_GetLengthUnit(int const index,
                                  KIM_LengthUnit * const lengthUnit)
{
  KIM::LengthUnit lengthUnitCpp;
  int error = KIM::LENGTH_UNIT::GetLengthUnit(index, &lengthUnitCpp);
  if (error) return error;
  *lengthUnit = makeLengthUnitC(lengthUnitCpp);
  return false;
}

}  // extern "C"
