// typedef struct __attribute__((packed)) AR_HEADER{
//     char file_identifier[16];
//     char file_mod_timestamp[12];
//     char owner[6];
//     char group_id[6];
//     char file_mode[8];
//     char file_size[10];
//     char ending_chars[2];
// } AR_HEADER;

typedef struct __attribute__((packed)) AR_HEADER{
    char *file_identifier;
    int file_size;
} AR_HEADER;