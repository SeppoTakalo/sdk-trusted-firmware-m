/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PSA_API_H__
#define __PSA_API_H__

#include <stdint.h>
#include <stdbool.h>
#include "config_spm.h"
#include "psa/client.h"
#include "psa/service.h"

#if PSA_FRAMEWORK_HAS_MM_IOVEC

/*
 * The MM-IOVEC status
 * The max total number of invec and outvec is 8.
 * Each invec/outvec takes 4 bit, 32 bits in total.
 *
 * The encoding format of the MM-IOVEC status:
 *--------------------------------------------------------------
 *|  Bit   |  31 - 28  |  27 - 24  | ... |  7 - 4   |  3 - 0   |
 *--------------------------------------------------------------
 *| Vector | outvec[3] | outvec[2] | ... | invec[1] | invec[0] |
 *--------------------------------------------------------------
 *
 * Take invec[0] as an example:
 *
 * bit 0:  whether invec[0] has been mapped.
 * bit 1:  whether invec[0] has been unmapped.
 * bit 2:  whether invec[0] has been accessed using psa_read(), psa_skip() or
 *         psa_write().
 * bit 3:  reserved for invec[0].
 */

#define IOVEC_STATUS_BITS              4   /* Each vector occupies 4 bits. */
#define OUTVEC_IDX_BASE                4   /*
                                            * Base index of outvec.
                                            * There are four invecs in front of
                                            * outvec.
                                            */
#define INVEC_IDX_BASE                 0   /* Base index of invec. */

#define IOVEC_MAPPED_BIT               (1U << 0)
#define IOVEC_UNMAPPED_BIT             (1U << 1)
#define IOVEC_ACCESSED_BIT             (1U << 2)

#define IOVEC_IS_MAPPED(handle, iovec_idx)      \
    ((((handle)->iovec_status) >> ((iovec_idx) * IOVEC_STATUS_BITS)) &  \
                               IOVEC_MAPPED_BIT)
#define IOVEC_IS_UNMAPPED(handle, iovec_idx)    \
    ((((handle)->iovec_status) >> ((iovec_idx) * IOVEC_STATUS_BITS)) &  \
                               IOVEC_UNMAPPED_BIT)
#define IOVEC_IS_ACCESSED(handle, iovec_idx)    \
    ((((handle)->iovec_status) >> ((iovec_idx) * IOVEC_STATUS_BITS)) &  \
                               IOVEC_ACCESSED_BIT)
#define SET_IOVEC_MAPPED(handle, iovec_idx)     \
    (((handle)->iovec_status) |= (IOVEC_MAPPED_BIT <<   \
                              ((iovec_idx) * IOVEC_STATUS_BITS)))
#define SET_IOVEC_UNMAPPED(handle, iovec_idx)   \
    (((handle)->iovec_status) |= (IOVEC_UNMAPPED_BIT << \
                              ((iovec_idx) * IOVEC_STATUS_BITS)))
#define SET_IOVEC_ACCESSED(handle, iovec_idx)   \
    (((handle)->iovec_status) |= (IOVEC_ACCESSED_BIT << \
                              ((iovec_idx) * IOVEC_STATUS_BITS)))

#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC */

/**
 * \brief This function handles the specific programmer error cases.
 *
 * \param[in] status            Standard error codes for the SPM.
 *
 * \retval void                 Status will not cause SPM panic
 * \retval "SPM panic"          Following programmer errors are triggered by SP:
 * \arg                           PSA_ERROR_PROGRAMMER_ERROR
 * \arg                           PSA_ERROR_CONNECTION_REFUSED
 * \arg                           PSA_ERROR_CONNECTION_BUSY
 */
void spm_handle_programmer_errors(psa_status_t status);

/**
 * \brief This function get the current PSA RoT lifecycle state.
 *
 * \return state                The current security lifecycle state of the PSA
 *                              RoT. The PSA state and implementation state are
 *                              encoded as follows:
 * \arg                           state[15:8] – PSA lifecycle state
 * \arg                           state[7:0] – IMPLEMENTATION DEFINED state
 */
uint32_t tfm_spm_get_lifecycle_state(void);

/* PSA Client API function body, for privileged use only. */

/**
 * \brief handler for \ref psa_framework_version.
 *
 * \return version              The version of the PSA Framework implementation
 *                              that is providing the runtime services.
 */
uint32_t tfm_spm_client_psa_framework_version(void);

/**
 * \brief handler for \ref psa_version.
 *
 * \param[in] sid               RoT Service identity.
 *
 * \retval PSA_VERSION_NONE     The RoT Service is not implemented, or the
 *                              caller is not permitted to access the service.
 * \retval > 0                  The version of the implemented RoT Service.
 */
uint32_t tfm_spm_client_psa_version(uint32_t sid);

/**
 * \brief handler for \ref psa_call.
 *
 * \param[in] handle            Service handle to the established connection,
 *                              \ref psa_handle_t
 * \param[in] ctrl_param        Parameters combined in uint32_t,
 *                              includes request type, in_num and out_num.
 * \param[in] inptr             Array of input psa_invec structures.
 *                              \ref psa_invec
 * \param[in] outptr            Array of output psa_outvec structures.
 *                              \ref psa_outvec
 *
 * \retval PSA_SUCCESS          Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           An invalid handle was passed.
 * \arg                           The connection is already handling a request.
 * \arg                           An invalid memory reference was provided.
 * \arg                           in_num + out_num > PSA_MAX_IOVEC.
 * \arg                           The message is unrecognized by the RoT
 *                                Service or incorrectly formatted.
 */
psa_status_t tfm_spm_client_psa_call(psa_handle_t handle,
                                     uint32_t ctrl_param,
                                     const psa_invec *inptr,
                                     psa_outvec *outptr);

/* Following PSA APIs are only needed by connection-based services */
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1

/**
 * \brief handler for \ref psa_connect.
 *
 * \param[in] sid               RoT Service identity.
 * \param[in] version           The version of the RoT Service.
 *
 * \retval PSA_SUCCESS          Success.
 * \retval PSA_ERROR_CONNECTION_REFUSED The SPM or RoT Service has refused the
 *                              connection.
 * \retval PSA_ERROR_CONNECTION_BUSY The SPM or RoT Service cannot make the
 *                              connection at the moment.
 * \retval "Does not return"    The RoT Service ID and version are not
 *                              supported, or the caller is not permitted to
 *                              access the service.
 */
psa_status_t tfm_spm_client_psa_connect(uint32_t sid, uint32_t version);

/**
 * \brief handler for \ref psa_close.
 *
 * \param[in] handle            Service handle to the connection to be closed,
 *                              \ref psa_handle_t
 *
 * \retval PSA_SUCCESS          Success.
 * \retval PSA_ERROR_PROGRAMMER_ERROR The call is invalid, one or more of the
 *                              following are true:
 * \arg                           Called with a stateless handle.
 * \arg                           An invalid handle was provided that is not
 *                                the null handle.
 * \arg                           The connection is handling a request.
 */
psa_status_t tfm_spm_client_psa_close(psa_handle_t handle);

#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API */

/* PSA Partition API function body, for privileged use only. */

#if CONFIG_TFM_SPM_BACKEND_IPC == 1 \
    || CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1
/**
 * \brief Function body of \ref psa_wait.
 *
 * \param[in] signal_mask       A set of signals to query. Signals that are not
 *                              in this set will be ignored.
 * \param[in] timeout           Specify either blocking \ref PSA_BLOCK or
 *                              polling \ref PSA_POLL operation.
 *
 * \retval >0                   At least one signal is asserted.
 * \retval 0                    No signals are asserted. This is only seen when
 *                              a polling timeout is used.
 */
psa_signal_t tfm_spm_partition_psa_wait(psa_signal_t signal_mask,
                                        uint32_t timeout);
#endif

/* This API is only used in IPC backend. */
#if CONFIG_TFM_SPM_BACKEND_IPC == 1
/**
 * \brief Function body of \ref psa_get.
 *
 * \param[in] signal            The signal value for an asserted RoT Service.
 * \param[out] msg              Pointer to \ref psa_msg_t object for receiving
 *                              the message.
 *
 * \retval PSA_SUCCESS          Success, *msg will contain the delivered
 *                              message.
 * \retval PSA_ERROR_DOES_NOT_EXIST Message could not be delivered.
 * \retval "PROGRAMMER ERROR"   The call is invalid because one or more of the
 *                              following are true:
 * \arg                           signal has more than a single bit set.
 * \arg                           signal does not correspond to an RoT Service.
 * \arg                           The RoT Service signal is not currently
 *                                asserted.
 * \arg                           The msg pointer provided is not a valid memory
 *                                reference.
 */
psa_status_t tfm_spm_partition_psa_get(psa_signal_t signal, psa_msg_t *msg);
#endif /* CONFIG_TFM_SPM_BACKEND_IPC == 1 */

/**
 * \brief Function body of \ref psa_read.
 *
 * \param[in] msg_handle        Handle for the client's message.
 * \param[in] invec_idx         Index of the input vector to read from. Must be
 *                              less than \ref PSA_MAX_IOVEC.
 * \param[out] buffer           Buffer in the Secure Partition to copy the
 *                              requested data to.
 * \param[in] num_bytes         Maximum number of bytes to be read from the
 *                              client input vector.
 *
 * \retval >0                   Number of bytes copied.
 * \retval 0                    There was no remaining data in this input
 *                              vector.
 * \retval "PROGRAMMER ERROR"   The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a
 *                                \ref PSA_IPC_CALL message.
 * \arg                           invec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 * \arg                           the memory reference for buffer is invalid or
 *                                not writable.
 */
size_t tfm_spm_partition_psa_read(psa_handle_t msg_handle, uint32_t invec_idx,
                                  void *buffer, size_t num_bytes);

/**
 * \brief Function body of psa_skip.
 *
 * \param[in] msg_handle        Handle for the client's message.
 * \param[in] invec_idx         Index of input vector to skip from. Must be
 *                              less than \ref PSA_MAX_IOVEC.
 * \param[in] num_bytes         Maximum number of bytes to skip in the client
 *                              input vector.
 *
 * \retval >0                   Number of bytes skipped.
 * \retval 0                    There was no remaining data in this input
 *                              vector.
 * \retval "PROGRAMMER ERROR"   The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a request
 *                                message.
 * \arg                           invec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 */
size_t tfm_spm_partition_psa_skip(psa_handle_t msg_handle, uint32_t invec_idx,
                                  size_t num_bytes);

/**
 * \brief Function body of \ref psa_write.
 *
 * \param[in] msg_handle        Handle for the client's message.
 * \param[out] outvec_idx       Index of output vector in message to write to.
 *                              Must be less than \ref PSA_MAX_IOVEC.
 * \param[in] buffer            Buffer with the data to write.
 * \param[in] num_bytes         Number of bytes to write to the client output
 *                              vector.
 *
 * \retval PSA_SUCCESS          Success.
 * \retval "PROGRAMMER ERROR"   The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a request
 *                                message.
 * \arg                           outvec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 * \arg                           The memory reference for buffer is invalid.
 * \arg                           The call attempts to write data past the end
 *                                of the client output vector.
 */
psa_status_t tfm_spm_partition_psa_write(psa_handle_t msg_handle, uint32_t outvec_idx,
                                         const void *buffer, size_t num_bytes);

/**
 * \brief Function body of \ref psa_reply.
 *
 * \param[in] msg_handle        Handle for the client's message.
 * \param[in] status            Message result value to be reported to the
 *                              client.
 *
 * \retval Positive integer     Success, the connection handle.
 * \retval PSA_SUCCESS          Success
 * \retval "PROGRAMMER ERROR"   The call is invalid, one or more of the
 *                              following are true:
 * \arg                         msg_handle is invalid.
 * \arg                         An invalid status code is specified for the
 *                              type of message.
 */
int32_t tfm_spm_partition_psa_reply(psa_handle_t msg_handle,
                                    psa_status_t status);

#if CONFIG_TFM_DOORBELL_API == 1
/**
 * \brief Function body of \ref psa_norify.
 *
 * \param[in] partition_id      Secure Partition ID of the target partition.
 *
 * \retval PSA_SUCCES           Success.
 * \retval "PROGRAMMER ERROR"   partition_id does not correspond to a Secure
 *                              Partition.
 */
psa_status_t tfm_spm_partition_psa_notify(int32_t partition_id);

/**
 * \brief Function body of \ref psa_clear.
 *
 * \retval PSA_SUCCES           Success.
 * \retval "PROGRAMMER ERROR"   The Secure Partition's doorbell signal is not
 *                              currently asserted.
 */
psa_status_t tfm_spm_partition_psa_clear(void);
#endif /* CONFIG_TFM_DOORBELL_API == 1 */

/**
 * \brief Function body of \ref psa_panic.
 *
 * \retval "Should not return"
 */
psa_status_t tfm_spm_partition_psa_panic(void);

/* psa_set_rhandle is only needed by connection-based services */
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1

/**
 * \brief Function body of \ref psa_set_rhandle.
 *
 * \param[in] msg_handle        Handle for the client's message.
 * \param[in] rhandle           Reverse handle allocated by the RoT Service.
 *
 * \retval PSA_SUCCES           Success, rhandle will be provided with all
 *                              subsequent messages delivered on this
 *                              connection.
 * \retval "PROGRAMMER ERROR"   msg_handle is invalid.
 */
psa_status_t tfm_spm_partition_psa_set_rhandle(psa_handle_t msg_handle, void *rhandle);

#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API */

#if CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1
/**
 * \brief Function body of \ref psa_irq_enable.
 *
 * \param[in] irq_signal The signal for the interrupt to be enabled.
 *                       This must have a single bit set, which must be the
 *                       signal value for an interrupt in the calling Secure
 *                       Partition.
 *
 * \retval PSA_SUCCES         Success.
 * \retval "PROGRAMMER ERROR" If one or more of the following are true:
 * \arg                       \a irq_signal is not an interrupt signal.
 * \arg                       \a irq_signal indicates more than one signal.
 */
psa_status_t tfm_spm_partition_psa_irq_enable(psa_signal_t irq_signal);

/**
 * \brief Function body of psa_irq_disable.
 *
 * \param[in] irq_signal The signal for the interrupt to be disabled.
 *                       This must have a single bit set, which must be the
 *                       signal value for an interrupt in the calling Secure
 *                       Partition.
 *
 * \retval 0                  The interrupt was disabled prior to this call.
 *         1                  The interrupt was enabled prior to this call.
 * \retval "PROGRAMMER ERROR" If one or more of the following are true:
 * \arg                       \a irq_signal is not an interrupt signal.
 * \arg                       \a irq_signal indicates more than one signal.
 *
 * \note The current implementation always return 1. Do not use the return.
 */
psa_irq_status_t tfm_spm_partition_psa_irq_disable(psa_signal_t irq_signal);

/* This API is only used for FLIH. */
#if CONFIG_TFM_FLIH_API == 1
/**
 * \brief Function body of \ref psa_reset_signal.
 *
 * \param[in] irq_signal    The interrupt signal to be reset.
 *                          This must have a single bit set, corresponding to a
 *                          currently asserted signal for an interrupt that is
 *                          defined to use FLIH handling.
 *
 * \retval PSA_SUCCES         .
 * \retval "Programmer Error" if one or more of the following are true:
 * \arg                       \a irq_signal is not a signal for an interrupt
 *                            that is specified with FLIH handling in the Secure
 *                            Partition manifest.
 * \arg                       \a irq_signal indicates more than one signal.
 * \arg                       \a irq_signal is not currently asserted.
 */
psa_status_t tfm_spm_partition_psa_reset_signal(psa_signal_t irq_signal);
#endif

/* This API is only used for SLIH. */
#if CONFIG_TFM_SLIH_API == 1
/**
 * \brief Function body of \ref psa_eoi.
 *
 * \param[in] irq_signal        The interrupt signal that has been processed.
 *
 * \retval PSA_SUCCES           Success.
 * \retval "PROGRAMMER ERROR"   The call is invalid, one or more of the
 *                              following are true:
 * \arg                           irq_signal is not an interrupt signal.
 * \arg                           irq_signal indicates more than one signal.
 * \arg                           irq_signal is not currently asserted.
 * \arg                           The interrupt is not using SLIH.
 */
psa_status_t tfm_spm_partition_psa_eoi(psa_signal_t irq_signal);
#endif
#endif /* CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1 */

#if PSA_FRAMEWORK_HAS_MM_IOVEC

/**
 * \brief Function body of psa_map_invec.
 */
const void *tfm_spm_partition_psa_map_invec(psa_handle_t msg_handle,
                                            uint32_t invec_idx);

/**
 * \brief Function body of psa_unmap_invec.
 */
void tfm_spm_partition_psa_unmap_invec(psa_handle_t msg_handle,
                                       uint32_t invec_idx);

/**
 * \brief Function body of psa_map_outvet.
 */
void *tfm_spm_partition_psa_map_outvec(psa_handle_t msg_handle,
                                       uint32_t outvec_idx);

/**
 * \brief Function body of psa_unmap_outvec.
 */
void tfm_spm_partition_psa_unmap_outvec(psa_handle_t msg_handle,
                                        uint32_t outvec_idx, size_t len);

#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC */

#endif /* __PSA_API_H__ */
