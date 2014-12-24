/**
 *          File: exMain.c
 *
 *        Create: 2014年12月11日 星期四 17时06分05秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <string.h>

// global include
#include "BVPU_SDK.h"

//
// The example of main function
// init
// deinit
//

int main(int argc, char **args)
{
    BVPU_SDK_GlobalParam globalParam;
    memset(&globalParam, 0, sizeof(BVPU_SDK_GlobalParam));

    BVPU_SDK_Result result = BVPU_SDK_Init(&globalParam);
    if (result != BVPU_SDK_RESULT_S_OK) {
        return -1;
    }

    BVPU_SDK_Deinit();

    return 0;
}

/*=============== End of file: exMain.c ==========================*/
