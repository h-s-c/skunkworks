/*
** Copyright (c) 2012 Pierre-Marc Jobin
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
*/

#define VA_NARGS(...) VA_NARGS_IMPL(, ##__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N

#define VA_DECL(n, r, f, ...)       VA_DECL_(VA_DECL_, n, r, f, __VA_ARGS__)
#define VA_DECL_(p, n, r, f, ...)   f(p##n(__VA_ARGS__))
#define VA_DECL_0()
#define VA_DECL_1(_, ...)           _ a
#define VA_DECL_2(_, ...)           _ b, VA_DECL_1(__VA_ARGS__)
#define VA_DECL_3(_, ...)           _ c, VA_DECL_2(__VA_ARGS__)
#define VA_DECL_4(_, ...)           _ d, VA_DECL_3(__VA_ARGS__)
#define VA_DECL_5(_, ...)           _ e, VA_DECL_4(__VA_ARGS__)
#define VA_DECL_6(_, ...)           _ f, VA_DECL_5(__VA_ARGS__)
#define VA_DECL_7(_, ...)           _ g, VA_DECL_6(__VA_ARGS__)
#define VA_DECL_8(_, ...)           _ h, VA_DECL_7(__VA_ARGS__)
#define VA_DECL_9(_, ...)           _ i, VA_DECL_8(__VA_ARGS__)
#define VA_DECL_10(_, ...)          _ j, VA_DECL_9(__VA_ARGS__)
#define VA_DECL_11(_, ...)          _ k, VA_DECL_10(__VA_ARGS__)
#define VA_DECL_12(_, ...)          _ l, VA_DECL_11(__VA_ARGS__)
#define VA_DECL_13(_, ...)          _ m, VA_DECL_12(__VA_ARGS__)
#define VA_DECL_14(_, ...)          _ n, VA_DECL_13(__VA_ARGS__)
#define VA_DECL_15(_, ...)          _ o, VA_DECL_14(__VA_ARGS__)
#define VA_DECL_16(_, ...)          _ p, VA_DECL_15(__VA_ARGS__)

#define VA_CALL(n, r, f, ...)       VA_CALL_(VA_CALL_, n, r, f, __VA_ARGS__)
#define VA_CALL_(p, n, r, f, ...)   f(p##n(__VA_ARGS__))
#define VA_CALL_0()
#define VA_CALL_1(_, ...)           a
#define VA_CALL_2(_, ...)           b, VA_CALL_1(__VA_ARGS__)
#define VA_CALL_3(_, ...)           c, VA_CALL_2(__VA_ARGS__)
#define VA_CALL_4(_, ...)           d, VA_CALL_3(__VA_ARGS__)
#define VA_CALL_5(_, ...)           e, VA_CALL_4(__VA_ARGS__)
#define VA_CALL_6(_, ...)           f, VA_CALL_5(__VA_ARGS__)
#define VA_CALL_7(_, ...)           g, VA_CALL_6(__VA_ARGS__)
#define VA_CALL_8(_, ...)           h, VA_CALL_7(__VA_ARGS__)
#define VA_CALL_9(_, ...)           i, VA_CALL_8(__VA_ARGS__)
#define VA_CALL_10(_, ...)          j, VA_CALL_9(__VA_ARGS__)
#define VA_CALL_11(_, ...)          k, VA_CALL_10(__VA_ARGS__)
#define VA_CALL_12(_, ...)          l, VA_CALL_11(__VA_ARGS__)
#define VA_CALL_13(_, ...)          m, VA_CALL_12(__VA_ARGS__)
#define VA_CALL_14(_, ...)          n, VA_CALL_13(__VA_ARGS__)
#define VA_CALL_15(_, ...)          o, VA_CALL_14(__VA_ARGS__)
#define VA_CALL_16(_, ...)          p, VA_CALL_15(__VA_ARGS__)

/* Hats off to the mad hatter! */
