/*******************************************************************************
 Copyright © 2016, STMicroelectronics International N.V.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of STMicroelectronics nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED.
 IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "vl53l0x_api.h"
#include "vl53l0x_api_core.h"
#include "vl53l0x_api_strings.h"

#ifndef __KERNEL__
#include <stdlib.h>
#endif

#define LOG_FUNCTION_START(fmt, ...) \
    _LOG_FUNCTION_START(TRACE_MODULE_API, fmt, ##__VA_ARGS__)
#define LOG_FUNCTION_END(status, ...) \
    _LOG_FUNCTION_END(TRACE_MODULE_API, status, ##__VA_ARGS__)
#define LOG_FUNCTION_END_FMT(status, fmt, ...) \
    _LOG_FUNCTION_END_FMT(TRACE_MODULE_API, status, fmt, ##__VA_ARGS__)


VL53L0X_Error VL53L0X_check_part_used(VL53L0X_DEV Dev,
                                      uint8_t *Revision,
                                      VL53L0X_DeviceInfo_t *pVL53L0X_DeviceInfo)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t ModuleIdInt;
    char *ProductId_tmp;

    LOG_FUNCTION_START("");

    Status = VL53L0X_get_info_from_device(Dev, 2);

    if (Status == VL53L0X_ERROR_NONE) {
        ModuleIdInt = VL53L0X_GETDEVICESPECIFICPARAMETER(Dev, ModuleId);

        if (ModuleIdInt == 0) {
            *Revision = 0;
            VL53L0X_COPYSTRING(pVL53L0X_DeviceInfo->ProductId, "");
        } else {
            *Revision = VL53L0X_GETDEVICESPECIFICPARAMETER(Dev, Revision);
            ProductId_tmp = VL53L0X_GETDEVICESPECIFICPARAMETER(Dev,
                            ProductId);
            VL53L0X_COPYSTRING(pVL53L0X_DeviceInfo->ProductId, ProductId_tmp);
        }
    }

    LOG_FUNCTION_END(Status);
    return Status;
}


VL53L0X_Error VL53L0X_get_device_info(VL53L0X_DEV Dev,
                                      VL53L0X_DeviceInfo_t *pVL53L0X_DeviceInfo)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t revision_id;
    uint8_t Revision;

    Status = VL53L0X_check_part_used(Dev, &Revision, pVL53L0X_DeviceInfo);

    if (Status == VL53L0X_ERROR_NONE) {
        if (Revision == 0) {
            VL53L0X_COPYSTRING(pVL53L0X_DeviceInfo->Name,
                               VL53L0X_STRING_DEVICE_INFO_NAME_TS0);
        } else if ((Revision <= 34) && (Revision != 32)) {
            VL53L0X_COPYSTRING(pVL53L0X_DeviceInfo->Name,
                               VL53L0X_STRING_DEVICE_INFO_NAME_TS1);
        } else if (Revision < 39) {
            VL53L0X_COPYSTRING(pVL53L0X_DeviceInfo->Name,
                               VL53L0X_STRING_DEVICE_INFO_NAME_TS2);
        } else {
            VL53L0X_COPYSTRING(pVL53L0X_DeviceInfo->Name,
                               VL53L0X_STRING_DEVICE_INFO_NAME_ES1);
        }

        VL53L0X_COPYSTRING(pVL53L0X_DeviceInfo->Type,
                           VL53L0X_STRING_DEVICE_INFO_TYPE);

    }

    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_RdByte(Dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID,
                                &pVL53L0X_DeviceInfo->ProductType);
    }

    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_RdByte(Dev,
                                VL53L0X_REG_IDENTIFICATION_REVISION_ID,
                                &revision_id);
        pVL53L0X_DeviceInfo->ProductRevisionMajor = 1;
        pVL53L0X_DeviceInfo->ProductRevisionMinor =
            (revision_id & 0xF0) >> 4;
    }

    return Status;
}


VL53L0X_Error VL53L0X_get_device_error_string(VL53L0X_DeviceError ErrorCode,
        char *pDeviceErrorString)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    LOG_FUNCTION_START("");

    switch (ErrorCode) {
    case VL53L0X_DEVICEERROR_NONE:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_NONE);
        break;
    case VL53L0X_DEVICEERROR_VCSELCONTINUITYTESTFAILURE:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_VCSELCONTINUITYTESTFAILURE);
        break;
    case VL53L0X_DEVICEERROR_VCSELWATCHDOGTESTFAILURE:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_VCSELWATCHDOGTESTFAILURE);
        break;
    case VL53L0X_DEVICEERROR_NOVHVVALUEFOUND:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_NOVHVVALUEFOUND);
        break;
    case VL53L0X_DEVICEERROR_MSRCNOTARGET:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_MSRCNOTARGET);
        break;
    case VL53L0X_DEVICEERROR_SNRCHECK:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_SNRCHECK);
        break;
    case VL53L0X_DEVICEERROR_RANGEPHASECHECK:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_RANGEPHASECHECK);
        break;
    case VL53L0X_DEVICEERROR_SIGMATHRESHOLDCHECK:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_SIGMATHRESHOLDCHECK);
        break;
    case VL53L0X_DEVICEERROR_TCC:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_TCC);
        break;
    case VL53L0X_DEVICEERROR_PHASECONSISTENCY:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_PHASECONSISTENCY);
        break;
    case VL53L0X_DEVICEERROR_MINCLIP:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_MINCLIP);
        break;
    case VL53L0X_DEVICEERROR_RANGECOMPLETE:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_RANGECOMPLETE);
        break;
    case VL53L0X_DEVICEERROR_ALGOUNDERFLOW:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_ALGOUNDERFLOW);
        break;
    case VL53L0X_DEVICEERROR_ALGOOVERFLOW:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_ALGOOVERFLOW);
        break;
    case VL53L0X_DEVICEERROR_RANGEIGNORETHRESHOLD:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_DEVICEERROR_RANGEIGNORETHRESHOLD);
        break;

    default:
        VL53L0X_COPYSTRING(pDeviceErrorString,
                           VL53L0X_STRING_UNKNOW_ERROR_CODE);

    }

    LOG_FUNCTION_END(Status);
    return Status;
}

VL53L0X_Error VL53L0X_get_range_status_string(uint8_t RangeStatus,
        char *pRangeStatusString)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    LOG_FUNCTION_START("");

    switch (RangeStatus) {
    case 0:
        VL53L0X_COPYSTRING(pRangeStatusString,
                           VL53L0X_STRING_RANGESTATUS_RANGEVALID);
        break;
    case 1:
        VL53L0X_COPYSTRING(pRangeStatusString,
                           VL53L0X_STRING_RANGESTATUS_SIGMA);
        break;
    case 2:
        VL53L0X_COPYSTRING(pRangeStatusString,
                           VL53L0X_STRING_RANGESTATUS_SIGNAL);
        break;
    case 3:
        VL53L0X_COPYSTRING(pRangeStatusString,
                           VL53L0X_STRING_RANGESTATUS_MINRANGE);
        break;
    case 4:
        VL53L0X_COPYSTRING(pRangeStatusString,
                           VL53L0X_STRING_RANGESTATUS_PHASE);
        break;
    case 5:
        VL53L0X_COPYSTRING(pRangeStatusString,
                           VL53L0X_STRING_RANGESTATUS_HW);
        break;

    default: /**/
        VL53L0X_COPYSTRING(pRangeStatusString,
                           VL53L0X_STRING_RANGESTATUS_NONE);
    }

    LOG_FUNCTION_END(Status);
    return Status;
}

VL53L0X_Error VL53L0X_get_pal_error_string(VL53L0X_Error PalErrorCode,
        char *pPalErrorString)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    LOG_FUNCTION_START("");

    switch (PalErrorCode) {
    case VL53L0X_ERROR_NONE:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_NONE);
        break;
    case VL53L0X_ERROR_CALIBRATION_WARNING:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_CALIBRATION_WARNING);
        break;
    case VL53L0X_ERROR_MIN_CLIPPED:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_MIN_CLIPPED);
        break;
    case VL53L0X_ERROR_UNDEFINED:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_UNDEFINED);
        break;
    case VL53L0X_ERROR_INVALID_PARAMS:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_INVALID_PARAMS);
        break;
    case VL53L0X_ERROR_NOT_SUPPORTED:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_NOT_SUPPORTED);
        break;
    case VL53L0X_ERROR_INTERRUPT_NOT_CLEARED:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_INTERRUPT_NOT_CLEARED);
        break;
    case VL53L0X_ERROR_RANGE_ERROR:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_RANGE_ERROR);
        break;
    case VL53L0X_ERROR_TIME_OUT:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_TIME_OUT);
        break;
    case VL53L0X_ERROR_MODE_NOT_SUPPORTED:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_MODE_NOT_SUPPORTED);
        break;
    case VL53L0X_ERROR_BUFFER_TOO_SMALL:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_BUFFER_TOO_SMALL);
        break;
    case VL53L0X_ERROR_GPIO_NOT_EXISTING:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_GPIO_NOT_EXISTING);
        break;
    case VL53L0X_ERROR_GPIO_FUNCTIONALITY_NOT_SUPPORTED:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_GPIO_FUNCTIONALITY_NOT_SUPPORTED);
        break;
    case VL53L0X_ERROR_CONTROL_INTERFACE:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_CONTROL_INTERFACE);
        break;
    case VL53L0X_ERROR_INVALID_COMMAND:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_INVALID_COMMAND);
        break;
    case VL53L0X_ERROR_DIVISION_BY_ZERO:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_DIVISION_BY_ZERO);
        break;
    case VL53L0X_ERROR_REF_SPAD_INIT:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_REF_SPAD_INIT);
        break;
    case VL53L0X_ERROR_NOT_IMPLEMENTED:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_ERROR_NOT_IMPLEMENTED);
        break;

    default:
        VL53L0X_COPYSTRING(pPalErrorString,
                           VL53L0X_STRING_UNKNOW_ERROR_CODE);
    }

    LOG_FUNCTION_END(Status);
    return Status;
}

VL53L0X_Error VL53L0X_get_pal_state_string(VL53L0X_State PalStateCode,
        char *pPalStateString)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    LOG_FUNCTION_START("");

    switch (PalStateCode) {
    case VL53L0X_STATE_POWERDOWN:
        VL53L0X_COPYSTRING(pPalStateString,
                           VL53L0X_STRING_STATE_POWERDOWN);
        break;
    case VL53L0X_STATE_WAIT_STATICINIT:
        VL53L0X_COPYSTRING(pPalStateString,
                           VL53L0X_STRING_STATE_WAIT_STATICINIT);
        break;
    case VL53L0X_STATE_STANDBY:
        VL53L0X_COPYSTRING(pPalStateString,
                           VL53L0X_STRING_STATE_STANDBY);
        break;
    case VL53L0X_STATE_IDLE:
        VL53L0X_COPYSTRING(pPalStateString,
                           VL53L0X_STRING_STATE_IDLE);
        break;
    case VL53L0X_STATE_RUNNING:
        VL53L0X_COPYSTRING(pPalStateString,
                           VL53L0X_STRING_STATE_RUNNING);
        break;
    case VL53L0X_STATE_UNKNOWN:
        VL53L0X_COPYSTRING(pPalStateString,
                           VL53L0X_STRING_STATE_UNKNOWN);
        break;
    case VL53L0X_STATE_ERROR:
        VL53L0X_COPYSTRING(pPalStateString,
                           VL53L0X_STRING_STATE_ERROR);
        break;

    default:
        VL53L0X_COPYSTRING(pPalStateString,
                           VL53L0X_STRING_STATE_UNKNOWN);
    }

    LOG_FUNCTION_END(Status);
    return Status;
}

VL53L0X_Error VL53L0X_get_sequence_steps_info(
    VL53L0X_SequenceStepId SequenceStepId,
    char *pSequenceStepsString)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    LOG_FUNCTION_START("");

    switch (SequenceStepId) {
    case VL53L0X_SEQUENCESTEP_TCC:
        VL53L0X_COPYSTRING(pSequenceStepsString,
                           VL53L0X_STRING_SEQUENCESTEP_TCC);
        break;
    case VL53L0X_SEQUENCESTEP_DSS:
        VL53L0X_COPYSTRING(pSequenceStepsString,
                           VL53L0X_STRING_SEQUENCESTEP_DSS);
        break;
    case VL53L0X_SEQUENCESTEP_MSRC:
        VL53L0X_COPYSTRING(pSequenceStepsString,
                           VL53L0X_STRING_SEQUENCESTEP_MSRC);
        break;
    case VL53L0X_SEQUENCESTEP_PRE_RANGE:
        VL53L0X_COPYSTRING(pSequenceStepsString,
                           VL53L0X_STRING_SEQUENCESTEP_PRE_RANGE);
        break;
    case VL53L0X_SEQUENCESTEP_FINAL_RANGE:
        VL53L0X_COPYSTRING(pSequenceStepsString,
                           VL53L0X_STRING_SEQUENCESTEP_FINAL_RANGE);
        break;

    default:
        Status = VL53L0X_ERROR_INVALID_PARAMS;
    }

    LOG_FUNCTION_END(Status);

    return Status;
}


VL53L0X_Error VL53L0X_get_limit_check_info(VL53L0X_DEV Dev, uint16_t LimitCheckId,
        char *pLimitCheckString)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    LOG_FUNCTION_START("");

    switch (LimitCheckId) {
    case VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE:
        VL53L0X_COPYSTRING(pLimitCheckString,
                           VL53L0X_STRING_CHECKENABLE_SIGMA_FINAL_RANGE);
        break;
    case VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE:
        VL53L0X_COPYSTRING(pLimitCheckString,
                           VL53L0X_STRING_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE);
        break;
    case VL53L0X_CHECKENABLE_SIGNAL_REF_CLIP:
        VL53L0X_COPYSTRING(pLimitCheckString,
                           VL53L0X_STRING_CHECKENABLE_SIGNAL_REF_CLIP);
        break;
    case VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD:
        VL53L0X_COPYSTRING(pLimitCheckString,
                           VL53L0X_STRING_CHECKENABLE_RANGE_IGNORE_THRESHOLD);
        break;

    case VL53L0X_CHECKENABLE_SIGNAL_RATE_MSRC:
        VL53L0X_COPYSTRING(pLimitCheckString,
                           VL53L0X_STRING_CHECKENABLE_SIGNAL_RATE_MSRC);
        break;

    case VL53L0X_CHECKENABLE_SIGNAL_RATE_PRE_RANGE:
        VL53L0X_COPYSTRING(pLimitCheckString,
                           VL53L0X_STRING_CHECKENABLE_SIGNAL_RATE_PRE_RANGE);
        break;

    default:
        VL53L0X_COPYSTRING(pLimitCheckString,
                           VL53L0X_STRING_UNKNOW_ERROR_CODE);

    }

    LOG_FUNCTION_END(Status);
    return Status;
}