
#include <iostream>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


static int average(lua_State *L)
{
	/* get number of arguments */
	int n = lua_gettop(L);
	double sum = 0;
	int i;

	/* loop through each argument */
	for (i = 1; i <= n; i++)
	{
		/* total the arguments */
		sum += lua_tonumber(L, i);
	}

	std::cout << "lua call it" << std::endl;

	/* push the average */
	lua_pushnumber(L, sum / n);

	/* push the sum */
	lua_pushnumber(L, sum);

	/* return the number of results */
	return 2;
}


int main()
{

	lua_State* L;
	L =  luaL_newstate();
	luaL_openlibs(L);
	luaopen_base())
	lua_register(L , "average" , average);
	 luaL_loadfile(L, "test.lua");

//	r = lua_pcall(L ,0 , 0 , 0 );
	
	
	/* Çå³ý Lua */
	lua_close(L);
	
	return 0;
}