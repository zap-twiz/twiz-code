#ifndef INCLUDED_PARSER_PARSE_ERROR_COLLECTION_H_
#define INCLUDED_PARSER_PARSE_ERROR_COLLECTION_H_

#include "parser/parse_error.h"

#include "base/base.h"
#include <vector>

class ParseErrorCollection {
 public:
  typedef std::vector<ParseError> ErrorArray;

  ParseErrorCollection() {}

  void RegisterError(Token const & token, std::string const & message) {
    parse_errors_.push_back(ParseError(token, message));
  }

  void RegisterErrorCollection(ParseErrorCollection const & error_collection) {
    // Add all of the errors in error collection to the local error set.
    ErrorArray::const_iterator error_iter(error_collection.errors().begin()),
        error_end(error_collection.errors().end());
    for (; error_iter != error_end; ++error_iter)
      RegisterError(error_iter->token(), error_iter->message());
  }

  ErrorArray const & errors() const { return parse_errors_; }

 private:
  ErrorArray parse_errors_;

  DISALLOW_COPY_AND_ASSIGN(ParseErrorCollection);
};

#endif  // INCLUDED_PARSER_PARSE_ERROR_COLLECTION_H_