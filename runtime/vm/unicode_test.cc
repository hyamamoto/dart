// Copyright (c) 2011, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "vm/globals.h"
#include "vm/unicode.h"
#include "vm/unit_test.h"

namespace dart {

TEST_CASE(Utf8Decode) {
  // Examples from the Unicode specification, chapter 3
  {
    const char* src = "\x41\xC3\xB1\x42";
    uint32_t expected[] = { 0x41, 0xF1, 0x42 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  {
    const char* src = "\x4D";
    uint32_t expected[] = { 0x4D };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  {
    const char* src = "\xD0\xB0";
    uint32_t expected[] = { 0x430 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  {
    const char* src = "\xE4\xBA\x8C";
    uint32_t expected[] = { 0x4E8C };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  {
    const char* src = "\xF0\x90\x8C\x82";
    uint32_t expected[] = { 0x10302 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  {
    const char* src = "\x4D\xD0\xB0\xE4\xBA\x8C\xF0\x90\x8C\x82";
    uint32_t expected[] = { 0x4D, 0x430, 0x4E8C, 0x10302 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // Mixture of non-ASCII and ASCII characters
  {
    const char* src = "\xD7\x92\xD7\x9C\xD7\xA2\xD7\x93"
                      "\x20"
                      "\xD7\x91\xD7\xA8\xD7\x9B\xD7\x94";
    uint32_t expected[] = { 0x5D2, 0x5DC, 0x5E2, 0x5D3,
                            0x20,
                            0x5D1, 0x5E8, 0x5DB, 0x5D4 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt

  // 1 - Some correct UTF-8 text
  {
    const char* src = "\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5";
    uint32_t expected[] = { 0x3BA, 0x1F79, 0x3C3, 0x3BC, 0x3B5 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2 - Boundary condition test cases

  // 2.1 - First possible sequence of a certain length

  // 2.1.1 - 1 byte (U-00000000):        "\x00"
  {
    const char* src = "\x00";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 2.1.2 - 2 bytes (U-00000080):        "\xC2\x80"
  {
    const char* src = "\xC2\x80";
    uint32_t expected[] = { 0x80 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.1.3 - 3 bytes (U-00000800):        "\xE0\xA0\x80"
  {
    const char* src = "\xE0\xA0\x80";
    uint32_t expected[] = { 0x800 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.1.4 - 4 bytes (U-00010000):        "\xF0\x90\x80\x80"
  {
    const char* src = "\xF0\x90\x80\x80";
    uint32_t expected[] = { 0x10000 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.1.5 - 5 bytes (U-00200000):        "\xF8\x88\x80\x80\x80"
  {
    const char* src = "\xF8\x88\x80\x80\x80";
    uint32_t expected[] = { 0x200000 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 2.1.6 - 6 bytes (U-04000000):        "\xFC\x84\x80\x80\x80\x80"
  {
    const char* src = "\xFC\x84\x80\x80\x80\x80";
    uint32_t expected[] = { 0x400000 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 2.2 - Last possible sequence of a certain length

  // 2.2.1 - 1 byte (U-0000007F):        "\x7F"
  {
    const char* src = "\x7F";
    uint32_t expected[] = { 0x7F };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.2.2 - 2 bytes (U-000007FF):        "\xDF\xBF"
  {
    const char* src = "\xDF\xBF";
    uint32_t expected[] = { 0x7FF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.2.3 - 3 bytes (U-0000FFFF):        "\xEF\xBF\xBF"
  {
    const char* src = "\xEF\xBF\xBF";
    uint32_t expected[] = { 0xFFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.2.4 - 4 bytes (U-001FFFFF):        "\xF7\xBF\xBF\xBF"
  {
    const char* src = "\xF7\xBF\xBF\xBF";
    uint32_t expected[] = { 0x1FFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 2.2.5 - 5 bytes (U-03FFFFFF):        "\xFB\xBF\xBF\xBF\xBF"
  {
    const char* src = "\xFB\xBF\xBF\xBF\xBF";
    uint32_t expected[] = { 0x3FFFFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 2.2.6 - 6 bytes (U-7FFFFFFF):        "\xFD\xBF\xBF\xBF\xBF\xBF"
  {
    const char* src = "\xFD\xBF\xBF\xBF\xBF\xBF";
    uint32_t expected[] = { 0x7FFFFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 2.3 - Other boundary conditions

  // 2.3.1 - U-0000D7FF = ed 9f bf = "\xED\x9F\xBF"
  {
    const char* src = "\xED\x9F\xBF";
    uint32_t expected[] = { 0xD7FF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.3.2 - U-0000E000 = ee 80 80 = "\xEE\x80\x80"
  {
    const char* src = "\xEE\x80\x80";
    uint32_t expected[] = { 0xE000 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.3.3 - U-0000FFFD = ef bf bd = "\xEF\xBF\xBD"
  {
    const char* src = "\xEF\xBF\xBD";
    uint32_t expected[] = { 0xFFFD };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.3.4 - U-0010FFFF = f4 8f bf bf = "\xF4\x8F\xBF\xBF"
  {
    const char* src = "\xF4\x8F\xBF\xBF";
    uint32_t expected[] = { 0x10FFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 2.3.5 - U-00110000 = f4 90 80 80 = "\xF4\x90\x80\x80"
  {
    const char* src = "\xF4\x90\x80\x80";
    uint32_t expected[] = { 0x110000 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3 - Malformed sequences

  // 3.1 - Unexpected continuation bytes

  // 3.1.1 - First continuation byte 0x80: "\x80"
  {
    const char* src = "\x80";
    uint32_t expected[] = { 0x80 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.1.2 - Last continuation byte 0xbf: "\xBF"
  {
    const char* src = "\xBF";
    uint32_t expected[] = { 0xBF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.1.3 - 2 continuation bytes: "\x80\xBF"
  {
    const char* src = "\x80\xBF";
    uint32_t expected[] = { 0x80, 0xBF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.1.4 - 3 continuation bytes: "\x80\xBF\x80"
  {
    const char* src = "\x80\xBF\x80";
    uint32_t expected[] = { 0x80, 0xBF, 0x80 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.1.5 - 4 continuation bytes: "\x80\xBF\x80\xBF"
  {
    const char* src = "\x80\xBF\x80\xBF";
    uint32_t expected[] = { 0x80, 0xBF, 0x80, 0xBF  };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.1.6 - 5 continuation bytes: "\x80\xBF\x80\xBF\x80"
  {
    const char* src = "\x80\xBF\x80\xBF\x80";
    uint32_t expected[] = { 0x80, 0xBF, 0x80, 0xBF, 0x80 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.1.7 - 6 continuation bytes: "\x80\xBF\x80\xBF\x80\xBF"
  {
    const char* src = "\x80\xBF\x80\xBF\x80\xBF";
    uint32_t expected[] = { 0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.1.8 - 7 continuation bytes: "\x80\xBF\x80\xBF\x80\xBF\x80"
  {
    const char* src = "\x80\xBF\x80\xBF\x80\xBF\x80";
    uint32_t expected[] = { 0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF, 0x80 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.1.9 - Sequence of all 64 possible continuation bytes (0x80-0xbf):
  {
    const char* src = "\x80\x81\x82\x83\x84\x85\x86\x87"
                      "\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F"
                      "\x90\x91\x92\x93\x94\x95\x96\x97"
                      "\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F"
                      "\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7"
                      "\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF"
                      "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7"
                      "\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    for (size_t i = 0; i < strlen(src); ++i) {
      memset(dst, 0xFF, sizeof(dst));
      bool is_valid = Utf8::Decode(&src[i], dst, ARRAY_SIZE(dst));
      EXPECT(!is_valid);
      EXPECT(memcmp(expected, dst, sizeof(expected)));
    }
  }

  // 3.2 - Lonely start character

  // 3.2.1 - All 32 first bytes of 2-byte sequences (0xc0-0xdf), each
  //         followed by a space character:
  {
    const char* src = "\xC0\x20\xC1\x20\xC2\x20\xC3\x20"
                      "\xC4\x20\xC5\x20\xC6\x20\xC7\x20"
                      "\xC8\x20\xC9\x20\xCA\x20\xCB\x20"
                      "\xCC\x20\xCD\x20\xCE\x20\xCF\x20"
                      "\xD0\x20\xD1\x20\xD2\x20\xD3\x20"
                      "\xD4\x20\xD5\x20\xD6\x20\xD7\x20"
                      "\xD8\x20\xD9\x20\xDA\x20\xDB\x20"
                      "\xDC\x20\xDD\x20\xDE\x20\xDF\x20";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    for (size_t i = 0; i < strlen(src); i += 2) {
      memset(dst, 0xFF, sizeof(dst));
      bool is_valid = Utf8::Decode(&src[i], dst, ARRAY_SIZE(dst));
      EXPECT(!is_valid);
      EXPECT(memcmp(expected, dst, sizeof(expected)));
    }
  }

  // 3.2.2 - All 16 first bytes of 3-byte sequences (0xe0-0xef), each
  //         followed by a space character:
  {
    const char* src = "\xE0\x20\xE1\x20\xE2\x20\xE3\x20"
                      "\xE4\x20\xE5\x20\xE6\x20\xE7\x20"
                      "\xE8\x20\xE9\x20\xEA\x20\xEB\x20"
                      "\xEC\x20\xED\x20\xEE\x20\xEF\x20";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    for (size_t i = 0; i < strlen(src); i += 2) {
      memset(dst, 0xFF, sizeof(dst));
      bool is_valid = Utf8::Decode(&src[i], dst, ARRAY_SIZE(dst));
      EXPECT(!is_valid);
      EXPECT(memcmp(expected, dst, sizeof(expected)));
    }
  }

  // 3.2.3 - All 8 first bytes of 4-byte sequences (0xf0-0xf7), each
  //         followed by a space character:
  {
    const char* src = "\xF0\x20\xF1\x20\xF2\x20\xF3\x20"
                      "\xF4\x20\xF5\x20\xF6\x20\xF7\x20";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    for (size_t i = 0; i < strlen(src); i += 2) {
      memset(dst, 0xFF, sizeof(dst));
      bool is_valid = Utf8::Decode(&src[i], dst, ARRAY_SIZE(dst));
      EXPECT(!is_valid);
      EXPECT(memcmp(expected, dst, sizeof(expected)));
    }
  }

  // 3.2.4 - All 4 first bytes of 5-byte sequences (0xf8-0xfb), each
  //         followed by a space character:
  {
    const char* src = "\xF8\x20\xF9\x20\xFA\x20\xFB\x20";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    for (size_t i = 0; i < strlen(src); i += 2) {
      memset(dst, 0xFF, sizeof(dst));
      bool is_valid = Utf8::Decode(&src[i], dst, ARRAY_SIZE(dst));
      EXPECT(!is_valid);
      EXPECT(memcmp(expected, dst, sizeof(expected)));
    }
  }

  // 3.2.5 - All 2 first bytes of 6-byte sequences (0xfc-0xfd), each
  //         followed by a space character:
  {
    const char* src = "\xFC\x20\xFD\x20";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    for (size_t i = 0; i < strlen(src); i += 2) {
      memset(dst, 0xFF, sizeof(dst));
      bool is_valid = Utf8::Decode(&src[i], dst, ARRAY_SIZE(dst));
      EXPECT(!is_valid);
      EXPECT(memcmp(expected, dst, sizeof(expected)));
    }
  }

  // 3.3 - Sequences with last continuation byte missing

  // 3.3.1 - 2-byte sequence with last byte missing (U+0000): "\xC0"
  {
    const char* src = "\xC0";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.2 - 3-byte sequence with last byte missing (U+0000): "\xE0\x80"
  {
    const char* src = "\xE0\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.3 - 4-byte sequence with last byte missing (U+0000): "\xF0\x80\x80"
  {
    const char* src = "\xF0\x80\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.4 - 5-byte sequence with last byte missing (U+0000): "\xF8\x80\x80\x80"
  {
    const char* src = "\xF8\x80\x80\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.5 - 6-byte sequence with last byte missing (U+0000):
  // "\xFC\x80\x80\x80\x80"
  {
    const char* src = "\xFC\x80\x80\x80\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.6 - 2-byte sequence with last byte missing (U-000007FF): "\xDF"
  {
    const char* src = "\xDF";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.7 - 3-byte sequence with last byte missing (U-0000FFFF): "\xEF\xBF"
  {
    const char* src = "\xEF\xBF";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.8 - 4-byte sequence with last byte missing (U-001FFFFF): "\xF7\xBF\xBF"
  {
    const char* src = "\xF7\xBF\xBF";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.9 - 5-byte sequence with last byte missing (U-03FFFFFF):
  // "\xFB\xBF\xBF\xBF"
  {
    const char* src = "\xFB\xBF\xBF\xBF";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.3.10 - 6-byte sequence with last byte missing (U-7FFFFFFF):
  // "\xFD\xBF\xBF\xBF\xBF"
  {
    const char* src = "\xFD\xBF\xBF\xBF\xBF";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.4 - Concatenation of incomplete sequences
  {
    const char* src = "\xC0\xE0\x80\xF0\x80\x80"
                      "\xF8\x80\x80\x80\xFC\x80"
                      "\x80\x80\x80\xDF\xEF\xBF"
                      "\xF7\xBF\xBF\xFB\xBF\xBF"
                      "\xBF\xFD\xBF\xBF\xBF\xBF";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    for (size_t i = 0; i < strlen(src); ++i) {
      for (size_t j = 1; j < (strlen(src) - i); ++j) {
        memset(dst, 0xFF, sizeof(dst));
        bool is_valid = Utf8::Decode(&src[i], dst, ARRAY_SIZE(dst));
        EXPECT(!is_valid);
        EXPECT(memcmp(expected, dst, sizeof(expected)));
      }
    }
  }

  // 3.5 - Impossible bytes

  // 3.5.1 - fe = "\xFE"
  {
    const char* src = "\xFE";
    uint32_t expected[] = { 0xFE };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.5.2 - ff = "\xFF"
  {
    const char* src = "\xFF";
    uint32_t expected[] = { 0xFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 3.5.3 - fe fe ff ff = "\xFE\xFE\xFF\xFF"
  {
    const char* src = "\xFE\xFE\xFF\xFF";
    uint32_t expected[] = { 0xFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4 - Overlong sequences

  // 4.1 - Examples of an overlong ASCII character

  // 4.1.1 - U+002F = c0 af             = "\xC0\xAF"
  {
    const char* src = "\xC0\xAF";
    uint32_t expected[] = { 0x2F };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.1.2 - U+002F = e0 80 af          = "\xE0\x80\xAF"
  {
    const char* src = "\xE0\x80\xAF";
    uint32_t expected[] = { 0x2F };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.1.3 - U+002F = f0 80 80 af       = "\xF0\x80\x80\xAF"
  {
    const char* src = "\xF0\x80\x80\xAF";
    uint32_t expected[] = { 0x2F };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.1.4 - U+002F = f8 80 80 80 af    = "\xF8\x80\x80\x80\xAF"
  {
    const char* src = "\xF8\x80\x80\x80\xAF";
    uint32_t expected[] = { 0x2F };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.1.5 - U+002F = fc 80 80 80 80 af = "\xFC\x80\x80\x80\x80\xAF"
  {
    const char* src = "\xFC\x80\x80\x80\x80\xAF";
    uint32_t expected[] = { 0x2F };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.2  Maximum overlong sequences

  // 4.2.1 - U-0000007F = c1 bf             = "\xC1\xBF"
  {
    const char* src = "\xC1\xBF";
    uint32_t expected[] = { 0x7F };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.2.2 U+000007FF = e0 9f bf          = "\xE0\x9F\xBF"
  {
    const char* src = "\xE0\x9F\xBF";
    uint32_t expected[] = { 0x7FF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.2.3 - U+0000FFFF = f0 8f bf bf       = "\xF0\x8F\xBF\xBF"
  {
    const char* src = "\xF0\x8F\xBF\xBF";
    uint32_t expected[] = { 0xFFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.2.4  U-001FFFFF = f8 87 bf bf bf    = "\xF8\x87\xBF\xBF\xBF"
  {
    const char* src = "\xF8\x87\xBF\xBF\xBF";
    uint32_t expected[] = { 0x1FFFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.2.5  U-03FFFFFF = fc 83 bf bf bf bf = "\xFC\x83\xBF\xBF\xBF\xBF"
  {
    const char* src = "\xFC\x83\xBF\xBF\xBF\xBF";
    uint32_t expected[] = { 0x3FFFFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.3 - Overlong representation of the NUL character

  // 4.3.1 - U+0000 = "\xC0\x80"
  {
    const char* src = "\xC0\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.3.2  U+0000 = e0 80 80 = "\xE0\x80\x80"
  {
    const char* src = "\xE0\x80\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.3.3  U+0000 = f0 80 80 80 = "\xF0\x80\x80\x80"
  {
    const char* src = "\xF0\x80\x80\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.3.4  U+0000 = f8 80 80 80 80 = "\xF8\x80\x80\x80\x80"
  {
    const char* src = "\xF8\x80\x80\x80\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 4.3.5  U+0000 = fc 80 80 80 80 80 = "\xFC\x80\x80\x80\x80\x80"
  {
    const char* src = "\xFC\x80\x80\x80\x80\x80";
    uint32_t expected[] = { 0x0 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0xFF, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.1 - Single UTF-16 surrogates

  // 5.1.1 - U+D800 = ed a0 80 = "\xED\xA0\x80"
  {
    const char* src = "\xED\xA0\x80";
    uint32_t expected[] = { 0xD800 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.1.2 - U+DB7F = ed ad bf = "\xED\xAD\xBF"
  {
    const char* src = "\xED\xAD\xBF";
    uint32_t expected[] = { 0xDB7F };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.1.3 - U+DB80 = ed ae 80 = "\xED\xAE\x80"
  {
    const char* src = "\xED\xAE\x80";
    uint32_t expected[] = { 0xDB80 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.1.4 - U+DBFF = ed af bf = "\xED\xAF\xBF"
  {
    const char* src = "\xED\xAF\xBF";
    uint32_t expected[] = { 0xDBFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.1.5 - U+DC00 = ed b0 80 = "\xED\xB0\x80"
  {
    const char* src = "\xED\xB0\x80";
    uint32_t expected[] = { 0xDC00 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.1.6 - U+DF80 = ed be 80 = "\xED\xBE\x80"
  {
    const char* src = "\xED\xBE\x80";
    uint32_t expected[] = { 0xDF80 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.1.7 - U+DFFF = ed bf bf = "\xED\xBF\xBF"
  {
    const char* src = "\xED\xBF\xBF";
    uint32_t expected[] = { 0xDFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.2 Paired UTF-16 surrogates

  // 5.2.1 - U+D800 U+DC00 = ed a0 80 ed b0 80 = "\xED\xA0\x80\xED\xB0\x80"
  {
    const char* src = "\xED\xA0\x80\xED\xB0\x80";
    uint32_t expected[] = { 0xD800, 0xDC00 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.2.2 - U+D800 U+DFFF = ed a0 80 ed bf bf = "\xED\xA0\x80\xED\xBF\xBF"
  {
    const char* src = "\xED\xA0\x80\xED\xBF\xBF";
    uint32_t expected[] = { 0xD800, 0xDFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.2.3 - U+DB7F U+DC00 = ed a0 80 ed bf bf = "\xED\xAD\xBF\xED\xB0\x80"
  {
    const char* src = "\xED\xAD\xBF\xED\xB0\x80";
    uint32_t expected[] = { 0xDB7F, 0xDC00 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.2.4 - U+DB7F U+DFFF = ed ad bf ed bf bf = "\xED\xAD\xBF\xED\xBF\xBF"
  {
    const char* src = "\xED\xAD\xBF\xED\xBF\xBF";
    uint32_t expected[] = { 0xDB7F, 0xDFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.2.5 - U+DB80 U+DC00 = ed ae 80 ed b0 80 = "\xED\xAE\x80\xED\xB0\x80"
  {
    const char* src = "\xED\xAE\x80\xED\xB0\x80";
    uint32_t expected[] = { 0xDB80, 0xDC00 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.2.6 - U+DB80 U+DFFF = ed ae 80 ed bf bf = "\xED\xAE\x80\xED\xBF\xBF"
  {
    const char* src = "\xED\xAE\x80\xED\xBF\xBF";
    uint32_t expected[] = { 0xDB80, 0xDFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.2.7 - U+DBFF U+DC00 = ed af bf ed b0 80 = "\xED\xAF\xBF\xED\xB0\x80"
  {
    const char* src = "\xED\xAF\xBF\ED\xB0\x80";
    uint32_t expected[] = { 0xDBFF, 0xDC00 };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.2.8 - U+DBFF U+DFFF = ed af bf ed bf bf = "\xED\xAF\xBF\xED\xBF\xBF"
  {
    const char* src = "\xED\xAF\xBF\xED\xBF\xBF";
    uint32_t expected[] = { 0xDBFF, 0xDFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(!is_valid);
    EXPECT(memcmp(expected, dst, sizeof(expected)));
  }

  // 5.3 - Other illegal code positions

  // 5.3.1 - U+FFFE = ef bf be = "\xEF\xBF\xBE"
  {
    const char* src = "\xEF\xBF\xBE";
    uint32_t expected[] = { 0xFFFE };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }

  // 5.3.2 - U+FFFF = ef bf bf = "\xEF\xBF\xBF"
  {
    const char* src = "\xEF\xBF\xBF";
    uint32_t expected[] = { 0xFFFF };
    uint32_t dst[ARRAY_SIZE(expected)];
    memset(dst, 0, sizeof(dst));
    bool is_valid = Utf8::Decode(src, dst, ARRAY_SIZE(dst));
    EXPECT(is_valid);
    EXPECT(!memcmp(expected, dst, sizeof(expected)));
  }
}

}  // namespace dart
