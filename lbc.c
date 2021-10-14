/*
* lbc.c
* big-number library for Lua based on GNU bc core library
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 24 Jul 2018 23:29:43
* This code is hereby placed in the public domain and also under the MIT license
*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "number.h"

#include "lua.h"
#include "lauxlib.h"
#include "mycompat.h"

#define MYNAME		"bc"
#define MYVERSION	MYNAME " library for " LUA_VERSION " / Jul 2018 / "\
			"based on GNU bc-1.07"
#define MYTYPE		MYNAME " bignumber"

static int digits=0;
static lua_State *LL=NULL;

void bc_error(const char *mesg)
{
 luaL_error(LL,"(bc) %s",mesg ? mesg : "not enough memory");
}

static void Pnew(lua_State *L, bc_num x)
{
 luaL_boxpointer(L,x);
 luaL_setmetatable(L,MYTYPE);
}

static bc_num Pget(lua_State *L, int i)
{
 LL=L;
 luaL_checkany(L,i);
 switch (lua_type(L,i))
 {
  case LUA_TNUMBER:
  case LUA_TSTRING:
  {
   bc_num x=NULL;
   const char *s=lua_tostring(L,i);
   for (; isspace(*s); s++);		/* bc_str2num chokes on spaces */
   bc_str2num(&x,(char*)s,digits);
   if (bc_is_zero(x))			/* bc_str2num chokes on sci notation */
   {
	char *t=strchr(s,'e');
	if (t==NULL) t=strchr(s,'E');
	if (t!=NULL)
	{
		bc_num y=NULL,n=NULL;
		int c=*t; *t=0;		/* harmless const violation! */
		bc_str2num(&x,(char*)s,digits);
		*t=c;
		bc_int2num(&y,10);
		bc_int2num(&n,atoi(t+1));
		bc_raise(y,n,&y,digits);
		bc_multiply(x,y,&x,digits);
		bc_free_num(&y);
		bc_free_num(&n);
	}
   }
   Pnew(L,x);
   lua_replace(L,i);
   return x;
  }
  default:
   return luaL_unboxpointer(L,i,MYTYPE);
 }
 return NULL;
}

static int Lnew(lua_State *L)			/** new(x) */
{
 Pget(L,1);
 lua_settop(L,1);
 return 1;
}

static int Ldigits(lua_State *L)		/** digits([n]) */
{
 lua_pushinteger(L,digits);
 digits=luaL_optinteger(L,1,digits);
 return 1;
}

static int Ltostring(lua_State *L)		/** tostring(x) */
{
 bc_num a=Pget(L,1);
 char *s=bc_num2str(a);
 lua_pushstring(L,s);
 free(s);
 return 1;
}

static int Ltonumber(lua_State *L)		/** tonumber(x) */
{
 Ltostring(L);
 lua_pushnumber(L,lua_tonumber(L,-1));
 return 1;
}

static int Lcompare(lua_State *L)		/** compare(x,y) */
{
 bc_num a=Pget(L,1);
 bc_num b=Pget(L,2);
 lua_pushinteger(L,bc_compare(a,b));
 return 1;
}

static int Leq(lua_State *L)
{
 bc_num a=Pget(L,1);
 bc_num b=Pget(L,2);
 lua_pushboolean(L,bc_compare(a,b)==0);
 return 1;
}

static int Lle(lua_State *L)
{
 bc_num a=Pget(L,1);
 bc_num b=Pget(L,2);
 lua_pushboolean(L,bc_compare(a,b)<=0);
 return 1;
}

static int Llt(lua_State *L)
{
 bc_num a=Pget(L,1);
 bc_num b=Pget(L,2);
 lua_pushboolean(L,bc_compare(a,b)<0);
 return 1;
}

static int Liszero(lua_State *L)		/** iszero(x) */
{
 bc_num a=Pget(L,1);
 lua_pushboolean(L,bc_is_zero(a));
 return 1;
}

static int Lisneg(lua_State *L)			/** isneg(x) */
{
 bc_num a=Pget(L,1);
 lua_pushboolean(L,bc_is_neg(a));
 return 1;
}

static int Pneg(lua_State *L, bc_num x)
{
 bc_num a=bc_zero;
 bc_num c=NULL;
 bc_sub(a,x,&c,digits);
 Pnew(L,c);
 return 1;
}

static int Lneg(lua_State *L)			/** neg(x) */
{
 bc_num a=Pget(L,1);
 return Pneg(L,a);
}

static int Labs(lua_State *L)			/** abs(x) */
{
 bc_num a=Pget(L,1);
 if (bc_is_neg(a))
  return Pneg(L,a);
 else
 {
  lua_settop(L,1);
  return 1;
 }
}

static int Pdo1(lua_State *L, void (*f)(bc_num a, bc_num b, bc_num *c, int n))
{
 bc_num a=Pget(L,1);
 bc_num b=Pget(L,2);
 bc_num c=NULL;
 //f(a,b,&c,digits);
 //f(a,b,&c,a->n_scale); /** The output will be minimum scale **/
 f(a,b,&c,0); /** The output will be minimum scale of a or b **/
 Pnew(L,c);
 return 1;
}

static int Ladd(lua_State *L)			/** add(x,y) */
{
 return Pdo1(L,bc_add);
}

static int Lsub(lua_State *L)			/** sub(x,y) */
{
 return Pdo1(L,bc_sub);
}

static int Lmul(lua_State *L)			/** mul(x,y) */
{
 return Pdo1(L,bc_multiply);
}

static int Lpow(lua_State *L)			/** pow(x,y) */
{
 return Pdo1(L,bc_raise);
}

static int Ldiv(lua_State *L)			/** div(x,y) */
{
 bc_num a=Pget(L,1);
 bc_num b=Pget(L,2);
 bc_num c=NULL;
 if (bc_divide(a,b,&c,digits)!=0) return 0;
 Pnew(L,c);
 return 1;
}

static int Lmod(lua_State *L)			/** mod(x,y) */
{
 bc_num a=Pget(L,1);
 bc_num b=Pget(L,2);
 bc_num c=NULL;
 if (bc_modulo(a,b,&c,0)!=0) return 0;
 Pnew(L,c);
 return 1;
}

static int Lquotrem(lua_State *L)		/** quotrem(x,y) */
{
 bc_num a=Pget(L,1);
 bc_num b=Pget(L,2);
 bc_num q=NULL;
 bc_num r=NULL;
 if (bc_divmod(a,b,&q,&r,0)!=0) return 0;
 Pnew(L,q);
 Pnew(L,r);
 return 2;
}

static int Lpowmod(lua_State *L)		/** powmod(x,y,m) */
{
 bc_num a=Pget(L,1);
 bc_num k=Pget(L,2);
 bc_num m=Pget(L,3);
 bc_num c=NULL;
 if (bc_raisemod(a,k,m,&c,0)!=0) return 0;
 Pnew(L,c);
 return 1;
}

static int Lsqrt(lua_State *L)			/** sqrt(x) */
{
 bc_num a=Pget(L,1);
 bc_num b=bc_zero;
 bc_num c=NULL;
 bc_add(a,b,&c,digits);				/* bc_sqrt works inplace! */
 if (bc_sqrt(&c,digits)==0) return 0;
 Pnew(L,c);
 return 1;
}

static int Ltrunc(lua_State *L)			/** trunc(x,[n]) */
{
 bc_num a=Pget(L,1);
 bc_num b=bc_one;
 bc_num c=NULL;
 int n=luaL_optinteger(L,2,0);
 if (n<0) n=0;
 bc_divide(a,b,&c,n);
 Pnew(L,c);
 return 1;
}

static int Lgc(lua_State *L)
{
 bc_num x=Pget(L,1);
 bc_free_num(&x);
 lua_pushnil(L);
 lua_setmetatable(L,1);
 return 0;
}

static const luaL_Reg R[] =
{
	{ "__add",	Ladd	},		/** __add(x,y) */
	{ "__div",	Ldiv	},		/** __div(x,y) */
	{ "__eq",	Leq	},		/** __eq(x,y) */
	{ "__gc",	Lgc	},
	{ "__le",	Lle	},		/** __le(x,y) */
	{ "__lt",	Llt	},		/** __lt(x,y) */
	{ "__mod",	Lmod	},		/** __mod(x,y) */
	{ "__mul",	Lmul	},		/** __mul(x,y) */
	{ "__pow",	Lpow	},		/** __pow(x,y) */
	{ "__sub",	Lsub	},		/** __sub(x,y) */
	{ "__tostring",	Ltostring},		/** __tostring(x) */
	{ "__unm",	Lneg	},		/** __unm(x) */
	{ "abs",	Labs	},
	{ "add",	Ladd	},
	{ "compare",	Lcompare},
	{ "digits",	Ldigits	},
	{ "div",	Ldiv	},
	{ "quotrem",	Lquotrem},
	{ "isneg",	Lisneg	},
	{ "iszero",	Liszero	},
	{ "mod",	Lmod	},
	{ "mul",	Lmul	},
	{ "neg",	Lneg	},
	{ "new",	Lnew	},
	{ "pow",	Lpow	},
	{ "powmod",	Lpowmod	},
	{ "sqrt",	Lsqrt	},
	{ "sub",	Lsub	},
	{ "tonumber",	Ltonumber},
	{ "tostring",	Ltostring},
	{ "trunc",	Ltrunc	},
	{ NULL,		NULL	}
};

LUALIB_API int luaopen_bc(lua_State *L)
{
 bc_init_numbers();
 luaL_newmetatable(L,MYTYPE);
 luaL_setfuncs(L,R,0);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_settable(L,-3);
 lua_pushliteral(L,"__index");
 lua_pushvalue(L,-2);
 lua_settable(L,-3);
 return 1;
}
