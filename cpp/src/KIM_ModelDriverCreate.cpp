//
// CDDL HEADER START
//
// The contents of this file are subject to the terms of the Common Development
// and Distribution License Version 1.0 (the "License").
//
// You can obtain a copy of the license at
// http://www.opensource.org/licenses/CDDL-1.0.  See the License for the
// specific language governing permissions and limitations under the License.
//
// When distributing Covered Code, include this CDDL HEADER in each file and
// include the License file in a prominent location with the name LICENSE.CDDL.
// If applicable, add the following below this CDDL HEADER, with the fields
// enclosed by brackets "[]" replaced with your own identifying information:
//
// Portions Copyright (c) [yyyy] [name of copyright owner]. All rights reserved.
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


#include <cstddef>

#ifndef KIM_LOG_VERBOSITY_HPP_
#include "KIM_LogVerbosity.hpp"
#endif

#ifndef KIM_LANGUAGE_NAME_HPP_
#include "KIM_LanguageName.hpp"
#endif

#ifndef KIM_MODEL_ROUTINE_NAME_HPP_
#include "KIM_ModelRoutineName.hpp"
#endif

#ifndef KIM_NUMBERING_HPP_
#include "KIM_Numbering.hpp"
#endif

#ifndef KIM_SPECIES_NAME_HPP_
#include "KIM_SpeciesName.hpp"
#endif

#ifndef KIM_UNIT_SYSTEM
#include "KIM_UnitSystem.hpp"
#endif

#ifndef KIM_MODEL_DRIVER_CREATE_HPP_
#include "KIM_ModelDriverCreate.hpp"
#endif

#ifndef KIM_MODEL_IMPLEMENTATION_HPP_
#include "KIM_ModelImplementation.hpp"
#endif

#define CONVERT_POINTER \
  ModelImplementation * pImpl = reinterpret_cast<ModelImplementation *>(pimpl)


namespace KIM
{
void ModelDriverCreate::GetParameterFileDirectoryName(
    std::string const ** const directoryName) const
{
  CONVERT_POINTER;

  pImpl->GetParameterFileDirectoryName(directoryName);
}

void ModelDriverCreate::GetNumberOfParameterFiles(
    int * const numberOfParameterFiles) const
{
  CONVERT_POINTER;

  pImpl->GetNumberOfParameterFiles(numberOfParameterFiles);
}

int ModelDriverCreate::GetParameterFileName(
    int const index, std::string const ** const parameterFileName) const
{
  CONVERT_POINTER;

  return pImpl->GetParameterFileName(index, parameterFileName);
}
int ModelDriverCreate::GetParameterFileBasename(
    int const index, std::string const ** const parameterFileBasename) const
{
  CONVERT_POINTER;

  return pImpl->GetParameterFileBasename(index, parameterFileBasename);
}


int ModelDriverCreate::SetModelNumbering(Numbering const numbering)
{
  CONVERT_POINTER;

  return pImpl->SetModelNumbering(numbering);
}

void ModelDriverCreate::SetInfluenceDistancePointer(
    double const * const influenceDistance)
{
  CONVERT_POINTER;

  pImpl->SetInfluenceDistancePointer(influenceDistance);
}

void ModelDriverCreate::SetNeighborListPointers(
    int const numberOfNeighborLists,
    double const * const cutoffs,
    int const * const modelWillNotRequestNeighborsOfNoncontributingParticles)
{
  CONVERT_POINTER;

  pImpl->SetNeighborListPointers(
      numberOfNeighborLists,
      cutoffs,
      modelWillNotRequestNeighborsOfNoncontributingParticles);
}

int ModelDriverCreate::SetRoutinePointer(
    ModelRoutineName const modelRoutineName,
    LanguageName const languageName,
    int const required,
    Function * const fptr)
{
  CONVERT_POINTER;

  return pImpl->SetRoutinePointer(
      modelRoutineName, languageName, required, fptr);
}

int ModelDriverCreate::SetSpeciesCode(SpeciesName const speciesName,
                                      int const code)
{
  CONVERT_POINTER;

  return pImpl->SetSpeciesCode(speciesName, code);
}

int ModelDriverCreate::SetParameterPointer(int const extent,
                                           int * const ptr,
                                           std::string const & name,
                                           std::string const & description)
{
  CONVERT_POINTER;

  return pImpl->SetParameterPointer(extent, ptr, name, description);
}

int ModelDriverCreate::SetParameterPointer(int const extent,
                                           double * const ptr,
                                           std::string const & name,
                                           std::string const & description)
{
  CONVERT_POINTER;

  return pImpl->SetParameterPointer(extent, ptr, name, description);
}

void ModelDriverCreate::SetModelBufferPointer(void * const ptr)
{
  CONVERT_POINTER;

  pImpl->SetModelBufferPointer(ptr);
}


int ModelDriverCreate::SetUnits(LengthUnit const lengthUnit,
                                EnergyUnit const energyUnit,
                                ChargeUnit const chargeUnit,
                                TemperatureUnit const temperatureUnit,
                                TimeUnit const timeUnit)
{
  CONVERT_POINTER;

  return pImpl->SetUnits(
      lengthUnit, energyUnit, chargeUnit, temperatureUnit, timeUnit);
}


int ModelDriverCreate::ConvertUnit(LengthUnit const fromLengthUnit,
                                   EnergyUnit const fromEnergyUnit,
                                   ChargeUnit const fromChargeUnit,
                                   TemperatureUnit const fromTemperatureUnit,
                                   TimeUnit const fromTimeUnit,
                                   LengthUnit const toLengthUnit,
                                   EnergyUnit const toEnergyUnit,
                                   ChargeUnit const toChargeUnit,
                                   TemperatureUnit const toTemperatureUnit,
                                   TimeUnit const toTimeUnit,
                                   double const lengthExponent,
                                   double const energyExponent,
                                   double const chargeExponent,
                                   double const temperatureExponent,
                                   double const timeExponent,
                                   double * const conversionFactor)
{
  return ModelImplementation::ConvertUnit(fromLengthUnit,
                                          fromEnergyUnit,
                                          fromChargeUnit,
                                          fromTemperatureUnit,
                                          fromTimeUnit,
                                          toLengthUnit,
                                          toEnergyUnit,
                                          toChargeUnit,
                                          toTemperatureUnit,
                                          toTimeUnit,
                                          lengthExponent,
                                          energyExponent,
                                          chargeExponent,
                                          temperatureExponent,
                                          timeExponent,
                                          conversionFactor);
}

void ModelDriverCreate::LogEntry(LogVerbosity const logVerbosity,
                                 std::string const & message,
                                 int const lineNumber,
                                 std::string const & fileName) const
{
  CONVERT_POINTER;

  pImpl->LogEntry(logVerbosity, message, lineNumber, fileName);
}

void ModelDriverCreate::LogEntry(LogVerbosity const logVerbosity,
                                 std::stringstream const & message,
                                 int const lineNumber,
                                 std::string const & fileName) const
{
  CONVERT_POINTER;

  pImpl->LogEntry(logVerbosity, message, lineNumber, fileName);
}

std::string const & ModelDriverCreate::ToString() const
{
  CONVERT_POINTER;

  return pImpl->ToString();
}

ModelDriverCreate::ModelDriverCreate() : pimpl(NULL) {}

ModelDriverCreate::~ModelDriverCreate() {}

}  // namespace KIM
