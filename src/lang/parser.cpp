// A Bison parser, made by GNU Bison 3.3.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2019 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.



// First part of user prologue.
#line 1 "grammar.y" // lalr1.cc:429

#include <iostream>
#include <string>
#include <cmath>
#include <stdarg.h>
#include "scanner.hpp"

#line 48 "parser.cpp" // lalr1.cc:429

#include "parser.hpp"


// Unqualified %code blocks.
#line 61 "grammar.y" // lalr1.cc:435

    namespace lang {
        void _emit(char *s, ...) {
		va_list ap;
        
			va_start(ap, s);
			vfprintf(stdout, s, ap);
			va_end(ap);
        }
    } // namespace lang

#line 66 "parser.cpp" // lalr1.cc:435


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 15 "grammar.y" // lalr1.cc:510
namespace lang {
#line 142 "parser.cpp" // lalr1.cc:510

  /// Build a parser object.
  Parser::Parser (yyscan_t scanner_yyarg, Emit *emit_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      emit (emit_yyarg)
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/

  // basic_symbol.
#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value ()
  {
    switch (this->type_get ())
    {
      case 6: // INTVAR
      case 7: // FLTVAR
        value.move< char > (std::move (that.value));
        break;

      case 5: // FLT
        value.move< double > (std::move (that.value));
        break;

      case 4: // INT
        value.move< long long > (std::move (that.value));
        break;

      case 3: // NAME
        value.move< std::string > (std::move (that.value));
        break;

      default:
        break;
    }

  }
#endif

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
  {
    switch (this->type_get ())
    {
      case 6: // INTVAR
      case 7: // FLTVAR
        value.copy< char > (YY_MOVE (that.value));
        break;

      case 5: // FLT
        value.copy< double > (YY_MOVE (that.value));
        break;

      case 4: // INT
        value.copy< long long > (YY_MOVE (that.value));
        break;

      case 3: // NAME
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }



  template <typename Base>
  bool
  Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->type_get ())
    {
      case 6: // INTVAR
      case 7: // FLTVAR
        value.move< char > (YY_MOVE (s.value));
        break;

      case 5: // FLT
        value.move< double > (YY_MOVE (s.value));
        break;

      case 4: // INT
        value.move< long long > (YY_MOVE (s.value));
        break;

      case 3: // NAME
        value.move< std::string > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

  }

  // by_type.
  Parser::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  Parser::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  Parser::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  void
  Parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  int
  Parser::by_type::type_get () const YY_NOEXCEPT
  {
    return type;
  }


  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_number_type
  Parser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.type_get ())
    {
      case 6: // INTVAR
      case 7: // FLTVAR
        value.YY_MOVE_OR_COPY< char > (YY_MOVE (that.value));
        break;

      case 5: // FLT
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case 4: // INT
        value.YY_MOVE_OR_COPY< long long > (YY_MOVE (that.value));
        break;

      case 3: // NAME
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.type_get ())
    {
      case 6: // INTVAR
      case 7: // FLTVAR
        value.move< char > (YY_MOVE (that.value));
        break;

      case 5: // FLT
        value.move< double > (YY_MOVE (that.value));
        break;

      case 4: // INT
        value.move< long long > (YY_MOVE (that.value));
        break;

      case 3: // NAME
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 6: // INTVAR
      case 7: // FLTVAR
        value.move< char > (that.value);
        break;

      case 5: // FLT
        value.move< double > (that.value);
        break;

      case 4: // INT
        value.move< long long > (that.value);
        break;

      case 3: // NAME
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, scanner));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 6: // INTVAR
      case 7: // FLTVAR
        yylhs.value.emplace< char > ();
        break;

      case 5: // FLT
        yylhs.value.emplace< double > ();
        break;

      case 4: // INT
        yylhs.value.emplace< long long > ();
        break;

      case 3: // NAME
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 3:
#line 76 "grammar.y" // lalr1.cc:919
    { emit->emit_item_name(FUNC_VOID,"FUNC_VOID",yystack_[2].value.as < std::string > ().c_str()); }
#line 709 "parser.cpp" // lalr1.cc:919
    break;

  case 4:
#line 77 "grammar.y" // lalr1.cc:919
    { emit->emit_item_name(FUNCTION,"FUNCTION",yystack_[3].value.as < std::string > ().c_str()); }
#line 715 "parser.cpp" // lalr1.cc:919
    break;

  case 5:
#line 78 "grammar.y" // lalr1.cc:919
    { emit->emit_item(ENC,"ENC"); }
#line 721 "parser.cpp" // lalr1.cc:919
    break;

  case 6:
#line 79 "grammar.y" // lalr1.cc:919
    { emit->emit_item(LIST,"LIST"); }
#line 727 "parser.cpp" // lalr1.cc:919
    break;

  case 7:
#line 80 "grammar.y" // lalr1.cc:919
    { emit->emit_item(ADD,"ADD"); }
#line 733 "parser.cpp" // lalr1.cc:919
    break;

  case 8:
#line 81 "grammar.y" // lalr1.cc:919
    { emit->emit_item(SUB,"SUB"); }
#line 739 "parser.cpp" // lalr1.cc:919
    break;

  case 9:
#line 82 "grammar.y" // lalr1.cc:919
    { emit->emit_item(AND,"AND"); }
#line 745 "parser.cpp" // lalr1.cc:919
    break;

  case 10:
#line 83 "grammar.y" // lalr1.cc:919
    { emit->emit_item(XOR,"XOR"); }
#line 751 "parser.cpp" // lalr1.cc:919
    break;

  case 11:
#line 84 "grammar.y" // lalr1.cc:919
    { emit->emit_item(OR,"OR"); }
#line 757 "parser.cpp" // lalr1.cc:919
    break;

  case 12:
#line 85 "grammar.y" // lalr1.cc:919
    { emit->emit_item(MUL,"MUL"); }
#line 763 "parser.cpp" // lalr1.cc:919
    break;

  case 13:
#line 86 "grammar.y" // lalr1.cc:919
    { emit->emit_item(DIV,"DIV"); }
#line 769 "parser.cpp" // lalr1.cc:919
    break;

  case 14:
#line 87 "grammar.y" // lalr1.cc:919
    { emit->emit_item(MOD,"MOD"); }
#line 775 "parser.cpp" // lalr1.cc:919
    break;

  case 15:
#line 88 "grammar.y" // lalr1.cc:919
    { emit->emit_item_name(ASSIGN,"ASSIGN",yystack_[2].value.as < std::string > ().c_str()); }
#line 781 "parser.cpp" // lalr1.cc:919
    break;

  case 16:
#line 89 "grammar.y" // lalr1.cc:919
    { emit->emit_item(EQUAL,"EQUAL"); }
#line 787 "parser.cpp" // lalr1.cc:919
    break;

  case 17:
#line 90 "grammar.y" // lalr1.cc:919
    { emit->emit_item(NEQUAL,"NEQUAL"); }
#line 793 "parser.cpp" // lalr1.cc:919
    break;

  case 18:
#line 91 "grammar.y" // lalr1.cc:919
    { emit->emit_item(LT,"LT"); }
#line 799 "parser.cpp" // lalr1.cc:919
    break;

  case 19:
#line 92 "grammar.y" // lalr1.cc:919
    { emit->emit_item(GT,"GT"); }
#line 805 "parser.cpp" // lalr1.cc:919
    break;

  case 20:
#line 93 "grammar.y" // lalr1.cc:919
    { emit->emit_item(LTE,"LTE"); }
#line 811 "parser.cpp" // lalr1.cc:919
    break;

  case 21:
#line 94 "grammar.y" // lalr1.cc:919
    { emit->emit_item(GTE,"GTE"); }
#line 817 "parser.cpp" // lalr1.cc:919
    break;

  case 22:
#line 95 "grammar.y" // lalr1.cc:919
    { emit->emit_item_name(NAME,"NAME",yystack_[0].value.as < std::string > ().c_str()); }
#line 823 "parser.cpp" // lalr1.cc:919
    break;

  case 23:
#line 96 "grammar.y" // lalr1.cc:919
    { emit->emit_item_number(NUMBER,"NUMBER",yystack_[0].value.as < long long > ()); }
#line 829 "parser.cpp" // lalr1.cc:919
    break;

  case 24:
#line 97 "grammar.y" // lalr1.cc:919
    { emit->emit_item(END,"END"); }
#line 835 "parser.cpp" // lalr1.cc:919
    break;

  case 25:
#line 98 "grammar.y" // lalr1.cc:919
    { emit->emit_item(INDENT,"INDENT"); }
#line 841 "parser.cpp" // lalr1.cc:919
    break;

  case 26:
#line 99 "grammar.y" // lalr1.cc:919
    { emit->emit_item(LF,"LF"); }
#line 847 "parser.cpp" // lalr1.cc:919
    break;

  case 27:
#line 100 "grammar.y" // lalr1.cc:919
    { emit->emit_item(IFTHENELSE,"IFTHENELS"); }
#line 853 "parser.cpp" // lalr1.cc:919
    break;

  case 28:
#line 101 "grammar.y" // lalr1.cc:919
    { emit->emit_item(IFTHEN,"IFTHEN"); }
#line 859 "parser.cpp" // lalr1.cc:919
    break;


#line 863 "parser.cpp" // lalr1.cc:919
            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yysyntax_error_ (yystack_[0].state, yyla));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }


      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char Parser::yypact_ninf_ = -6;

  const signed char Parser::yytable_ninf_ = -1;

  const short
  Parser::yypact_[] =
  {
      35,    12,    -6,    -6,    -6,    35,    -6,    35,   147,    24,
      35,    27,    97,    55,    35,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    35,    35,    35,    -6,
     147,   147,    -6,    76,    35,    -6,   161,   169,   169,   169,
     169,   169,   169,    13,    13,    13,    13,    13,    -6,    -6,
      -6,    -6,   115,    35,    -6,   132,    -6
  };

  const unsigned char
  Parser::yydefact_[] =
  {
       2,    22,    23,    26,    24,     2,    25,     2,    29,     2,
       2,     2,     0,     0,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
      30,    15,     3,     0,     2,     5,     6,    16,    17,    18,
      19,    20,    21,     7,     8,     9,    11,    10,    12,    13,
      14,     4,     0,     2,    28,     0,    27
  };

  const signed char
  Parser::yypgoto_[] =
  {
      -6,    -5,    -6
  };

  const signed char
  Parser::yydefgoto_[] =
  {
      -1,     8,     9
  };

  const unsigned char
  Parser::yytable_[] =
  {
      12,     0,    13,     0,    30,    31,    33,     0,     0,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    29,     0,    10,     1,     2,    52,
       1,     2,     3,     4,     5,     3,     4,     5,     1,     2,
      26,    27,    28,     3,     4,     5,    11,     0,    55,     0,
       0,     0,     0,     0,     0,     0,     0,     6,     7,     0,
       6,     7,    32,     0,     0,     0,     0,     0,     6,     7,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,     0,     0,     0,     0,     0,
      35,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,     0,     0,    34,     0,
       0,    51,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    53,    54,     0,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    56,     0,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    21,    22,    23,    24,    25,    26,    27,    28
  };

  const signed char
  Parser::yycheck_[] =
  {
       5,    -1,     7,    -1,     9,    10,    11,    -1,    -1,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,     0,    -1,    14,     3,     4,    34,
       3,     4,     8,     9,    10,     8,     9,    10,     3,     4,
      27,    28,    29,     8,     9,    10,    34,    -1,    53,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    -1,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    33,    34,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    -1,    -1,    -1,    -1,    -1,
      35,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    -1,    -1,    11,    -1,
      -1,    35,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    12,    13,    -1,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    13,    -1,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    22,    23,    24,    25,    26,    27,    28,    29
  };

  const unsigned char
  Parser::yystos_[] =
  {
       0,     3,     4,     8,     9,    10,    33,    34,    37,    38,
      14,    34,    37,    37,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,     0,
      37,    37,    35,    37,    11,    35,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    35,    37,    12,    13,    37,    13
  };

  const unsigned char
  Parser::yyr1_[] =
  {
       0,    36,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    38,
      38
  };

  const unsigned char
  Parser::yyr2_[] =
  {
       0,     2,     0,     3,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     1,     1,     1,     1,     7,     5,     1,
       2
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "$end", "error", "$undefined", "NAME", "INT", "FLT", "INTVAR", "FLTVAR",
  "LF", "END", "IF", "THEN", "ELSE", "FI", "'='", "LIST", "EQUAL",
  "NEQUAL", "LT", "GT", "LTE", "GTE", "'+'", "'-'", "B_AND", "B_OR",
  "B_XOR", "MUL", "DIV", "MOD", "UMINUS", "FACTORIAL", "EXPONENT",
  "INDENT", "'('", "')'", "$accept", "expr", "stmt_list", YY_NULLPTR
  };


  const unsigned char
  Parser::yyrline_[] =
  {
       0,    75,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   104,
     105
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  Parser::token_number_type
  Parser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const token_number_type
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      34,    35,     2,    22,     2,    23,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    14,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    15,
      16,    17,    18,    19,    20,    21,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33
    };
    const unsigned user_token_number_max_ = 285;
    const token_number_type undef_token_ = 2;

    if (static_cast<int> (t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 15 "grammar.y" // lalr1.cc:1242
} // lang
#line 1261 "parser.cpp" // lalr1.cc:1242
#line 108 "grammar.y" // lalr1.cc:1243


void lang::Parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}
