#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <math.h>

static int lbfptr(int *arr, int p, int r, int i);

static int computeMedian(int *arr, int begin, int end) {
    Array.sort(arr, begin, end + 1);    // java
    return arr[begin + (end - begin) / 2];
}

static int median_of_medians(int *arr, int p, int r) {
    int num = r - p + 1;
    int extra = num % 5 == 0 ? 0 : 1;
    int length = num / 5 + extra;
    int medians[length] = { 0 };
    for (int i = 0; i < length; i++) {
        medians[i] = computeMedian(arr, p + i * 5; (int)fmin(p + i * 5 + 4, r));
    }
    return lbfptr(medians, 0, length - 1, length / 2);
}

static int* partition(int *arr, int p, int r, int pivot) {
    int small = p - 1;
    int equal = 0;
    int temp;
    for (int j = p; j <= r; j++) {
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
    return { small + 1, small + equal };    // java
}

static int lbfptr(int *arr, int p, int r, int i) {
    if (p == r) {
        return arr[p];
    }
    int pivot = median_of_medians(arr, p, r);
    int *pivotRange = partition(arr, p, r, pivot);
    if (p + i >= pivotRange[0] && p + i <= pivotRange[1]) {
        return arr[pivotRange[0]];
    } else if (p + i < pivotRange[0]) {
        return lbfptr(arr, p, pivotRange[0] - 1, i);
    } else {
        return lbfptr(arr, pivotRange[1] + 1, r, i + p - pivotRange[1] - 1);
    }
}

static const luaL_Reg bfptr[] = {
    { "bfptr", lbfptr },
    { NULL, NULL }
};

int
luaopen_bfptr(lua_State *L)
{
    luaL_checkversion(L);
    luaL_newlib(L, bfptr);
    return 1;
}
