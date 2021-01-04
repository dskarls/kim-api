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


#ifndef KIM_FUNCTION_TYPES_HPP_
#define KIM_FUNCTION_TYPES_HPP_

#include <string>

namespace KIM
{
// Forward declarations
class LengthUnit;
class EnergyUnit;
class ChargeUnit;
class TemperatureUnit;
class TimeUnit;
class ModelCreate;
class ModelDriverCreate;
class ModelCompute;
class ModelExtension;
class ModelComputeArgumentsCreate;
class ModelComputeArguments;
class ModelRefresh;
class ModelWriteParameterizedModel;
class ModelComputeArgumentsDestroy;
class ModelDestroy;

/// \brief Generic function type.
///
/// \sa KIM_Function
///
/// \since 2.0
typedef void(Function)(void);  // Generic function pointer

/// \brief Prototype for MODEL_ROUTINE_NAME::Create routine.
///
/// \sa KIM_ModelCreateFunction, kim_model_module::kim_model_create
///
/// \since 2.0
typedef int ModelCreateFunction(ModelCreate * const modelCreate,
                                LengthUnit const requestedLengthUnit,
                                EnergyUnit const requestedEnergyUnit,
                                ChargeUnit const requestedChargeUnit,
                                TemperatureUnit const requestedTemperatureUnit,
                                TimeUnit const requestedTimeUnit);

/// \brief Prototype for MODEL_ROUTINE_NAME::Create routine.
///
/// \sa KIM_ModelDriverCreateFunction, kim_model_module::kim_model_create
///
/// \since 2.0
typedef int
ModelDriverCreateFunction(ModelDriverCreate * const modelDriverCreate,
                          LengthUnit const requestedLengthUnit,
                          EnergyUnit const requestedEnergyUnit,
                          ChargeUnit const requestedChargeUnit,
                          TemperatureUnit const requestedTemperatureUnit,
                          TimeUnit const requestedTimeUnit);

/// \brief Prototype for MODEL_ROUTINE_NAME::ComputeArgumentsCreate
/// routine.
///
/// \sa KIM_ModelComputeArgumentsCreateFunction,
/// kim_model_module::kim_model_compute_arguments_create
///
/// \since 2.0
typedef int ModelComputeArgumentsCreateFunction(
    ModelCompute const * const modelCompute,
    ModelComputeArgumentsCreate * const modelComputeArgumentsCreate);

/// \brief Prototype for MODEL_ROUTINE_NAME::Compute routine.
///
/// \sa KIM_ModelComputeFunction, kim_model_module::kim_model_compute
///
/// \since 2.0
typedef int ModelComputeFunction(
    ModelCompute const * const modelCompute,
    ModelComputeArguments const * const modelComputeArgumentsCreate);

/// \brief Prototype for COMPUTE_CALLBACK_NAME::GetNeighborList routine.
///
/// \sa KIM_GetNeighborListFunction, kim_model_compute_arguments_module::<!--
/// -->kim_model_compute_arguments_get_neighbor_list
///
/// \since 2.0
typedef int GetNeighborListFunction(void * const dataObject,
                                    int const numberOfNeighborLists,
                                    double const * const cutoffs,
                                    int const neighborListIndex,
                                    int const particleNumber,
                                    int * const numberOfNeighbors,
                                    int const ** const neighborsOfParticle);

/// \brief Prototype for COMPUTE_CALLBACK_NAME::ProcessDEDrTerm
/// routine.
///
/// \sa KIM_ProcessDEDrTermFunction, kim_model_compute_arguments_module::<!--
/// -->kim_model_compute_arguments_process_dedr_term
///
/// \since 2.0
typedef int ProcessDEDrTermFunction(void * const dataObject,
                                    double const de,
                                    double const r,
                                    double const * const dx,
                                    int const i,
                                    int const j);

/// \brief Prototype for COMPUTE_CALLBACK_NAME::ProcessD2EDr2Term
/// routine.
///
/// \sa KIM_ProcessD2EDr2TermFunction, kim_model_compute_arguments_module::<!--
/// -->kim_model_compute_arguments_process_d2edr2_term
///
///
/// \since 2.0
typedef int ProcessD2EDr2TermFunction(void * const dataObject,
                                      double const de,
                                      double const * const r,
                                      double const * const dx,
                                      int const * const i,
                                      int const * const j);

/// \brief Prototype for MODEL_ROUTINE_NAME::Extension routine.
///
/// \sa KIM_ModelExtensionFunction, kim_model_module::kim_model_extension
///
/// \since 2.0
typedef int ModelExtensionFunction(ModelExtension * const modelExtension,
                                   void * const extensionStructure);

/// \brief Prototype for MODEL_ROUTINE_NAME::Refresh routine.
///
/// \sa KIM_ModelRefreshFunction,
/// kim_model_module::kim_model_clear_then_refresh
///
/// \since 2.0
typedef int ModelRefreshFunction(ModelRefresh * const modelRefresh);

/// \brief Prototype for MODEL_ROUTINE_NAME::WriteParameterizedModel routine.
///
/// \sa KIM_ModelWriteParameterizedModelFunction,
/// kim_model_module::kim_model_write_parameterized_model
///
/// \since 2.0
typedef int ModelWriteParameterizedModelFunction(
    ModelWriteParameterizedModel const * const modelWriteParameterizedModel);

/// \brief Prototype for MODEL_ROUTINE_NAME::ComputeArgumentsDestroy
/// routine.
///
/// \sa KIM_ModelComputeArgumentsDestroyFunction,
/// kim_model_module::kim_model_compute_arguments_destroy
///
/// \since 2.0
typedef int ModelComputeArgumentsDestroyFunction(
    ModelCompute const * const modelCompute,
    ModelComputeArgumentsDestroy * const modelComputeArgumentsDestroy);

/// \brief Prototype for MODEL_ROUTINE_NAME::Destroy routine.
///
/// \sa KIM_ModelDestroyFunction, kim_model_module::kim_model_destroy
///
/// \since 2.0
typedef int ModelDestroyFunction(ModelDestroy * const modelDestroy);

/// \brief Prototype for Log PrintFunction routine.
///
/// \sa KIM_LogPrintFunction,
/// kim_log_module::kim_log_push_default_print_function
///
/// \since 2.2
typedef int LogPrintFunction(std::string const & entryString);
}  // namespace KIM

#endif  // KIM_FUNCTION_TYPES_HPP_
