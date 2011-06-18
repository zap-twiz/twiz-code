
#include "lexer/lex_utils.h"
#include "lexer/line_counting_stream.h"
#include "lexer/token_stream.h"
#include "lexer/trie.h"

namespace {

// Internal keywords and string values used by the lexer.
typedef enum KeyWord {
  LEFT_SQUARE_BRACE = 0,
  RIGHT_SQUARE_BRACE,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  DOT,
  DOT_DOT,
  COMMA,
  SEMI_COLON,
  COLON,
  RIGHT_ARROW,
  LEFT_ARROW,
  CHIP,
  IMPORT,
  WIRE,
  TRUE,
  FALSE,
  HIGH,
  LOW,
  CC_COMMENT,
  CPP_COMMENT_PREFIX,
  HEX_PREFIX,
  BINARY_PREFIX,
  UNKNOWN,
  NUM_KEY_WORDS
};

const char* kKeyWords[] = {
  "[",
  "]",
  "(",
  ")",
  "{",
  "}",
  ".",
  "..",
  ",",
  ";",
  ":",
  "->",
  "<-",
  "chip",
  "import",
  "wire",
  "TRUE",
  "FALSE",
  "HIGH",
  "LOW",
  "//",
  "/*",
  "0x",
  "b",
};

class TokenConsumer {
 public:
  TokenConsumer() {
    for (char x = 0; x < NUM_KEY_WORDS; ++x)
      trie_.insert(kKeyWords[x], static_cast<char>(x));
  };

  KeyWord NextToken(std::string* token,
                    LineCountingStream& stream);

 private:
  // Store the enum types as chars, so that value semantics will
  // be used.  See pointer traits.
  typedef TrieMap<char, char, std::string> KeywordTrie;
  KeywordTrie trie_;
};

TokenConsumer kTokenConsumer;

KeyWord TokenConsumer::NextToken(std::string* token,
                                 LineCountingStream& stream) {
  KeywordTrie::Iterator iter(trie_.root()), end(trie_.end());

  KeyWord key_word = UNKNOWN;
  std::string consumed;
  *token = "";
  while (!stream.IsEOS()) {
    char read = stream.Get();
    consumed += read;
    iter = iter.next(read);

    if (iter.exists()) {
      *token += consumed;
      consumed.clear();

      key_word = static_cast<KeyWord>(iter.value());
    }

    if (end == iter)
      break;
  }

  // Unget the data that was not matched to a token prefix.
  size_t offset = consumed.length();
  while (offset > 0) {
    stream.Unget(consumed[--offset]);
  }

  return key_word;
}

} // namespace

Token TokenStream::Get() {
  SkipWhiteSpace(input_stream_);

  if (input_stream_.IsEOS())
    return Token(input_stream_.GetCount(), Token::UNKNOWN, "");

  size_t current_line = input_stream_.GetCount();

  std::string token;
  KeyWord key_word = kTokenConsumer.NextToken(&token, input_stream_);
  Token::TokenType token_type = Token::UNKNOWN;

  bool scan_identifier = false;
  switch (key_word) {
    case ::RIGHT_SQUARE_BRACE: token_type = Token::RIGHT_SQUARE_BRACE; break;
    case ::LEFT_SQUARE_BRACE: token_type = Token::LEFT_SQUARE_BRACE; break;
    case ::RIGHT_PAREN: token_type = Token::RIGHT_PAREN; break;
    case ::LEFT_PAREN: token_type = Token::LEFT_PAREN; break;
    case ::RIGHT_BRACE:token_type = Token::RIGHT_BRACE; break;
    case ::LEFT_BRACE: token_type = Token::LEFT_BRACE; break;
    case ::DOT: token_type = Token::DOT; break;
    case ::DOT_DOT: token_type = Token::DOT_DOT; break;
    case ::COMMA: token_type = Token::COMMA; break;
    case ::SEMI_COLON: token_type = Token::SEMI_COLON; break;
    case ::COLON: token_type = Token::COLON; break;
    case ::RIGHT_ARROW: token_type = Token::RIGHT_ARROW; break;
    case ::LEFT_ARROW: token_type = Token::LEFT_ARROW; break;
    case ::CHIP: token_type = Token::CHIP; scan_identifier = true; break;
    case ::IMPORT: token_type = Token::IMPORT; scan_identifier = true; break;
    case ::WIRE: token_type = Token::WIRE; scan_identifier = true; break;
    case ::TRUE: token_type = Token::TRUE; scan_identifier = true; break;
    case ::FALSE: token_type = Token::FALSE; scan_identifier = true; break;
    case ::HIGH: token_type = Token::HIGH; scan_identifier = true; break;
    case ::LOW: token_type = Token::LOW; scan_identifier = true; break;
    case ::CC_COMMENT:
      token_type = Token::COMMENT;
      ReadLine(input_stream_);
      break;
    case ::CPP_COMMENT_PREFIX:
      if (ReadCPPCommentTail(input_stream_)) {
        token_type = Token::COMMENT;
      } else {
        input_stream_.Unget(kKeyWords[::CPP_COMMENT_PREFIX][1]);
        input_stream_.Unget(kKeyWords[::CPP_COMMENT_PREFIX][0]);
      }
      break;
    case ::HEX_PREFIX:
      input_stream_.Unget(kKeyWords[::HEX_PREFIX][1]);
      input_stream_.Unget(kKeyWords[::HEX_PREFIX][0]);
      if (ReadHexNumber(input_stream_, &token)) {
        token_type = Token::NUMBER_HEX;
      } else {
        scan_identifier = true;
      }
      break;
    case ::BINARY_PREFIX:
      input_stream_.Unget(kKeyWords[::BINARY_PREFIX][0]);
      if (ReadBinaryNumber(input_stream_, &token)) {
        token_type = Token::NUMBER_BINARY;
      } else {
        scan_identifier = true;
      }
      break;
    case ::UNKNOWN:
      if (ReadDecimalNumber(input_stream_, &token)) {
        token_type = Token::NUMBER_DECIMAL;
      } else {
        scan_identifier = true;
      }
      break;
    default: break;
  }

  // A keyword may have been read, when a full identifier could have been
  // lexed.
  if (scan_identifier) {
    // Nothing has been eaten, so consume a full identifier.
    if (Token::UNKNOWN == token_type) {
      if (ReadIdentifier(input_stream_, &token))
        token_type = Token::IDENTIFIER;
    } else {
      std::string tail;
      if (ReadIdentifierTail(input_stream_, &tail)) {
        token_type = Token::IDENTIFIER;
        token += tail;
      }
    }
  }

  // Read at least one element from the stream and report the unknown
  // token.
  if (Token::UNKNOWN == token_type) {
    if (!input_stream_.IsEOS())
      token += input_stream_.Get();
  }

  return Token(current_line, token_type, token);
}

bool TokenStream::IsEOS() const {
  SkipWhiteSpace(input_stream_);
  return input_stream_.IsEOS();
}
