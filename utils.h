#pragma once

#define SBX_SAFE_CALL(f, msg)                       \
    {                                               \
        int status = f;                             \
        if(status == -1 || status == 1) {           \
        perror(msg);                                \
        return 1;                                   \
    }                                               \
}     

#define SBX_SAFE_CALL_DEALLOC(f, msg)               \
    {                                               \
        int status = f;                             \
        if(status == -1 || status == 1) {           \
        perror(msg);                                \
        free(GUARDED);                              \
        return 1;                                   \
    }                                               \
}  

                            