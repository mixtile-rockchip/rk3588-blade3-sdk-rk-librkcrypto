/*
 * Copyright (c) 2022 Rockchip Electronics Co. Ltd.
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "rkcrypto_core.h"
#include "rkcrypto_trace.h"
#include "cmode_adapter.h"
#include "test_rsa.h"
#include "test_utils.h"

//#define RSA_KEY_ADDITINAL_PARAM

#define TEST_BUFFER_SIZE	sizeof(no_padding_data)

#define TEST_END_PASS() \
	printf("****************** %s test PASS !!! ******************\n", __func__)

#define TEST_END_FAIL() \
	printf("****************** %s test FAIL !!! ******************\n", __func__)

typedef RK_RES (*test_rsa_one)(uint32_t padding, const uint8_t *in, uint32_t in_len,
			       const uint8_t *expect, int verbose);

struct test_rsa_item {
	test_rsa_one	do_test;
	uint32_t	padding;
	const uint8_t	*in;
	uint32_t	in_len;
	const uint8_t	*expect;
};

static const uint8_t no_padding_data[] = {
0x31, 0x5d, 0xfa, 0x52, 0xa4, 0x93, 0x52, 0xf8, 0xf5, 0xed, 0x39, 0xf4, 0xf8, 0x23, 0x4b, 0x30,
0x11, 0xa2, 0x2c, 0x5b, 0xa9, 0x8c, 0xcf, 0xdf, 0x19, 0x66, 0xf5, 0xf5, 0x1a, 0x6d, 0xf6, 0x25,
0x89, 0xaf, 0x06, 0x13, 0xdc, 0xa4, 0xd4, 0x0b, 0x3c, 0x1c, 0x4f, 0xb9, 0xd3, 0xd0, 0x63, 0x29,
0x2a, 0x5d, 0xfe, 0xb6, 0x99, 0x20, 0x58, 0x36, 0x2b, 0x1d, 0x57, 0xf4, 0x71, 0x38, 0xa7, 0x8b,
0xad, 0x8c, 0xef, 0x1f, 0x2f, 0xea, 0x4c, 0x87, 0x2b, 0xd7, 0xb8, 0xc8, 0xb8, 0x09, 0xcb, 0xb9,
0x05, 0xab, 0x43, 0x41, 0xd9, 0x75, 0x36, 0x4d, 0xb6, 0x8a, 0xd3, 0x45, 0x96, 0xfd, 0x9c, 0xe8,
0x6e, 0xc8, 0x37, 0x5e, 0x4f, 0x63, 0xf4, 0x1c, 0x18, 0x2c, 0x38, 0x79, 0xe2, 0x5a, 0xe5, 0x1d,
0x48, 0xf6, 0xb2, 0x79, 0x57, 0x12, 0xab, 0xae, 0xc1, 0xb1, 0x9d, 0x11, 0x4f, 0xa1, 0x4d, 0x1b,
0x4c, 0x8c, 0x3a, 0x2d, 0x7b, 0x98, 0xb9, 0x89, 0x7b, 0x38, 0x84, 0x13, 0x8e, 0x3f, 0x3c, 0xe8,
0x59, 0x26, 0x90, 0x77, 0xe7, 0xca, 0x52, 0xbf, 0x3a, 0x5e, 0xe2, 0x58, 0x54, 0xd5, 0x9b, 0x2a,
0x0d, 0x33, 0x31, 0xf4, 0x4d, 0x68, 0x68, 0xf3, 0xe9, 0xb2, 0xbe, 0x28, 0xeb, 0xce, 0xdb, 0x36,
0x1e, 0xae, 0xb7, 0x37, 0xca, 0xaa, 0xf0, 0x9c, 0x6e, 0x27, 0x93, 0xc9, 0x61, 0x76, 0x99, 0x1a,
0x0a, 0x99, 0x57, 0xa8, 0xea, 0x71, 0x96, 0x63, 0xbc, 0x76, 0x11, 0x5c, 0x0c, 0xd4, 0x70, 0x0b,
0xd8, 0x1c, 0x4e, 0x95, 0x89, 0x5b, 0x09, 0x17, 0x08, 0x44, 0x70, 0xec, 0x60, 0x7c, 0xc9, 0x8a,
0xa0, 0xe8, 0x98, 0x64, 0xfa, 0xe7, 0x52, 0x73, 0xb0, 0x04, 0x9d, 0x78, 0xee, 0x09, 0xa1, 0xb9,
0x79, 0xd5, 0x52, 0x4f, 0xf2, 0x39, 0x1c, 0xf7, 0xb9, 0x73, 0xe0, 0x3d, 0x6b, 0x54, 0x64, 0x86
};

static const uint8_t rsa_2048_n[] = {
0x82, 0x78, 0xA0, 0xC5, 0x39, 0xE6, 0xF6, 0xA1, 0x5E, 0xD1, 0xC6, 0x8B, 0x9C, 0xF9, 0xC4, 0x3F,
0xEA, 0x19, 0x16, 0xB0, 0x96, 0x3A, 0xB0, 0x5A, 0x94, 0xED, 0x6A, 0xD3, 0x83, 0xE8, 0xA0, 0xFD,
0x01, 0x5E, 0x92, 0x2A, 0x7D, 0x0D, 0xF9, 0x72, 0x1E, 0x03, 0x8A, 0x68, 0x8B, 0x4D, 0x57, 0x55,
0xF5, 0x2F, 0x9A, 0xC9, 0x45, 0xCF, 0x9B, 0xB7, 0xF5, 0x11, 0x94, 0x7A, 0x16, 0x0B, 0xED, 0xD9,
0xA3, 0xF0, 0x63, 0x8A, 0xEC, 0xD3, 0x21, 0xAB, 0xCF, 0x74, 0xFC, 0x6B, 0xCE, 0x06, 0x4A, 0x51,
0xC9, 0x7C, 0x7C, 0xA3, 0xC4, 0x10, 0x63, 0x7B, 0x00, 0xEC, 0x2D, 0x02, 0x18, 0xD5, 0xF1, 0x8E,
0x19, 0x7F, 0xBE, 0xE2, 0x45, 0x5E, 0xD7, 0xA8, 0x95, 0x90, 0x88, 0xB0, 0x73, 0x35, 0x89, 0x66,
0x1C, 0x23, 0xB9, 0x6E, 0x88, 0xE0, 0x7A, 0x57, 0xB0, 0x55, 0x8B, 0x81, 0x9B, 0x9C, 0x34, 0x9F,
0x86, 0x0E, 0x15, 0x94, 0x2C, 0x6B, 0x12, 0xC3, 0xB9, 0x56, 0x60, 0x25, 0x59, 0x3E, 0x50, 0x7B,
0x62, 0x4A, 0xD0, 0xF0, 0xB6, 0xB1, 0x94, 0x83, 0x51, 0x66, 0x6F, 0x60, 0x4D, 0xEF, 0x8F, 0x94,
0xA6, 0xD1, 0xA2, 0x80, 0x06, 0x24, 0xF2, 0x6E, 0xD2, 0xC7, 0x01, 0x34, 0x8D, 0x2B, 0x6B, 0x03,
0xF7, 0x05, 0xA3, 0x99, 0xCC, 0xC5, 0x16, 0x75, 0x1A, 0x81, 0xC1, 0x67, 0xA0, 0x88, 0xE6, 0xE9,
0x00, 0xFA, 0x62, 0xAF, 0x2D, 0xA9, 0xFA, 0xC3, 0x30, 0x34, 0x98, 0x05, 0x4C, 0x1A, 0x81, 0x0C,
0x52, 0xCE, 0xBA, 0xD6, 0xEB, 0x9C, 0x1E, 0x76, 0x01, 0x41, 0x6C, 0x34, 0xFB, 0xC0, 0x83, 0xC5,
0x4E, 0xB3, 0xF2, 0x5B, 0x4F, 0x94, 0x08, 0x33, 0x87, 0x5E, 0xF8, 0x39, 0xEF, 0x7F, 0x72, 0x94,
0xFF, 0xD7, 0x51, 0xE8, 0xA2, 0x5E, 0x26, 0x25, 0x5F, 0xE9, 0xCC, 0x2A, 0x7D, 0xAC, 0x5B, 0x35
};

static const uint8_t rsa_2048_e[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01
};

static const uint8_t rsa_2048_d[] = {
0x49, 0x7E, 0x93, 0xE9, 0xA5, 0x7D, 0x42, 0x0E, 0x92, 0xB0, 0x0E, 0x6C, 0x94, 0xC7, 0x69, 0x52,
0x2B, 0x97, 0x68, 0x5D, 0x9E, 0xB2, 0x7E, 0xA6, 0xF7, 0xDF, 0x69, 0x5E, 0xAE, 0x9E, 0x7B, 0x19,
0x2A, 0x0D, 0x50, 0xBE, 0xD8, 0x64, 0xE7, 0xCF, 0xED, 0xB2, 0x46, 0xE4, 0x2F, 0x1C, 0x29, 0x07,
0x45, 0xAF, 0x44, 0x3C, 0xFE, 0xB3, 0x3C, 0xDF, 0x7A, 0x10, 0x26, 0x18, 0x43, 0x95, 0x02, 0xAD,
0xA7, 0x98, 0x81, 0x2A, 0x3F, 0xCF, 0x8A, 0xD7, 0x12, 0x6C, 0xAE, 0xC8, 0x37, 0x6C, 0xF9, 0xAE,
0x6A, 0x96, 0x52, 0x4B, 0x99, 0xE5, 0x35, 0x74, 0x93, 0x87, 0x76, 0xAF, 0x08, 0xB8, 0x73, 0x72,
0x7D, 0x50, 0xA5, 0x81, 0x26, 0x5C, 0x8F, 0x94, 0xEA, 0x73, 0x59, 0x5C, 0x33, 0xF9, 0xC3, 0x65,
0x1E, 0x92, 0xCD, 0x20, 0xC3, 0xBF, 0xD7, 0x8A, 0xCF, 0xCC, 0xD0, 0x61, 0xF8, 0xFB, 0x1B, 0xF4,
0xB6, 0x0F, 0xD4, 0xCF, 0x3E, 0x55, 0x48, 0x4C, 0x99, 0x2D, 0x40, 0x44, 0x7C, 0xBA, 0x7B, 0x6F,
0xDB, 0x5D, 0x71, 0x91, 0x2D, 0x93, 0x80, 0x19, 0xE3, 0x26, 0x5D, 0x59, 0xBE, 0x46, 0x6D, 0x90,
0x4B, 0xDF, 0x72, 0xCE, 0x6C, 0x69, 0x72, 0x8F, 0x5B, 0xA4, 0x74, 0x50, 0x2A, 0x42, 0x95, 0xB2,
0x19, 0x04, 0x88, 0xD7, 0xDA, 0xBB, 0x17, 0x23, 0x69, 0xF4, 0x52, 0xEB, 0xC8, 0x55, 0xBE, 0xBC,
0x2E, 0xA9, 0xD0, 0x57, 0x7D, 0xC6, 0xC8, 0x8B, 0x86, 0x7B, 0x73, 0xCD, 0xE4, 0x32, 0x79, 0xC0,
0x75, 0x53, 0x53, 0xE7, 0x59, 0x38, 0x0A, 0x8C, 0xEC, 0x06, 0xA9, 0xFC, 0xA5, 0x15, 0x81, 0x61,
0x3E, 0x44, 0xCD, 0x05, 0xF8, 0x54, 0x04, 0x00, 0x79, 0xB2, 0x0D, 0x69, 0x2A, 0x47, 0x60, 0x1A,
0x2B, 0x79, 0x3D, 0x4B, 0x50, 0x8A, 0x31, 0x72, 0x48, 0xBB, 0x75, 0x78, 0xD6, 0x35, 0x90, 0xE1,
};

#ifdef RSA_KEY_ADDITINAL_PARAM
static const uint8_t rsa_2048_p[] = {
0xC4, 0x29, 0x15, 0xFD, 0xE6, 0xAD, 0x5D, 0x4D, 0xC5, 0xDE, 0x95, 0xB6, 0x4A, 0xA8, 0x25, 0x42,
0x15, 0x1D, 0x58, 0x0E, 0xA5, 0xEB, 0xDD, 0xE4, 0xDC, 0x70, 0xA4, 0x28, 0xA7, 0xC1, 0xC9, 0x1C,
0xBA, 0x2F, 0xA7, 0xAF, 0xE4, 0xCF, 0xC7, 0xBD, 0x87, 0x3F, 0x7C, 0xD6, 0xD7, 0x7B, 0x7D, 0x4F,
0xA1, 0xBB, 0x79, 0x06, 0x2D, 0xF1, 0xDE, 0x00, 0x1E, 0xA9, 0x87, 0x29, 0xFD, 0xE1, 0xA8, 0x43,
0xB6, 0x28, 0xC5, 0xF7, 0x2F, 0xFB, 0x64, 0x05, 0x7C, 0x6C, 0xD2, 0xFD, 0x54, 0xE4, 0x43, 0xE0,
0x80, 0xD2, 0xE3, 0xE3, 0xB4, 0xBC, 0x4E, 0xE5, 0x59, 0x93, 0x36, 0x5E, 0x9A, 0x79, 0x49, 0x7B,
0x40, 0x5F, 0xCE, 0xAE, 0x4A, 0x1B, 0x59, 0x63, 0x5F, 0xA1, 0x37, 0x6C, 0x18, 0x65, 0xA9, 0x2F,
0x71, 0x0C, 0x1A, 0xB4, 0xA1, 0x00, 0xD3, 0x20, 0x0D, 0x9A, 0xC1, 0x0B, 0x96, 0x88, 0x16, 0xC9,
};

static const uint8_t rsa_2048_q[] = {
0xAA, 0x45, 0x9E, 0x08, 0x44, 0x6B, 0x7B, 0x7D, 0x47, 0x09, 0x4C, 0xFA, 0x24, 0x4A, 0xAD, 0xD3,
0x4B, 0xF6, 0x46, 0x35, 0x63, 0xA0, 0xAD, 0xB6, 0x61, 0x0A, 0xE6, 0x1D, 0x8E, 0xAB, 0x74, 0xF1,
0x16, 0x5F, 0x83, 0xD8, 0x96, 0xB8, 0x69, 0xC5, 0x17, 0x70, 0x5B, 0x31, 0x18, 0xA4, 0xD7, 0x32,
0x83, 0xCD, 0x41, 0xAF, 0xC1, 0xAB, 0xD3, 0xA2, 0x15, 0xBF, 0x29, 0x05, 0x8F, 0xB9, 0xB7, 0xB8,
0xDB, 0xCC, 0x42, 0x3C, 0x4E, 0x8C, 0xC7, 0xF2, 0x27, 0x89, 0xA6, 0x0D, 0x6C, 0x4F, 0xD2, 0x11,
0x94, 0x64, 0xAA, 0xA0, 0x8C, 0x31, 0x7E, 0x5B, 0x0D, 0xF0, 0x6B, 0xAA, 0xE8, 0x43, 0x80, 0x2B,
0x73, 0x1B, 0x69, 0xD0, 0x0F, 0x0F, 0xD8, 0x5E, 0x35, 0xC5, 0xDF, 0x67, 0x86, 0x33, 0x0B, 0xDD,
0xE6, 0xCF, 0x6D, 0x2D, 0x60, 0x8B, 0x2B, 0x0F, 0x9F, 0xFA, 0x06, 0x7E, 0x4D, 0x7C, 0x1B, 0x0D,
};

static const uint8_t rsa_2048_dp[] = {
0x3E, 0xCD, 0x40, 0xA3, 0x37, 0x55, 0x4D, 0xC7, 0xF6, 0x8F, 0x9A, 0xB2, 0xF0, 0x18, 0x01, 0x45,
0xB4, 0xE8, 0xDE, 0x26, 0x62, 0x6C, 0xAF, 0x6F, 0xF4, 0x3B, 0x83, 0xF3, 0x18, 0x32, 0x6C, 0xA6,
0xEB, 0xDD, 0x11, 0xFC, 0xB8, 0x6E, 0xE5, 0x6E, 0x02, 0x7D, 0x0B, 0x04, 0xE5, 0x9C, 0x3D, 0xB4,
0x5E, 0xFD, 0x5C, 0x73, 0xE4, 0x05, 0xC9, 0xA3, 0x94, 0x2D, 0x86, 0x7E, 0xA5, 0x2F, 0xB7, 0xE5,
0x65, 0xCE, 0x8C, 0x02, 0xE5, 0xB3, 0xC0, 0x84, 0x19, 0x1F, 0xE6, 0x35, 0x01, 0x16, 0xCB, 0xBC,
0x76, 0xC2, 0x0D, 0xFF, 0xFA, 0xFF, 0x46, 0xEB, 0x1A, 0xD0, 0x8C, 0xD9, 0xA0, 0xEF, 0x2F, 0xDD,
0x6B, 0xF0, 0xC0, 0x85, 0x00, 0x68, 0xDD, 0x27, 0x79, 0x98, 0x6D, 0xDC, 0x07, 0xBB, 0x94, 0x7D,
0x01, 0xDE, 0x74, 0x36, 0x5C, 0x4D, 0x9D, 0x4A, 0x67, 0xD0, 0xA1, 0xF9, 0x89, 0xCA, 0x2C, 0x31,
};

static const uint8_t rsa_2048_dq[] = {
0x6F, 0xE5, 0x28, 0x21, 0x4E, 0xB5, 0x43, 0x85, 0xE4, 0x74, 0xDC, 0x3D, 0x56, 0x34, 0x2D, 0x5F,
0x3A, 0x00, 0x31, 0xCB, 0x4C, 0x19, 0x7F, 0x8E, 0xE6, 0xDE, 0xFE, 0xB2, 0x55, 0xDB, 0x9F, 0x12,
0x00, 0x1D, 0xEC, 0xCB, 0x1C, 0xE1, 0x3B, 0xC0, 0xE5, 0xD2, 0x54, 0x5B, 0x43, 0x52, 0xB0, 0x88,
0xE5, 0xCC, 0xB1, 0x6A, 0x0A, 0xE0, 0x1C, 0x47, 0xDB, 0xFA, 0xAF, 0xBE, 0x93, 0xE0, 0xFC, 0x37,
0x63, 0x5A, 0x2B, 0xFC, 0xED, 0xB1, 0xDE, 0x83, 0xE8, 0x2F, 0xB1, 0x0C, 0x09, 0x2D, 0xBB, 0x63,
0x0A, 0x1D, 0xCD, 0x73, 0x8C, 0x2D, 0xCA, 0x57, 0x94, 0x25, 0x76, 0xDB, 0xED, 0x9E, 0xCE, 0x4F,
0xBF, 0x69, 0x38, 0x74, 0x1E, 0x31, 0xCF, 0x82, 0xB9, 0xDB, 0xBB, 0x20, 0x91, 0x34, 0x62, 0x8C,
0x04, 0xEE, 0x33, 0x55, 0x2A, 0xA8, 0xCA, 0x37, 0x8E, 0x6E, 0x04, 0x6E, 0x64, 0x05, 0x36, 0xD1,
};

static const uint8_t rsa_2048_qp[] = {
0xBA, 0xBE, 0x60, 0x75, 0x6A, 0x59, 0x4B, 0x63, 0xE6, 0x2D, 0xBF, 0x48, 0x28, 0xDD, 0xC2, 0x8F,
0x3F, 0x59, 0x74, 0xCC, 0xD0, 0xC4, 0x0B, 0xD4, 0x19, 0x2E, 0x88, 0x80, 0x58, 0x8E, 0x28, 0x11,
0x69, 0x55, 0xB3, 0xC5, 0x16, 0x12, 0x0F, 0xF5, 0xDB, 0x99, 0x2A, 0xCA, 0x02, 0x1B, 0xA7, 0xBA,
0xC0, 0x80, 0x97, 0x63, 0x3A, 0xD7, 0x9F, 0x8C, 0xF4, 0xC7, 0xA8, 0x26, 0xEC, 0x03, 0x36, 0x5C,
0xE2, 0x01, 0x07, 0xF4, 0xD6, 0x6E, 0xAF, 0x31, 0x3B, 0x0E, 0xD8, 0xA5, 0x84, 0x9D, 0x8F, 0x20,
0x29, 0xA9, 0x28, 0xFB, 0x8D, 0x5D, 0xD4, 0xBD, 0xF0, 0x08, 0x31, 0x88, 0x9E, 0x04, 0x43, 0x5F,
0x0F, 0x21, 0x50, 0xBD, 0x25, 0x01, 0xFA, 0xD9, 0x0A, 0xA0, 0xDE, 0xA6, 0x16, 0xDA, 0x11, 0xC3,
0x99, 0x05, 0xCA, 0x16, 0x59, 0x62, 0x1F, 0xB4, 0x78, 0xE7, 0x96, 0x1D, 0xCF, 0x2F, 0x23, 0xE8,
};
#endif

static RK_RES test_rsa_pub_enc(uint32_t padding, const uint8_t *in, uint32_t in_len,
			       const uint8_t *expect, int verbose);
static RK_RES test_rsa_priv_enc(uint32_t padding, const uint8_t *in, uint32_t in_len,
				const uint8_t *expect, int verbose);
static RK_RES test_rsa_sign(uint32_t padding, const uint8_t *in, uint32_t in_len,
			    const uint8_t *expect, int verbose);

static const struct test_rsa_item test_rsa_tbl[] = {
	{test_rsa_pub_enc,  RK_RSA_CRYPT_PADDING_NONE, no_padding_data, sizeof(no_padding_data), NULL},
	{test_rsa_priv_enc, RK_RSA_CRYPT_PADDING_NONE, no_padding_data, sizeof(no_padding_data), NULL},
	{test_rsa_sign,     RK_RSA_CRYPT_PADDING_NONE, no_padding_data, sizeof(no_padding_data), NULL},
};

static void test_init_pubkey(rk_rsa_pub_key_pack *pub)
{
	memset(pub, 0x00, sizeof(*pub));

	pub->key_type  = RK_RSA_KEY_TYPE_PLAIN;

	pub->key.n     = rsa_2048_n;
	pub->key.n_len = sizeof(rsa_2048_n);
	pub->key.e     = rsa_2048_e;
	pub->key.e_len = sizeof(rsa_2048_e);
}

static void test_init_privkey(rk_rsa_priv_key_pack *priv)
{
	memset(priv, 0x00, sizeof(*priv));

	priv->key_type   = RK_RSA_KEY_TYPE_PLAIN;

	priv->key.n      = rsa_2048_n;
	priv->key.n_len  = sizeof(rsa_2048_n);
	priv->key.e      = rsa_2048_e;
	priv->key.e_len  = sizeof(rsa_2048_e);
	priv->key.d      = rsa_2048_d;
	priv->key.d_len  = sizeof(rsa_2048_d);

#ifdef RSA_KEY_ADDITINAL_PARAM
	priv->key.p      = rsa_2048_p;
	priv->key.p_len  = sizeof(rsa_2048_d);
	priv->key.q      = rsa_2048_q;
	priv->key.q_len  = sizeof(rsa_2048_q);
	priv->key.dp     = rsa_2048_dp;
	priv->key.dp_len = sizeof(rsa_2048_dp);
	priv->key.dq     = rsa_2048_dq;
	priv->key.dq_len = sizeof(rsa_2048_dq);
	priv->key.qp     = rsa_2048_qp;
	priv->key.qp_len = sizeof(rsa_2048_qp);
#endif
}

static RK_RES test_rsa_pub_enc(uint32_t padding, const uint8_t *in, uint32_t in_len,
			       const uint8_t *expect, int verbose)
{
	RK_RES res = RK_CRYPTO_SUCCESS;
	uint8_t  *enc_buf = NULL;
	uint8_t  *dec_buf = NULL;
	uint32_t out_len;
	rk_rsa_pub_key_pack pub_key;
	rk_rsa_priv_key_pack priv_key;

	enc_buf = (uint8_t *)malloc(TEST_BUFFER_SIZE);
	if (!enc_buf) {
		printf("malloc for enc_buf failed\n");
		res = RK_CRYPTO_ERR_OUT_OF_MEMORY;
		goto exit;
	}

	dec_buf = (uint8_t *)malloc(TEST_BUFFER_SIZE);
	if (!dec_buf) {
		printf("malloc for dec_buf failed\n");
		res = RK_CRYPTO_ERR_OUT_OF_MEMORY;
		goto exit;
	}

	test_init_pubkey(&pub_key);
	test_init_privkey(&priv_key);

	res = rk_rsa_pub_encrypt(&pub_key, padding, in, in_len, enc_buf, &out_len);
	if (res) {
		printf("rk_rsa_pub_encrypt failed %x\n", res);
		goto exit;
	}

	if (expect) {
		if (memcmp(enc_buf, expect, out_len)) {
			printf("rk_rsa_pub_encrypt compare failed\n");
			test_dump_hex("result", enc_buf, out_len);
			test_dump_hex("expect", expect, TEST_BUFFER_SIZE);
			goto exit;
		}
	}

	res = rk_rsa_priv_decrypt(&priv_key, padding, enc_buf, out_len, dec_buf, &out_len);
	if (res) {
		printf("rk_rsa_priv_decrypt failed %x\n", res);
		goto exit;
	}

	if (in_len != out_len || memcmp(dec_buf, in, in_len)) {
		printf("rk_rsa_priv_decrypt compare failed\n");
		test_dump_hex("result", enc_buf, out_len);
		test_dump_hex("expect", in, in_len);
		goto exit;
	}

	if (verbose)
		TEST_END_PASS();

exit:
	if (enc_buf)
		free(enc_buf);

	if (dec_buf)
		free(dec_buf);

	if (res && verbose)
		TEST_END_FAIL();

	return res;
}

static RK_RES test_rsa_priv_enc(uint32_t padding, const uint8_t *in, uint32_t in_len,
				const uint8_t *expect, int verbose)
{
	RK_RES res = RK_CRYPTO_SUCCESS;
	uint8_t  *enc_buf = NULL;
	uint8_t  *dec_buf = NULL;
	uint32_t out_len;
	rk_rsa_pub_key_pack pub_key;
	rk_rsa_priv_key_pack priv_key;

	enc_buf = (uint8_t *)malloc(TEST_BUFFER_SIZE);
	if (!enc_buf) {
		printf("malloc for enc_buf failed\n");
		res = RK_CRYPTO_ERR_OUT_OF_MEMORY;
		goto exit;
	}

	dec_buf = (uint8_t *)malloc(TEST_BUFFER_SIZE);
	if (!dec_buf) {
		printf("malloc for dec_buf failed\n");
		res = RK_CRYPTO_ERR_OUT_OF_MEMORY;
		goto exit;
	}

	test_init_pubkey(&pub_key);
	test_init_privkey(&priv_key);

	res = rk_rsa_priv_encrypt(&priv_key, padding, in, in_len, enc_buf, &out_len);
	if (res) {
		printf("rk_rsa_priv_encrypt failed %x\n", res);
		goto exit;
	}

	if (expect) {
		if (memcmp(enc_buf, expect, out_len)) {
			printf("rk_rsa_priv_encrypt compare failed\n");
			test_dump_hex("result", enc_buf, out_len);
			test_dump_hex("expect", expect, TEST_BUFFER_SIZE);
			goto exit;
		}
	}

	res = rk_rsa_pub_decrypt(&pub_key, padding, enc_buf, out_len, dec_buf, &out_len);
	if (res) {
		printf("rk_rsa_pub_decrypt failed %x\n", res);
		goto exit;
	}

	if (in_len != out_len || memcmp(dec_buf, in, in_len)) {
		printf("rk_rsa_pub_decrypt compare failed\n");
		test_dump_hex("result", enc_buf, out_len);
		test_dump_hex("expect", in, in_len);
		goto exit;
	}

	if (verbose)
		TEST_END_PASS();

exit:
	if (enc_buf)
		free(enc_buf);


	if (dec_buf)
		free(dec_buf);

	if (res && verbose)
		TEST_END_FAIL();

	return res;
}

static RK_RES test_rsa_sign(uint32_t padding, const uint8_t *in, uint32_t in_len,
			    const uint8_t *expect, int verbose)
{
	RK_RES res = RK_CRYPTO_SUCCESS;
	uint8_t  *sign = NULL;
	uint32_t sign_len;
	rk_rsa_pub_key_pack pub_key;
	rk_rsa_priv_key_pack priv_key;

	sign = (uint8_t *)malloc(TEST_BUFFER_SIZE);
	if (!sign) {
		printf("malloc for sign failed\n");
		res = RK_CRYPTO_ERR_OUT_OF_MEMORY;
		goto exit;
	}

	test_init_pubkey(&pub_key);
	test_init_privkey(&priv_key);

	res = rk_rsa_sign(&priv_key, padding, in, in_len, sign, &sign_len);
	if (res) {
		printf("rk_rsa_sign failed %x\n", res);
		goto exit;
	}

	if (expect) {
		if (memcmp(sign, expect, sign_len)) {
			printf("rk_rsa_sign compare failed\n");
			test_dump_hex("result", sign, sign_len);
			test_dump_hex("expect", expect, TEST_BUFFER_SIZE);
			goto exit;
		}
	}

	res = rk_rsa_verify(&pub_key, padding, in, in_len, sign, sign_len);
	if (res) {
		printf("rk_rsa_verify failed %x\n", res);
		goto exit;
	}

	*sign = 0xaa;
	res = rk_rsa_verify(&pub_key, padding, in, in_len, sign, sign_len);
	if (res != RK_CRYPTO_ERR_VERIFY) {
		printf("rk_rsa_verify should be RK_CRYPTO_ERR_VERIFY but %x\n", res);
		goto exit;
	}

	res = RK_CRYPTO_SUCCESS;

	if (verbose)
		TEST_END_PASS();

exit:
	if (sign)
		free(sign);

	if (res && verbose)
		TEST_END_FAIL();

	return res;
}

RK_RES test_rsa(int verbose)
{
	RK_RES res = RK_CRYPTO_ERR_GENERIC;
	uint32_t i;

	res = rk_crypto_init();
	if (res) {
		printf("rk_crypto_init error %08x\n", res);
		return res;
	}

	for (i = 0; i < ARRAY_SIZE(test_rsa_tbl); i++) {
		res = test_rsa_tbl[i].do_test(test_rsa_tbl[i].padding,
					      test_rsa_tbl[i].in, test_rsa_tbl[i].in_len,
					      test_rsa_tbl[i].expect, verbose);
		if (res)
			goto exit;

		if (verbose)
			printf("\n");
	}
exit:
	rk_crypto_deinit();
	return res;
}
