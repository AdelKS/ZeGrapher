/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CALCULUSDEFINES_H
#define CALCULUSDEFINES_H

#include <QList>

#define OP_LOW 0
#define OP_HIGH 1
#define POW 2
#define FONC 3
#define PTHO 4
#define PTHF 5

#define NOMBRE 6
#define VAR 7

#define VARS_START 8

#define VAR_X 8
#define VAR_T 9
#define VAR_N 10
#define PAR_K 11
#define CONST_Pi_NAMES_START 12
#define CONST_Pi_NAMES_END 15

#define PLUS 16
#define MINUS 17
#define MULTIPLY 18
#define DIVIDE 19

#define SEQUENCES_START 20

#define SEQ_U 20
#define SEQ_V 21
#define SEQ_L 22
#define SEQ_W 23
#define SEQ_Q 24
#define SEQ_Z 25

#define SEQUENCES_END 25

#define INTEGRATION_FUNC_START 29

#define INTGR_F 29
#define INTGR_G 30
#define INTGR_H 31
#define INTGR_P 32
#define INTGR_R 33
#define INTGR_M 34

#define INTEGRATION_FUNC_END 34

#define DERIV_START 400

#define DERIV_F 400
#define DERIV_G 401
#define DERIV_H 402
#define DERIV_P 403
#define DERIV_R 404
#define DERIV_M 405

#define DERIV_END 405

#define FUNC_START 406

#define FUNC_F 406
#define FUNC_G 407
#define FUNC_H 408
#define FUNC_P 409
#define FUNC_R 410
#define FUNC_M 411

#define FUNC_END 411

#define REF_FUNC_START 412

#define ACOS 412
#define ASIN 413
#define ATAN 414
#define COS 415
#define SIN 416
#define TAN 417
#define SQRT 418
#define LOG 419
#define LN 420
#define ABS 421
#define EXP 422
#define FLOOR 423
#define CEIL 424
#define COSH 425
#define SINH 426
#define TANH 427
#define E 428 // E n = 10^n like 2E2 or 2e+2 or 2e2 = 2*10^2
#define e 429 // same as E

#define REF_FUNC_END 429

#endif // CALCULUSDEFINES_H
