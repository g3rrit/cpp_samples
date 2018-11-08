#include "src/util/c99_stdint.h"
#include <string>

#include "src/code/output.h"
#include "src/conf/msg.h"
#include "src/re/encoding/enc.h"
#include "src/ast/scanner.h"
#include "src/util/s_to_n32_unsafe.h"

namespace re2c
{

// global re2c config (affects the whole file)
/*!re2c
    re2c:define:YYCTYPE     = "unsigned char";
    re2c:define:YYCURSOR    = cur;
    re2c:define:YYLIMIT     = lim;
    re2c:define:YYMARKER    = mar;
    re2c:define:YYCTXMARKER = ctx;
    re2c:define:YYFILL      = fill;

    // source code is in ASCII: pointers have type 'char *'
    // but re2c makes an implicit assumption that YYCTYPE is unsigned
    // when it generates comparisons
    re2c:yych:conversion = 1;

    space = [ \t];

    conf_assign = space* "=" space*;

    naked_char = [^\x00\n] \ (space | [;]);
    naked      = (naked_char \ ['"]) naked_char*;

    number = "0" | ("-"? [1-9] [0-9]*);
*/

void Scanner::lex_conf(Opt &opts)
{
    tok = cur;
    const uint32_t l = get_cline(), c = get_column();
/*!re2c
    "flags:" ("b" | "bit-vectors")       { opts.set_bFlag(lex_conf_bool());              return; }
    "flags:" ("d" | "debug-output")      { opts.set_dFlag(lex_conf_bool());              return; }
    "flags:" ("g" | "computed-gotos")    { opts.set_gFlag(lex_conf_bool());              return; }
    "flags:" ("i" | "no-debug-info")     { opts.set_iFlag(lex_conf_bool());              return; }
    "flags:" ("s" | "nested-ifs")        { opts.set_sFlag(lex_conf_bool());              return; }
    "flags:" ("T" | "tags")              { opts.set_tags(lex_conf_bool());               return; }
    "flags:" ("P" | "posix-captures")    { opts.set_posix_captures(lex_conf_bool());     return; }
    "flags:case-insensitive"             { opts.set_bCaseInsensitive(lex_conf_bool());   return; }
    "flags:case-inverted"                { opts.set_bCaseInverted(lex_conf_bool());      return; }
    "flags:lookahead"                    { opts.set_lookahead(lex_conf_bool());          return; }
    "flags:optimize-tags"                { opts.set_optimize_tags(lex_conf_bool());      return; }
    "flags:eager-skip"                   { opts.set_eager_skip(lex_conf_bool());         return; }

    "flags:" ("e" | "ecb")        { lex_conf_enc(Enc::EBCDIC, opts); return; }
    "flags:" ("u" | "unicode")    { lex_conf_enc(Enc::UTF32, opts);  return; }
    "flags:" ("w" | "wide-chars") { lex_conf_enc(Enc::UCS2, opts);   return; }
    "flags:" ("x" | "utf-16")     { lex_conf_enc(Enc::UTF16, opts);  return; }
    "flags:" ("8" | "utf-8")      { lex_conf_enc(Enc::UTF8, opts);   return; }

    "flags:encoding-policy"  { lex_conf_encoding_policy(opts);  return; }
    "flags:input"            { lex_conf_input(opts);            return; }
    "flags:empty-class"      { lex_conf_empty_class(opts);      return; }
    "flags:dfa-minimization" { lex_conf_dfa_minimization(opts); return; }

    "define:YYCONDTYPE"           { opts.set_yycondtype       (lex_conf_string ()); return; }
    "define:YYGETCONDITION"       { opts.set_cond_get         (lex_conf_string ()); return; }
    "define:YYGETCONDITION:naked" { opts.set_cond_get_naked   (lex_conf_bool()); return; }
    "define:YYSETCONDITION"       { opts.set_cond_set         (lex_conf_string ()); return; }
    "define:YYSETCONDITION@cond"  { opts.set_cond_set_arg     (lex_conf_string ()); return; }
    "define:YYSETCONDITION:naked" { opts.set_cond_set_naked   (lex_conf_bool()); return; }
    "condprefix"                  { opts.set_condPrefix       (lex_conf_string ()); return; }
    "condenumprefix"              { opts.set_condEnumPrefix   (lex_conf_string ()); return; }
    "cond:divider"                { opts.set_condDivider      (lex_conf_string ()); return; }
    "cond:divider@cond"           { opts.set_condDividerParam (lex_conf_string ()); return; }
    "cond:goto"                   { opts.set_condGoto         (lex_conf_string ()); return; }
    "cond:goto@cond"              { opts.set_condGotoParam    (lex_conf_string ()); return; }
    "variable:yyctable"           { opts.set_yyctable         (lex_conf_string ()); return; }

    "define:YYGETSTATE"       { opts.set_state_get       (lex_conf_string ()); return; }
    "define:YYGETSTATE:naked" { opts.set_state_get_naked (lex_conf_bool()); return; }
    "define:YYSETSTATE"       { opts.set_state_set       (lex_conf_string ()); return; }
    "define:YYSETSTATE:naked" { opts.set_state_set_naked (lex_conf_bool()); return; }
    "define:YYSETSTATE@state" { opts.set_state_set_arg   (lex_conf_string ()); return; }
    "label:yyFillLabel"       { opts.set_yyfilllabel     (lex_conf_string ()); return; }
    "label:yyNext"            { opts.set_yynext          (lex_conf_string ()); return; }
    "state:abort"             { opts.set_bUseStateAbort  (lex_conf_bool()); return; }
    "state:nextlabel"         { opts.set_bUseStateNext   (lex_conf_bool()); return; }
    "variable:yyaccept"       { opts.set_yyaccept        (lex_conf_string ()); return; }

    "variable:yybm"     { opts.set_yybm         (lex_conf_string ()); return; }
    "yybm:hex"          { opts.set_yybmHexTable (lex_conf_bool()); return; }
    "cgoto:threshold"
    {
        const int32_t n = lex_conf_number ();
        if (n < 0)
        {
            fatal_lc(l, c, "configuration 'cgoto:threshold' must be nonnegative");
        }
        opts.set_cGotoThreshold (static_cast<uint32_t> (n));
        return;
    }
    "variable:yytarget" { opts.set_yytarget (lex_conf_string ()); return; }

    "define:YYCURSOR"    { opts.set_yycursor    (lex_conf_string ()); return; }
    "define:YYMARKER"    { opts.set_yymarker    (lex_conf_string ()); return; }
    "define:YYCTXMARKER" { opts.set_yyctxmarker (lex_conf_string ()); return; }
    "define:YYLIMIT"     { opts.set_yylimit     (lex_conf_string ()); return; }

    "define:YYPEEK"       { opts.set_yypeek       (lex_conf_string ()); return; }
    "define:YYSKIP"       { opts.set_yyskip       (lex_conf_string ()); return; }
    "define:YYBACKUP"     { opts.set_yybackup     (lex_conf_string ()); return; }
    "define:YYBACKUPCTX"  { opts.set_yybackupctx  (lex_conf_string ()); return; }
    "define:YYRESTORE"    { opts.set_yyrestore    (lex_conf_string ()); return; }
    "define:YYRESTORECTX" { opts.set_yyrestorectx (lex_conf_string ()); return; }
    "define:YYRESTORETAG" { opts.set_yyrestoretag (lex_conf_string ()); return; }
    "define:YYLESSTHAN"   { opts.set_yylessthan   (lex_conf_string ()); return; }
    "define:YYSTAGN"      { opts.set_yystagn      (lex_conf_string ()); return; }
    "define:YYSTAGP"      { opts.set_yystagp      (lex_conf_string ()); return; }
    "define:YYMTAGN"      { opts.set_yymtagn      (lex_conf_string ()); return; }
    "define:YYMTAGP"      { opts.set_yymtagp      (lex_conf_string ()); return; }

    "tags:prefix"     { opts.set_tags_prefix    (lex_conf_string ()); return; }
    "tags:expression" { opts.set_tags_expression(lex_conf_string ()); return; }

    "indent:string" { opts.set_indString (lex_conf_string ()); return; }
    "indent:top"
    {
        const int32_t n = lex_conf_number ();
        if (n < 0)
        {
            fatal_lc(l, c, "configuration 'indent:top' must be nonnegative");
        }
        opts.set_topIndent (static_cast<uint32_t> (n));
        return;
    }

    "define:YYDEBUG" { opts.set_yydebug (lex_conf_string ()); return; }

    "define:YYCTYPE"  { opts.set_yyctype        (lex_conf_string ()); return; }
    "variable:yych"   { opts.set_yych           (lex_conf_string ()); return; }
    "yych:conversion" { opts.set_yychConversion (lex_conf_bool()); return; }
    "yych:emit"       { opts.set_bEmitYYCh      (lex_conf_bool()); return; }

    "define:YYFILL"       { opts.set_fill         (lex_conf_string ()); return; }
    "yyfill:enable"       { opts.set_fill_use     (lex_conf_bool()); return; }
    "define:YYFILL@len"   { opts.set_fill_arg     (lex_conf_string ()); return; }
    "yyfill:parameter"    { opts.set_fill_arg_use (lex_conf_bool()); return; }
    "define:YYFILL:naked" { opts.set_fill_naked   (lex_conf_bool()); return; }
    "yyfill:check"        { opts.set_fill_check   (lex_conf_bool()); return; }

    "labelprefix" { opts.set_labelPrefix (lex_conf_string ()); return; }

    // try to lex number first, otherwize it would be lexed as a naked string
    "startlabel" / conf_assign number { opts.set_startlabel_force (lex_conf_bool());   return; }
    "startlabel"                      { opts.set_startlabel       (lex_conf_string()); return; }

    // deprecated
    "variable:yystable" { lex_conf_string (); return; }

    [a-zA-Z0-9_:-]* {
        fatal_lc(l, c, "unrecognized configuration '%.*s'",
            static_cast<int>(cur - tok), tok);
    }
*/
}

void Scanner::lex_conf_encoding_policy(Opt &opts)
{
    lex_conf_assign ();
/*!re2c
    * { fatal_lc(get_cline(), get_column(),
        "bad configuration value (expected: 'ignore', 'substitute', 'fail')"); }
    "ignore"     { opts.set_encoding_policy(Enc::POLICY_IGNORE);     goto end; }
    "substitute" { opts.set_encoding_policy(Enc::POLICY_SUBSTITUTE); goto end; }
    "fail"       { opts.set_encoding_policy(Enc::POLICY_FAIL);       goto end; }
*/
end:
    lex_conf_semicolon();
}

void Scanner::lex_conf_input(Opt &opts)
{
    lex_conf_assign ();
/*!re2c
    * { fatal_lc(get_cline(), get_column(),
        "bad configuration value (expected: 'default', 'custom')"); }
    "default" { opts.set_input_api(INPUT_DEFAULT); goto end; }
    "custom"  { opts.set_input_api(INPUT_CUSTOM);  goto end; }
*/
end:
    lex_conf_semicolon();
}

void Scanner::lex_conf_empty_class(Opt &opts)
{
    lex_conf_assign ();
/*!re2c
    * { fatal_lc(get_cline(), get_column(),
        "bad configuration value (expected: 'match-empty', 'match-none', 'error')"); }
    "match-empty" { opts.set_empty_class_policy(EMPTY_CLASS_MATCH_EMPTY); goto end; }
    "match-none"  { opts.set_empty_class_policy(EMPTY_CLASS_MATCH_NONE);  goto end; }
    "error"       { opts.set_empty_class_policy(EMPTY_CLASS_ERROR);       goto end; }
*/
end:
    lex_conf_semicolon();
}

void Scanner::lex_conf_dfa_minimization(Opt &opts)
{
    lex_conf_assign ();
/*!re2c
    * { fatal_lc(get_cline(), get_column(),
        "bad configuration value (expected: 'table', 'moore')"); }
    "table" { opts.set_dfa_minimization(DFA_MINIMIZATION_TABLE); goto end; }
    "moore" { opts.set_dfa_minimization(DFA_MINIMIZATION_MOORE); goto end; }
*/
end:
    lex_conf_semicolon();
}

void Scanner::lex_conf_enc(Enc::type_t enc, Opt &opts)
{
    if (lex_conf_bool()) {
        opts.set_encoding(enc);
    } else {
        opts.unset_encoding(enc);
    }
}

void Scanner::lex_conf_assign ()
{
/*!re2c
    * { fatal_lc(get_cline(), get_column(), "missing '=' in configuration"); }
    conf_assign { return; }
*/
}

void Scanner::lex_conf_semicolon ()
{
/*!re2c
    * { fatal_lc(get_cline(), get_column(), "missing ending ';' in configuration"); }
    space* ";" { return; }
*/
}

bool Scanner::lex_conf_bool()
{
    return lex_conf_number() != 0;
}

int32_t Scanner::lex_conf_number ()
{
    lex_conf_assign ();
    tok = cur;
/*!re2c
    number {
        int32_t n = 0;
        if (!s_to_i32_unsafe (tok, cur, n)) {
            fatal_lc(get_cline(), get_column(), "configuration value overflow");
        }
        lex_conf_semicolon ();
        return n;
    }
*/
}

std::string Scanner::lex_conf_string ()
{
    lex_conf_assign ();
    std::string s;
    tok = cur;
/*!re2c
    ['"] {
        const char quote = tok[0];
        for (bool end;;) {
            const uint32_t c = lex_str_chr(quote, end);
            if (end) {
                goto end;
            }
            if (c > 0xFF) {
                fatal_lc(get_cline(), get_column(),
                    "multibyte character in configuration string: 0x%X", c);
            } else {
                s += static_cast<char>(c);
            }
        }
    }
    naked {
        s = std::string(tok, tok_len());
        goto end;
    }
    "" { goto end; }
*/
end:
    lex_conf_semicolon ();
    return s;
}

} // end namespace re2c
