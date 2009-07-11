// This file is distributed under the BSD License.
// See LICENSE.TXT for details.

#ifndef CHAISCRIPT_HPP_
#define CHAISCRIPT_HPP_

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <stdexcept>
#include <iostream>
#include <map>
#include <fstream>
#include <boost/shared_ptr.hpp>

#include "dispatchkit/dispatchkit.hpp"
#include "dispatchkit/bootstrap.hpp"
#include "dispatchkit/bootstrap_stl.hpp"

namespace chaiscript
{
    /**
     * Types of AST nodes available to the parser and eval
     */
    class Token_Type { public: enum Type { Error, Int, Float, Id, Char, Str, Eol, Fun_Call, Arg_List, Variable, Equation, Var_Decl,
        Expression, Comparison, Additive, Multiplicative, Negate, Not, Array_Call, Dot_Access, Quoted_String, Single_Quoted_String,
        Lambda, Block, Def, While, If, For, Inline_Array, Inline_Map, Return, File, Prefix, Break, Map_Pair, Value_Range,
        Inline_Range, Annotation }; };

    /**
     * Helper lookup to get the name of each node type
     */
    const char *token_type_to_string(int tokentype) {
        const char *token_types[] = { "Internal Parser Error", "Int", "Float", "Id", "Char", "Str", "Eol", "Fun_Call", "Arg_List", "Variable", "Equation", "Var_Decl",
            "Expression", "Comparison", "Additive", "Multiplicative", "Negate", "Not", "Array_Call", "Dot_Access", "Quoted_String", "Single_Quoted_String",
            "Lambda", "Block", "Def", "While", "If", "For", "Inline_Array", "Inline_Map", "Return", "File", "Prefix", "Break", "Map_Pair", "Value_Range",
            "Inline_Range", "Annotation"};

        return token_types[tokentype];
    }

    /**
     * Convenience type for file positions
     */
    struct File_Position {
        int line;
        int column;

        File_Position(int file_line, int file_column)
            : line(file_line), column(file_column) { }

        File_Position() : line(0), column(0) { }
    };

    typedef boost::shared_ptr<struct Token> TokenPtr;

    /**
     * The struct that doubles as both a parser token and an AST node
     */
    struct Token {
        std::string text;
        int identifier;
        const char *filename;
        File_Position start, end;

        std::vector<TokenPtr> children;
        TokenPtr annotation;

        Token(const std::string &token_text, int id, const char *fname) : text(token_text), identifier(id), filename(fname) { }

        Token(const std::string &token_text, int id, const char *fname, int start_line, int start_col, int end_line, int end_col) :
            text(token_text), identifier(id), filename(fname) {

            start.line = start_line;
            start.column = start_col;
            end.line = end_line;
            end.column = end_col;
        }
    };

    /**
     * Errors generated inside the parser
     */
    struct Parse_Error {
        std::string reason;
        File_Position position;
        const char *filename;

        Parse_Error(const std::string &why, const File_Position &where, const char *fname) :
            reason(why), position(where), filename(fname) { }

        Parse_Error(const std::string &why, const TokenPtr &where) : reason(why) {
            filename = where->filename;
            position = where->start;
        }

        virtual ~Parse_Error() throw() {}
    };

    /**
     * Errors generated inside the evaluator
     */
    struct Eval_Error : public std::runtime_error {
        std::string reason;
        TokenPtr location;

        Eval_Error(const std::string &why, const TokenPtr where)
          : std::runtime_error("Eval error: \"" + why + "\" in '"
              + where->filename + "' line: " + boost::lexical_cast<std::string>(where->start.line+1)),
            reason(why), location(where) { }

        virtual ~Eval_Error() throw() {}
    };

    /**
     * Special type for returned values
     */
    struct Return_Value {
        dispatchkit::Boxed_Value retval;
        TokenPtr location;

        Return_Value(const dispatchkit::Boxed_Value &return_value, const TokenPtr where) : retval(return_value), location(where) { }
    };

    /**
     * Special type indicating a call to 'break'
     */
    struct Break_Loop {
        TokenPtr location;

        Break_Loop(const TokenPtr where) : location(where) { }
    };
}

#include "language/chaiscript_eval.hpp"
#include "language/chaiscript_engine.hpp"

#endif /* CHAISCRIPT_HPP_ */
