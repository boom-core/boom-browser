// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/css/parser/css_parser_token_stream.h"

#include "testing/gtest/include/gtest/gtest.h"

#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"

namespace blink {

namespace {

TEST(CSSParserTokenStreamTest, EmptyStream) {
  CSSTokenizer tokenizer(String(""));
  CSSParserTokenStream stream(tokenizer);
  EXPECT_TRUE(stream.Consume().IsEOF());
  EXPECT_TRUE(stream.Peek().IsEOF());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, PeekThenConsume) {
  CSSTokenizer tokenizer(String("A"));  // kIdent
  CSSParserTokenStream stream(tokenizer);
  EXPECT_EQ(kIdentToken, stream.Peek().GetType());
  EXPECT_EQ(kIdentToken, stream.Consume().GetType());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, ConsumeThenPeek) {
  CSSTokenizer tokenizer(String("A"));  // kIdent
  CSSParserTokenStream stream(tokenizer);
  EXPECT_EQ(kIdentToken, stream.Consume().GetType());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, ConsumeMultipleTokens) {
  CSSTokenizer tokenizer(String("A 1"));  // kIdent kWhitespace kNumber
  CSSParserTokenStream stream(tokenizer);
  EXPECT_EQ(kIdentToken, stream.Consume().GetType());
  EXPECT_EQ(kWhitespaceToken, stream.Consume().GetType());
  EXPECT_EQ(kNumberToken, stream.Consume().GetType());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, UncheckedPeekAndConsumeAfterPeek) {
  CSSTokenizer tokenizer(String("A"));  // kIdent
  CSSParserTokenStream stream(tokenizer);
  EXPECT_EQ(kIdentToken, stream.Peek().GetType());
  EXPECT_EQ(kIdentToken, stream.UncheckedPeek().GetType());
  EXPECT_EQ(kIdentToken, stream.UncheckedConsume().GetType());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, UncheckedPeekAndConsumeAfterAtEnd) {
  CSSTokenizer tokenizer(String("A"));  // kIdent
  CSSParserTokenStream stream(tokenizer);
  EXPECT_FALSE(stream.AtEnd());
  EXPECT_EQ(kIdentToken, stream.UncheckedPeek().GetType());
  EXPECT_EQ(kIdentToken, stream.UncheckedConsume().GetType());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, UncheckedConsumeComponentValue) {
  CSSTokenizer tokenizer(String("A{1}{2{3}}B"));
  CSSParserTokenStream stream(tokenizer);

  EXPECT_EQ(kIdentToken, stream.Peek().GetType());
  stream.UncheckedConsumeComponentValue();
  EXPECT_EQ(kLeftBraceToken, stream.Peek().GetType());
  stream.UncheckedConsumeComponentValue();
  EXPECT_EQ(kLeftBraceToken, stream.Peek().GetType());
  stream.UncheckedConsumeComponentValue();
  EXPECT_EQ(kIdentToken, stream.Peek().GetType());
  stream.UncheckedConsumeComponentValue();

  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, ConsumeWhitespace) {
  CSSTokenizer tokenizer(String(" \t\n"));  // kWhitespace
  CSSParserTokenStream stream(tokenizer);

  EXPECT_EQ(kWhitespaceToken, stream.Consume().GetType());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, ConsumeIncludingWhitespace) {
  CSSTokenizer tokenizer(String("A \t\n"));  // kIdent kWhitespace
  CSSParserTokenStream stream(tokenizer);

  EXPECT_EQ(kIdentToken, stream.ConsumeIncludingWhitespace().GetType());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, RangesDoNotGetInvalidatedWhenConsuming) {
  StringBuilder s;
  s.Append("1 ");
  for (int i = 0; i < 100; i++) {
    s.Append("A ");
  }

  CSSTokenizer tokenizer(s.ToString());
  CSSParserTokenStream stream(tokenizer);

  // Consume a single token range.
  auto range = stream.ConsumeUntilPeekedTypeIs<kIdentToken>();

  EXPECT_EQ(kNumberToken, range.Peek().GetType());

  // Consume remaining tokens to try to invalidate the range.
  while (!stream.AtEnd()) {
    stream.ConsumeIncludingWhitespace();
  }

  EXPECT_EQ(kNumberToken, range.ConsumeIncludingWhitespace().GetType());
  EXPECT_TRUE(range.AtEnd());
}

TEST(CSSParserTokenStreamTest, BlockErrorRecoveryConsumesRestOfBlock) {
  CSSTokenizer tokenizer(String("{B }1"));
  CSSParserTokenStream stream(tokenizer);

  {
    CSSParserTokenStream::BlockGuard guard(stream);
    EXPECT_EQ(kIdentToken, stream.Consume().GetType());
    EXPECT_FALSE(stream.AtEnd());
  }  // calls destructor

  EXPECT_EQ(kNumberToken, stream.Consume().GetType());
}

TEST(CSSParserTokenStreamTest, BlockErrorRecoveryOnSuccess) {
  CSSTokenizer tokenizer(String("{B }1"));
  CSSParserTokenStream stream(tokenizer);

  {
    CSSParserTokenStream::BlockGuard guard(stream);
    EXPECT_EQ(kIdentToken, stream.Consume().GetType());
    EXPECT_EQ(kWhitespaceToken, stream.Consume().GetType());
    EXPECT_TRUE(stream.AtEnd());
  }  // calls destructor

  EXPECT_EQ(kNumberToken, stream.Consume().GetType());
}

TEST(CSSParserTokenStreamTest, BlockErrorRecoveryConsumeComponentValue) {
  CSSTokenizer tokenizer(String("{{B} C}1"));
  CSSParserTokenStream stream(tokenizer);

  {
    CSSParserTokenStream::BlockGuard guard(stream);
    stream.EnsureLookAhead();
    stream.UncheckedConsumeComponentValue();
  }  // calls destructor

  EXPECT_EQ(kNumberToken, stream.Consume().GetType());
}

TEST(CSSParserTokenStreamTest, OffsetAfterPeek) {
  CSSTokenizer tokenizer(String("ABC"));
  CSSParserTokenStream stream(tokenizer);

  EXPECT_EQ(0U, stream.Offset());
  EXPECT_EQ(kIdentToken, stream.Peek().GetType());
  EXPECT_EQ(0U, stream.Offset());
}

TEST(CSSParserTokenStreamTest, OffsetAfterConsumes) {
  CSSTokenizer tokenizer(String("ABC 1 {23 }"));
  CSSParserTokenStream stream(tokenizer);

  EXPECT_EQ(0U, stream.Offset());
  EXPECT_EQ(kIdentToken, stream.Consume().GetType());
  EXPECT_EQ(3U, stream.Offset());
  EXPECT_EQ(kWhitespaceToken, stream.Consume().GetType());
  EXPECT_EQ(4U, stream.Offset());
  EXPECT_EQ(kNumberToken, stream.ConsumeIncludingWhitespace().GetType());
  EXPECT_EQ(6U, stream.Offset());
  stream.EnsureLookAhead();
  stream.UncheckedConsumeComponentValue();
  EXPECT_EQ(11U, stream.Offset());
}

TEST(CSSParserTokenStreamTest, LookAheadOffset) {
  CSSTokenizer tokenizer(String("ABC/* *//* */1"));
  CSSParserTokenStream stream(tokenizer);

  stream.EnsureLookAhead();
  EXPECT_EQ(0U, stream.Offset());
  EXPECT_EQ(0U, stream.LookAheadOffset());
  EXPECT_EQ(kIdentToken, stream.Consume().GetType());

  stream.EnsureLookAhead();
  EXPECT_EQ(3U, stream.Offset());
  EXPECT_EQ(13U, stream.LookAheadOffset());
}

TEST(CSSParserTokenStreamTest, ConsumeUntilPeekedTypeIsEmpty) {
  CSSTokenizer tokenizer(String("{23 }"));
  CSSParserTokenStream stream(tokenizer);

  auto range = stream.ConsumeUntilPeekedTypeIs<>();
  EXPECT_TRUE(stream.AtEnd());

  EXPECT_EQ(kLeftBraceToken, range.Consume().GetType());
  EXPECT_EQ(kNumberToken, range.Consume().GetType());
  EXPECT_EQ(kWhitespaceToken, range.Consume().GetType());
  EXPECT_EQ(kRightBraceToken, range.Consume().GetType());
  EXPECT_TRUE(range.AtEnd());
}

TEST(CSSParserTokenStreamTest, Boundary) {
  CSSTokenizer tokenizer(String("foo:red;bar:blue;asdf"));
  CSSParserTokenStream stream(tokenizer);

  {
    CSSParserTokenStream::Boundary boundary(stream, kSemicolonToken);
    CSSParserTokenRange range = stream.ConsumeUntilPeekedTypeIs<>();
    EXPECT_EQ("foo", range.Consume().Value());
    EXPECT_EQ(kColonToken, range.Consume().GetType());
    EXPECT_EQ("red", range.Consume().Value());
    EXPECT_TRUE(stream.AtEnd());
  }

  EXPECT_FALSE(stream.AtEnd());
  EXPECT_EQ(kSemicolonToken, stream.Consume().GetType());

  {
    CSSParserTokenStream::Boundary boundary(stream, kSemicolonToken);
    CSSParserTokenRange range = stream.ConsumeUntilPeekedTypeIs<>();
    EXPECT_EQ("bar", range.Consume().Value());
    EXPECT_EQ(kColonToken, range.Consume().GetType());
    EXPECT_EQ("blue", range.Consume().Value());
    EXPECT_TRUE(stream.AtEnd());
  }

  EXPECT_FALSE(stream.AtEnd());
  EXPECT_EQ(kSemicolonToken, stream.Consume().GetType());

  EXPECT_EQ("asdf", stream.Consume().Value());
  EXPECT_TRUE(stream.AtEnd());
}

TEST(CSSParserTokenStreamTest, MultipleBoundaries) {
  CSSTokenizer tokenizer(String("a:b,c;d:,;e"));
  CSSParserTokenStream stream(tokenizer);

  {
    CSSParserTokenStream::Boundary boundary_semicolon(stream, kSemicolonToken);

    {
      CSSParserTokenStream::Boundary boundary_comma(stream, kCommaToken);

      {
        CSSParserTokenStream::Boundary boundary_colon(stream, kColonToken);
        CSSParserTokenRange range = stream.ConsumeUntilPeekedTypeIs<>();
        EXPECT_EQ("a", range.Consume().Value());
        EXPECT_TRUE(range.AtEnd());
        EXPECT_TRUE(stream.AtEnd());
      }

      EXPECT_FALSE(stream.AtEnd());
      EXPECT_EQ(kColonToken, stream.Consume().GetType());

      CSSParserTokenRange range = stream.ConsumeUntilPeekedTypeIs<>();
      EXPECT_EQ("b", range.Consume().Value());
      EXPECT_TRUE(range.AtEnd());
      EXPECT_TRUE(stream.AtEnd());
    }

    EXPECT_FALSE(stream.AtEnd());
    EXPECT_EQ(kCommaToken, stream.Consume().GetType());

    CSSParserTokenRange range = stream.ConsumeUntilPeekedTypeIs<>();
    EXPECT_EQ("c", range.Consume().Value());
    EXPECT_TRUE(range.AtEnd());
    EXPECT_TRUE(stream.AtEnd());
  }

  EXPECT_FALSE(stream.AtEnd());
  EXPECT_EQ(kSemicolonToken, stream.Consume().GetType());

  CSSParserTokenRange range = stream.ConsumeUntilPeekedTypeIs<>();
  EXPECT_TRUE(stream.AtEnd());

  EXPECT_EQ("d", range.Consume().Value());
  EXPECT_EQ(kColonToken, range.Consume().GetType());
  EXPECT_EQ(kCommaToken, range.Consume().GetType());
  EXPECT_EQ(kSemicolonToken, range.Consume().GetType());
  EXPECT_EQ("e", range.Consume().Value());
}

TEST(CSSParserTokenStreamTest, IneffectiveBoundary) {
  CSSTokenizer tokenizer(String("a:b|"));
  CSSParserTokenStream stream(tokenizer);

  {
    CSSParserTokenStream::Boundary boundary_colon(stream, kColonToken);

    {
      // It's valid to add another boundary, but it has no affect in this
      // case, since kColonToken appears first.
      CSSParserTokenStream::Boundary boundary_semicolon(stream,
                                                        kSemicolonToken);

      CSSParserTokenRange range = stream.ConsumeUntilPeekedTypeIs<>();
      EXPECT_EQ("a", range.Consume().Value());
      EXPECT_TRUE(range.AtEnd());

      EXPECT_EQ(kColonToken, stream.Peek().GetType());
      EXPECT_TRUE(stream.AtEnd());
    }

    EXPECT_TRUE(stream.AtEnd());
  }

  EXPECT_FALSE(stream.AtEnd());
}

}  // namespace

}  // namespace blink
