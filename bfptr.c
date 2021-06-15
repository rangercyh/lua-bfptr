#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <math.h>
#include <stdlib.h>

static lua_Number lbfptr(lua_Number *arr, lua_Integer p, lua_Integer r, lua_Integer i);

static int cmpfunc(const void *a, const void *b) {
    return (*(lua_Number *)a - *(lua_Number *)b);
}

static lua_Number computeMedian(lua_Number *arr, lua_Integer begin, lua_Integer end) {
    qsort(arr + begin, end - begin + 1, sizeof(lua_Number), cmpfunc);
    return arr[begin + (end - begin) / 2];
}

static lua_Number median_of_medians(lua_Number *arr, lua_Integer p, lua_Integer r) {
    lua_Integer num = r - p + 1;
    lua_Integer extra = num % 5 == 0 ? 0 : 1;
    lua_Integer length = num / 5 + extra;
    lua_Number *medians = (lua_Number *)malloc(sizeof(lua_Number) * length);
    for (lua_Integer i = 0; i < length; i++) {
        medians[i] = computeMedian(arr, p + i * 5, (lua_Integer)fmin(p + i * 5 + 4, r));
    }
    return lbfptr(medians, 0, length - 1, length / 2);
}

static void partition(lua_Number *arr, lua_Integer p, lua_Integer r, lua_Number pivot, lua_Integer *pivotRange) {
    lua_Integer small = p - 1;
    lua_Integer equal = 0;
    lua_Number temp;
    for (lua_Integer j = p; j <= r; j++) {
        if (arr[j] < pivot) {
            small++;
            temp = arr[small];
            if (equal > 0) {
                arr[j] = arr[small + equal];
                arr[small + equal] = temp;
            } else {
                arr[j] = temp;
            }
        } else if (arr[j] == pivot) {
            equal++;
            temp = arr[j];
            arr[j] = arr[small + equal];
            arr[small + equal] = temp;
        }
    }
    pivotRange[0] = small + 1;
    pivotRange[1] = small + equal;
}

static lua_Number lbfptr(lua_Number *arr, lua_Integer p, lua_Integer r, lua_Integer i) {
    if (p == r) {
        return arr[p];
    }
    lua_Number pivot = median_of_medians(arr, p, r);
    lua_Integer pivotRange[2] = { 0 };
    partition(arr, p, r, pivot, pivotRange);
    if (p + i >= pivotRange[0] && p + i <= pivotRange[1]) {
        return arr[pivotRange[0]];
    } else if (p + i < pivotRange[0]) {
        return lbfptr(arr, p, pivotRange[0] - 1, i);
    } else {
        return lbfptr(arr, pivotRange[1] + 1, r, i + p - pivotRange[1] - 1);
    }
}

static int ltopk(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Integer n = luaL_len(L, 1);
    if (n > 0) {
        luaL_argcheck(L, n < LUA_MAXINTEGER, 1, "array too big");
        lua_Integer k = luaL_checkinteger(L, 2);
        if (k > n || k < 1) {
            luaL_error(L, "k range is: 0 < k <= %d(table length)", n);
            return 0;
        }
        lua_settop(L, 2);
        lua_Number *arr = (lua_Number *)malloc(sizeof(lua_Number) * n);
        for (lua_Integer i = 0; i < n; i++) {
            lua_geti(L, 1, i + 1);
            arr[i] = luaL_checknumber(L, -1);
            lua_pop(L, 1);
        }
        lua_pushnumber(L, lbfptr(arr, 0, n - 1, k - 1));
        return 1;
    }
    return 0;
}

static const luaL_Reg bfptr_funcs[] = {
    { "topk", ltopk },
    { NULL, NULL }
};

int
luaopen_bfptr(lua_State *L)
{
    luaL_checkversion(L);
    luaL_newlib(L, bfptr_funcs);
    return 1;
}
