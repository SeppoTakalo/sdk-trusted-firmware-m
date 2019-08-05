#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file contains settings to specify how GCC shall be used

#Include some dependencies
Include(Common/CompilerGNUARMCommon)
Include(Common/Utils)

check_gnuarm_input_vars("8.3")

if(NOT DEFINED ARM_CPU_ARCHITECTURE)
	set(_NO_ARM_CPU_ARCHITECTURE true)
elseif (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8-M.BASE")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.base")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.base")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.base")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.base")
elseif(${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8-M.MAIN")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.main")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.main")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.main")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "-march=" VAL "-march=armv8-m.main")
elseif(${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv7-M")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
elseif(${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv6-M")
	string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv6-m")
	string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv6-m")
	string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "-march=" VAL "-march=armv6-m")
	string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "-march=" VAL "-march=armv6-m")
else()
	message(FATAL_ERROR "Unknown or unsupported ARM cpu architecture setting.")
endif()

#Prefer architecture definition over cpu type.
if(NOT DEFINED ARM_CPU_ARCHITECTURE)
	if(NOT DEFINED ARM_CPU_TYPE)
		string_append_unique_item(_NO_ARM_CPU_TYPE true)
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M3")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m3")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m3")
		string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL  "--cpu=Cortex-M3")
		string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M3")
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M33")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m33")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m33")
		string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M33")
		string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M33")
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M23")
		string_append_unique_item(STRING CMAKE_C_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m23")
		string_append_unique_item(STRING CMAKE_CXX_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m23")
		string_append_unique_item(STRING CMAKE_ASM_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M23")
		string_append_unique_item(STRING CMAKE_LINK_FLAGS_CPU KEY "--cpu=" VAL "--cpu=Cortex-M23")
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M4")
		string_append_unique_item (STRING CMAKE_C_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
		string_append_unique_item (STRING CMAKE_CXX_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
		string_append_unique_item (STRING CMAKE_ASM_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
		string_append_unique_item (STRING CMAKE_LINK_FLAGS_CPU KEY "-march=" VAL "-march=armv7-m")
	elseif(${ARM_CPU_TYPE} STREQUAL "Cortex-M0p")
		string_append_unique_item (STRING CMAKE_C_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m0plus")
		string_append_unique_item (STRING CMAKE_CXX_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=cortex-m0plus")
		string_append_unique_item (STRING CMAKE_ASM_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=Cortex-m0plus")
		string_append_unique_item (STRING CMAKE_LINK_FLAGS_CPU KEY "-mcpu=" VAL "-mcpu=Cortex-m0plus")
	else()
		message(FATAL_ERROR "Unknown ARM cpu setting.")
	endif()
endif()

if (_NO_ARM_CPU_TYPE AND _NO_ARM_CPU_ARCHITECTURE)
	message(FATAL_ERROR "Cannot set CPU specific compiler flags: neither the ARM CPU type nor the architecture is set.")
endif()