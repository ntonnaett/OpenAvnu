/*************************************************************************************************************
Copyright (c) 2012-2015, Symphony Teleca Corporation, a Harman International Industries, Incorporated company
Copyright (c) 2016-2017, Harman International Industries, Incorporated
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS LISTED "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS LISTED BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Attributions: The inih library portion of the source code is licensed from
Brush Technology and Ben Hoyt - Copyright (c) 2009, Brush Technology and Copyright (c) 2009, Ben Hoyt.
Complete license and copyright information can be found at
https://github.com/benhoyt/inih/commit/74d2ca064fb293bc60a77b0bd068075b293cf175.
*************************************************************************************************************/

/*
 ******************************************************************
 * MODULE : AEM - AVDECC Strings Descriptor
 * MODULE SUMMARY : Implements the AVDECC Strings IEEE Std 1722.1-2013 clause 7.2.12 
 ******************************************************************
 */

#include <stdlib.h>

#define	AVB_LOG_COMPONENT	"AEM"
#include "openavb_log.h"

#include "openavb_rawsock.h"
#include "openavb_aem.h"
#include "openavb_descriptor_strings.h"


////////////////////////////////
// Private (internal) functions
////////////////////////////////
openavbRC openavbAemDescriptorStringsToBuf(void *pVoidDescriptor, U16 bufLength, U8 *pBuf, U16 *descriptorSize)
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	openavb_aem_descriptor_strings_t *pDescriptor = pVoidDescriptor;

	if (!pDescriptor || !pBuf || !descriptorSize) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT), AVB_TRACE_AEM);
	}

	if (bufLength < OPENAVB_DESCRIPTOR_STRINGS_BASE_LENGTH) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVBAVDECC_RC_BUFFER_TOO_SMALL), AVB_TRACE_AEM);
	}

	*descriptorSize = 0;

	openavb_aem_descriptor_strings_t *pSrc = pDescriptor;
	U8 *pDst = pBuf;

	OCT_D2BHTONS(pDst, pSrc->descriptor_type);
	OCT_D2BHTONS(pDst, pSrc->descriptor_index);
	memcpy(pDst, pSrc->string, sizeof(pSrc->string));
	pDst += sizeof(pSrc->string);
	*descriptorSize = pDst - pBuf;

	AVB_RC_TRACE_RET(OPENAVB_AVDECC_SUCCESS, AVB_TRACE_AEM);
}

openavbRC openavbAemDescriptorStringsFromBuf(void *pVoidDescriptor, U16 bufLength, U8 *pBuf)
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	openavb_aem_descriptor_strings_t *pDescriptor = pVoidDescriptor;

	if (!pDescriptor || !pBuf) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT), AVB_TRACE_AEM);
	}

	if (bufLength < OPENAVB_DESCRIPTOR_STRINGS_BASE_LENGTH) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVBAVDECC_RC_BUFFER_TOO_SMALL), AVB_TRACE_AEM);
	}

	U8 *pSrc = pBuf;
	openavb_aem_descriptor_strings_t *pDst = pDescriptor;

	OCT_B2DNTOHS(pDst->descriptor_type, pSrc);
	OCT_B2DNTOHS(pDst->descriptor_index, pSrc);
	memcpy(pDst->string, pSrc, sizeof(pDst->string));
	pSrc += sizeof(pDst->string);

	AVB_RC_TRACE_RET(OPENAVB_AVDECC_SUCCESS, AVB_TRACE_AEM);
}

openavbRC openavbAemDescriptorStringsUpdate(void *pVoidDescriptor)
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	openavb_aem_descriptor_strings_t *pDescriptor = pVoidDescriptor;

	if (!pDescriptor) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT), AVB_TRACE_AEM);
	}

	// AVDECC_TODO - Any updates needed?

	AVB_RC_TRACE_RET(OPENAVB_AVDECC_SUCCESS, AVB_TRACE_AEM);
}

////////////////////////////////
// Public functions
////////////////////////////////
extern DLL_EXPORT openavb_aem_descriptor_strings_t *openavbAemDescriptorStringsNew()
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	openavb_aem_descriptor_strings_t *pDescriptor;

	pDescriptor = malloc(sizeof(*pDescriptor));

	if (!pDescriptor) {
		AVB_RC_LOG(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_OUT_OF_MEMORY));
		AVB_TRACE_EXIT(AVB_TRACE_AEM);
		return NULL;
	}
	memset(pDescriptor, 0, sizeof(*pDescriptor));

	pDescriptor->descriptorPvtPtr = malloc(sizeof(*pDescriptor->descriptorPvtPtr));
	if (!pDescriptor->descriptorPvtPtr) {
		free(pDescriptor);
		pDescriptor = NULL;
		AVB_RC_LOG(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_OUT_OF_MEMORY));
		AVB_TRACE_EXIT(AVB_TRACE_AEM);
		return NULL;
	}

	pDescriptor->descriptorPvtPtr->size = sizeof(openavb_aem_descriptor_strings_t);
	pDescriptor->descriptorPvtPtr->bTopLevel = FALSE;
	pDescriptor->descriptorPvtPtr->toBuf = openavbAemDescriptorStringsToBuf;
	pDescriptor->descriptorPvtPtr->fromBuf = openavbAemDescriptorStringsFromBuf;
	pDescriptor->descriptorPvtPtr->update = openavbAemDescriptorStringsUpdate;

	pDescriptor->descriptor_type = OPENAVB_AEM_DESCRIPTOR_STRINGS;

	AVB_TRACE_EXIT(AVB_TRACE_AEM);
	return pDescriptor;
}

extern DLL_EXPORT const char * openavbAemDescriptorStringsGet_string(openavb_aem_descriptor_strings_t *pDescriptor, U8 index)
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	if (!pDescriptor || index >= OPENAVB_AEM_NUM_DESCRIPTOR_STRINGS) {
		AVB_RC_LOG(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT));
		AVB_TRACE_EXIT(AVB_TRACE_AEM);
		return NULL;
	}

	const char *ret = (const char *) (pDescriptor->string[index]);
	if (*ret == '\0') {
		/* The string is empty. */
		ret = NULL;
	}

	AVB_TRACE_EXIT(AVB_TRACE_AEM);
	return ret;
}

extern DLL_EXPORT bool openavbAemDescriptorStringsSet_string(openavb_aem_descriptor_strings_t *pDescriptor, const char *pString, U8 index)
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	if (!pDescriptor || !pString || index >= OPENAVB_AEM_NUM_DESCRIPTOR_STRINGS) {
		AVB_RC_LOG(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT));
		AVB_TRACE_EXIT(AVB_TRACE_AEM);
		return FALSE;
	}

	openavbAemSetString(pDescriptor->string[index], pString);

	AVB_TRACE_EXIT(AVB_TRACE_AEM);
	return TRUE;
}
